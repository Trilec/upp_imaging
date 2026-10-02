#include "ImagingVideo.h"
#include <FFmpeg/FFmpeg.h>
extern "C" {
#include <libavutil/mathematics.h>
}
#include <chrono>
#include <climits>

namespace Upp { namespace Imaging {

static String MediaError(int code)
{
	char text[AV_ERROR_MAX_STRING_SIZE] = {};
	av_strerror(code, text, sizeof(text));
	return text;
}

struct VideoReader::Impl {
	AVFormatContext* format = nullptr;
	AVCodecContext* codec = nullptr;
	AVPacket* packet = nullptr;
	AVFrame* frame = nullptr;
	SwsContext* scaler = nullptr;
	VideoInputLimits limits;
	int stream_index = -1;
	Size size;
	int64 duration_ms = 0, seek_target_ms = -1, position_ms = 0;
	bool flushed = false, eof = false;
	std::chrono::steady_clock::time_point deadline;

	~Impl() {
		sws_freeContext(scaler);
		av_frame_free(&frame);
		av_packet_free(&packet);
		avcodec_free_context(&codec);
		avformat_close_input(&format);
	}
	void BeginOperation() {
		deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(limits.operation_ms);
	}
	static int Interrupt(void* opaque) {
		return std::chrono::steady_clock::now() >= static_cast<Impl*>(opaque)->deadline;
	}
	bool ValidSize(int w, int h) const {
		return w > 0 && h > 0 && w <= limits.dimension && h <= limits.dimension &&
		       int64(w) * h <= limits.pixels && int64(w) * h <= INT_MAX / 4;
	}
	int64 StartTime() const {
		int64 value = format->streams[stream_index]->start_time;
		return value == AV_NOPTS_VALUE ? 0 : value;
	}
};

VideoReader::VideoReader() {}
VideoReader::~VideoReader() {}

bool VideoReader::Open(const String& path)
{
	error.Clear();
	if(limits.file_bytes <= 0 || limits.pixels <= 0 || limits.dimension <= 0 ||
	   limits.packet_bytes <= 0 || limits.packets_per_frame <= 0 ||
	   limits.operation_ms <= 0 || limits.operation_ms > 30000 ||
	   path.StartsWith("\\\\") || path.StartsWith("//") ||
	   !FileExists(path) || GetFileLength(path) <= 0 || GetFileLength(path) > limits.file_bytes) {
		error = "video must be a bounded local file with valid limits";
		return false;
	}
	auto next = std::make_unique<Impl>();
	next->limits = limits;
	next->BeginOperation();
	next->format = avformat_alloc_context();
	if(!next->format) { error = "video context allocation failed"; return false; }
	next->format->interrupt_callback = {Impl::Interrupt, next.get()};
	next->format->max_streams = 16;
	next->format->max_index_size = 16 * 1024 * 1024;
	next->format->probesize = 1024 * 1024;
	next->format->max_analyze_duration = 2 * AV_TIME_BASE;
	AVDictionary* options = nullptr;
	av_dict_set(&options, "protocol_whitelist", "file", 0);
	int result = avformat_open_input(&next->format, path.Begin(), av_find_input_format("mov"), &options);
	av_dict_free(&options);
	if(result < 0) { error = MediaError(result); return false; }
	// MOV/MP4 must describe the supported track in its header. Stream-info
	// probing can open/decode every stream before our decoder budgets apply.
	// Do not probe: reject incomplete headers and cap our only decoder first.
	next->stream_index = av_find_best_stream(next->format, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	if(next->stream_index < 0) { error = "no supported video stream"; return false; }
	AVCodecParameters* parameters = next->format->streams[next->stream_index]->codecpar;
	if(parameters->codec_id != AV_CODEC_ID_H264 || !next->ValidSize(parameters->width, parameters->height)) {
		error = "only H.264 within the video dimension/pixel limits is supported";
		return false;
	}
	const AVCodec* decoder = avcodec_find_decoder(AV_CODEC_ID_H264);
	next->codec = decoder ? avcodec_alloc_context3(decoder) : nullptr;
	if(!next->codec) { error = "H.264 decoder allocation failed"; return false; }
	result = avcodec_parameters_to_context(next->codec, parameters);
	next->codec->thread_count = 1;
	next->codec->max_pixels = limits.pixels;
	if(result >= 0) result = avcodec_open2(next->codec, decoder, nullptr);
	if(result < 0) { error = MediaError(result); return false; }
	next->packet = av_packet_alloc();
	next->frame = av_frame_alloc();
	if(!next->packet || !next->frame) { error = "video packet/frame allocation failed"; return false; }
	next->size = Size(parameters->width, parameters->height);
	AVStream* stream = next->format->streams[next->stream_index];
	if(stream->duration != AV_NOPTS_VALUE)
		next->duration_ms = av_rescale_q(stream->duration, stream->time_base, AVRational{1, 1000});
	else if(next->format->duration != AV_NOPTS_VALUE)
		next->duration_ms = av_rescale_q(next->format->duration, AV_TIME_BASE_Q, AVRational{1, 1000});
	state = std::move(next);
	return true;
}

bool VideoReader::ReadNext(VideoFrame& output)
{
	error.Clear();
	if(!state) { error = "video is not open"; return false; }
	Impl& s = *state;
	if(s.eof) return false;
	s.BeginOperation();
	for(int attempts = 0; attempts < s.limits.packets_per_frame; ++attempts) {
		if(Impl::Interrupt(&s)) { error = "video operation time budget exceeded"; return false; }
		int result = avcodec_receive_frame(s.codec, s.frame);
		if(result == AVERROR_EOF) { s.eof = true; return false; }
		if(result == 0) {
			if(!s.ValidSize(s.frame->width, s.frame->height)) {
				av_frame_unref(s.frame); error = "decoded frame exceeds video limits"; return false;
			}
			int64 timestamp = s.frame->best_effort_timestamp;
			int64 time = timestamp == AV_NOPTS_VALUE ? s.position_ms :
				av_rescale_q(timestamp - s.StartTime(), s.format->streams[s.stream_index]->time_base, AVRational{1, 1000});
			if(s.seek_target_ms >= 0 && (timestamp == AV_NOPTS_VALUE || time < s.seek_target_ms)) {
				av_frame_unref(s.frame);
				continue;
			}
			s.scaler = sws_getCachedContext(s.scaler, s.frame->width, s.frame->height,
				(AVPixelFormat)s.frame->format, s.frame->width, s.frame->height,
				AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);
			if(!s.scaler) { av_frame_unref(s.frame); error = "video conversion allocation failed"; return false; }
			const int* coefficients = sws_getCoefficients(s.frame->colorspace == AVCOL_SPC_BT709 ? SWS_CS_ITU709 : SWS_CS_DEFAULT);
			result = sws_setColorspaceDetails(s.scaler, coefficients, s.frame->color_range == AVCOL_RANGE_JPEG,
				coefficients, 1, 0, 1 << 16, 1 << 16);
			if(result < 0) { av_frame_unref(s.frame); error = "video colour conversion setup failed"; return false; }
			Vector<byte> rgba;
			rgba.SetCount(s.frame->width * s.frame->height * 4);
			uint8_t* destination[] = {rgba.Begin(), nullptr, nullptr, nullptr};
			int strides[] = {s.frame->width * 4, 0, 0, 0};
			int rows = sws_scale(s.scaler, s.frame->data, s.frame->linesize, 0, s.frame->height, destination, strides);
			Size size(s.frame->width, s.frame->height);
			av_frame_unref(s.frame);
			if(rows != size.cy) { error = "incomplete video frame conversion"; return false; }
			ImageBuffer image(size);
			RGBA* pixels = image.Begin();
			for(int i = 0; i < size.cx * size.cy; ++i) {
				pixels[i].r = rgba[i * 4]; pixels[i].g = rgba[i * 4 + 1];
				pixels[i].b = rgba[i * 4 + 2]; pixels[i].a = 255;
			}
			image.SetKind(IMAGE_OPAQUE);
			VideoFrame completed;
			completed.image = Image(image);
			completed.time_ms = max<int64>(0, time);
			output = completed;
			s.position_ms = completed.time_ms;
			s.size = size; s.seek_target_ms = -1;
			return true;
		}
		if(result != AVERROR(EAGAIN)) { error = MediaError(result); return false; }
		if(s.flushed) { error = "decoder requested input after flush"; return false; }
		av_packet_unref(s.packet);
		result = av_read_frame(s.format, s.packet);
		if(result == AVERROR_EOF) {
			result = avcodec_send_packet(s.codec, nullptr);
			s.flushed = true;
		}
		else if(result >= 0) {
			if(s.packet->stream_index != s.stream_index) continue;
			if(s.packet->size < 0 || s.packet->size > s.limits.packet_bytes) {
				av_packet_unref(s.packet); error = "video packet exceeds limit"; return false;
			}
			result = avcodec_send_packet(s.codec, s.packet);
			av_packet_unref(s.packet);
		}
		if(result < 0) { error = MediaError(result); return false; }
	}
	error = "video packet/frame work budget exceeded";
	return false;
}

bool VideoReader::Seek(int64 time_ms)
{
	error.Clear();
	if(!state || time_ms < 0 || (state->duration_ms > 0 && time_ms >= state->duration_ms)) {
		error = "video seek is outside the clip"; return false;
	}
	Impl& s = *state; s.BeginOperation();
	int64 timestamp = av_rescale_q(time_ms, AVRational{1, 1000}, s.format->streams[s.stream_index]->time_base) + s.StartTime();
	int result = av_seek_frame(s.format, s.stream_index, timestamp, AVSEEK_FLAG_BACKWARD);
	if(result < 0) { error = MediaError(result); return false; }
	avcodec_flush_buffers(s.codec);
	av_packet_unref(s.packet); av_frame_unref(s.frame);
	s.flushed = s.eof = false;
	s.seek_target_ms = time_ms;
	return true;
}

void VideoReader::Close() { state.reset(); error.Clear(); }
bool VideoReader::IsOpen() const { return bool(state); }
bool VideoReader::IsEof() const { return state && state->eof; }
Size VideoReader::GetSize() const { return state ? state->size : Size(0, 0); }
int64 VideoReader::GetDurationMs() const { return state ? state->duration_ms : 0; }

}}
