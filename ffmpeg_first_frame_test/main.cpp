#include <Core/Core.h>
#include <FFmpeg/FFmpeg.h>
#include "Fixture.h"

using namespace Upp;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const char* label)
{
	Cout() << (condition ? "PASS " : "FAIL ") << label << '\n';
	(condition ? state.passed : state.failed)++;
}

static void FNV1a64Update(unsigned long long& h, unsigned char value)
{
	h ^= value;
	h *= 1099511628211ULL;
}

static unsigned long long FNV1a64(const unsigned char* data, int size)
{
	unsigned long long h = 1469598103934665603ULL;
	for(int i = 0; i < size; ++i)
		FNV1a64Update(h, data[i]);
	return h;
}

static unsigned long long LogicalYUV420PHash(const AVFrame* frame)
{
	unsigned long long h = 1469598103934665603ULL;
	for(int y = 0; y < frame->height; ++y)
		for(int x = 0; x < frame->width; ++x)
			FNV1a64Update(h, frame->data[0][y * frame->linesize[0] + x]);
	for(int plane = 1; plane <= 2; ++plane)
		for(int y = 0; y < frame->height / 2; ++y)
			for(int x = 0; x < frame->width / 2; ++x)
				FNV1a64Update(h, frame->data[plane][y * frame->linesize[plane] + x]);
	return h;
}

static bool Near(int value, int expected, int tolerance = 2)
{
	return value >= expected - tolerance && value <= expected + tolerance;
}

CONSOLE_APP_MAIN
{
	State state;
	String fixture_path = AppendFileName(GetTempPath(), "upp_imaging_ffmpeg_first_frame.mp4");
	DeleteFile(fixture_path);

	Check(state, kFfmpegFirstFrameFixtureSize == 1463,
	      "embedded MP4 fixture size is exact");
	Check(state, FNV1a64((const unsigned char*)kFfmpegFirstFrameFixture, kFfmpegFirstFrameFixtureSize) ==
	             kFfmpegFirstFrameFixtureFNV1a64,
	      "embedded MP4 fixture hash is exact");
	Check(state, SaveFile(fixture_path,
	                      String((const char*)kFfmpegFirstFrameFixture,
	                             kFfmpegFirstFrameFixtureSize)),
	      "embedded MP4 fixture writes to local temporary file");
	Check(state, FileExists(fixture_path), "local MP4 fixture exists");

	AVFormatContext* format = nullptr;
	int open_result = avformat_open_input(&format, fixture_path.Begin(), nullptr, nullptr);
	Check(state, open_result == 0 && format != nullptr,
	      "MOV/MP4 input opens through direct FFmpeg package");
	Check(state, format && format->iformat && String(format->iformat->name).Find("mov") >= 0,
	      "MOV demuxer selected for embedded MP4");

	int stream_info_result = format ? avformat_find_stream_info(format, nullptr) : AVERROR(EINVAL);
	Check(state, stream_info_result >= 0, "stream information resolves");

	int video_index = -1;
	if(format) {
		for(unsigned i = 0; i < format->nb_streams; ++i) {
			if(format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
				video_index = (int)i;
				break;
			}
		}
	}
	Check(state, video_index >= 0, "video stream is present");
	AVCodecParameters* parameters = video_index >= 0 ? format->streams[video_index]->codecpar : nullptr;
	Check(state, parameters && parameters->codec_id == AV_CODEC_ID_H264,
	      "video stream is H.264");
	Check(state, parameters && parameters->width == 16 && parameters->height == 16,
	      "container reports 16x16 video dimensions");

	const AVCodec* decoder = avcodec_find_decoder(AV_CODEC_ID_H264);
	Check(state, decoder && String(decoder->name) == "h264",
	      "native H.264 decoder resolves");
	AVCodecContext* codec = decoder ? avcodec_alloc_context3(decoder) : nullptr;
	Check(state, codec != nullptr, "H.264 decoder context allocates");
	int parameter_result = codec && parameters ? avcodec_parameters_to_context(codec, parameters)
	                                          : AVERROR(EINVAL);
	Check(state, parameter_result == 0, "container parameters copy to decoder context");
	int codec_open_result = codec && parameter_result == 0 ? avcodec_open2(codec, decoder, nullptr)
	                                                       : AVERROR(EINVAL);
	Check(state, codec_open_result == 0, "native H.264 decoder opens");

	AVPacket* packet = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();
	Check(state, packet != nullptr && frame != nullptr, "packet and frame allocate");

	bool got_frame = false;
	if(format && codec && packet && frame && video_index >= 0) {
		while(av_read_frame(format, packet) >= 0) {
			if(packet->stream_index == video_index) {
				int send_result = avcodec_send_packet(codec, packet);
				if(send_result >= 0) {
					int receive_result = avcodec_receive_frame(codec, frame);
					if(receive_result == 0)
						got_frame = true;
				}
			}
			av_packet_unref(packet);
			if(got_frame)
				break;
		}
		if(!got_frame) {
			avcodec_send_packet(codec, nullptr);
			if(avcodec_receive_frame(codec, frame) == 0)
				got_frame = true;
		}
	}
	Check(state, got_frame, "first H.264 video frame decodes");
	Check(state, got_frame && frame->width == 16 && frame->height == 16,
	      "decoded frame dimensions are exact");
	Check(state, got_frame && frame->format == AV_PIX_FMT_YUV420P,
	      "decoded frame format is YUV420P");
	Check(state, got_frame && frame->format == AV_PIX_FMT_YUV420P &&
	             LogicalYUV420PHash(frame) == kFfmpegFirstFrameYUVFNV1a64,
	      "decoded logical YUV420P frame hash is exact");

	unsigned char rgba[16 * 16 * 4] = {};
	SwsContext* sws = got_frame ? sws_getContext(frame->width, frame->height,
	                                             (AVPixelFormat)frame->format,
	                                             16, 16, AV_PIX_FMT_RGBA,
	                                             SWS_POINT, nullptr, nullptr, nullptr)
	                            : nullptr;
	Check(state, sws != nullptr, "scalar swscale RGBA context creates");
	const int* coefficients = sws_getCoefficients(SWS_CS_ITU601);
	int colorspace_result = sws ? sws_setColorspaceDetails(sws, coefficients, 0,
	                                                       coefficients, 1,
	                                                       0, 1 << 16, 1 << 16)
	                            : AVERROR(EINVAL);
	Check(state, colorspace_result == 0,
	      "limited-range ITU-601 to full-range RGBA policy is explicit");

	unsigned char* destination[4] = {rgba, nullptr, nullptr, nullptr};
	int destination_stride[4] = {16 * 4, 0, 0, 0};
	int converted_rows = sws ? sws_scale(sws, frame->data, frame->linesize,
	                                     0, frame->height,
	                                     destination, destination_stride)
	                         : -1;
	Check(state, converted_rows == 16, "all decoded rows convert to RGBA8");

	bool opaque = true;
	bool uniform = true;
	for(int i = 0; i < 16 * 16; ++i) {
		opaque = opaque && rgba[i * 4 + 3] == 255;
		uniform = uniform && rgba[i * 4 + 0] == rgba[0] &&
		                     rgba[i * 4 + 1] == rgba[1] &&
		                     rgba[i * 4 + 2] == rgba[2] &&
		                     rgba[i * 4 + 3] == rgba[3];
	}
	Check(state, opaque, "decoded RGBA frame is fully opaque");
	Check(state, uniform, "solid-colour fixture remains spatially uniform");
	Check(state, Near(rgba[0], 49) && Near(rgba[1], 100) && Near(rgba[2], 201) &&
	             rgba[3] == 255,
	      "decoded first pixel matches expected RGBA colour");

	if(sws)
		sws_freeContext(sws);
	av_frame_free(&frame);
	av_packet_free(&packet);
	avcodec_free_context(&codec);
	if(format)
		avformat_close_input(&format);
	DeleteFile(fixture_path);
	Check(state, !FileExists(fixture_path), "temporary MP4 fixture cleans up");
	Check(state, format == nullptr && codec == nullptr && frame == nullptr && packet == nullptr,
	      "FFmpeg decode resources release cleanly");

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
