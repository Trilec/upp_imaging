#include <Core/Core.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavformat/version.h>
}

#include <cstring>

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

CONSOLE_APP_MAIN
{
	State state;

	Check(state, avformat_version() == LIBAVFORMAT_VERSION_INT,
	      "libavformat runtime version matches headers");
	Check(state, String(avformat_license()).Find("LGPL") >= 0,
	      "libavformat reports LGPL configuration");
	String configuration = avformat_configuration();
	Check(state, configuration.Find("--enable-demuxer=mov") >= 0 &&
	             configuration.Find("--enable-protocol=file") >= 0,
	      "libavformat reports MOV/file configuration");

	const AVInputFormat* mov = av_find_input_format("mov");
	Check(state, mov != nullptr, "MOV demuxer is registered");
	void* demux_iterator = nullptr;
	const AVInputFormat* first_demux = av_demuxer_iterate(&demux_iterator);
	const AVInputFormat* second_demux = av_demuxer_iterate(&demux_iterator);
	Check(state, first_demux == mov && second_demux == nullptr,
	      "demuxer registry contains exactly MOV");
	void* mux_iterator = nullptr;
	Check(state, av_muxer_iterate(&mux_iterator) == nullptr,
	      "muxer registry remains empty");

	void* input_protocol_iterator = nullptr;
	const char* input_protocol = avio_enum_protocols(&input_protocol_iterator, 0);
	const char* second_input_protocol = avio_enum_protocols(&input_protocol_iterator, 0);
	Check(state, input_protocol && String(input_protocol) == "file" &&
	             second_input_protocol == nullptr,
	      "input protocol registry contains exactly file");
	void* output_protocol_iterator = nullptr;
	const char* output_protocol = avio_enum_protocols(&output_protocol_iterator, 1);
	const char* second_output_protocol = avio_enum_protocols(&output_protocol_iterator, 1);
	Check(state, output_protocol && String(output_protocol) == "file" &&
	             second_output_protocol == nullptr,
	      "output protocol registry contains exactly file");

	String fixture = AppendFileName(GetTempPath(), "ffmpeg_avformat_file_test.bin");
	const char payload[] = "not-a-mov-but-a-file-protocol-fixture";
	Check(state, SaveFile(fixture, String(payload, (int)sizeof(payload) - 1)),
	      "local file fixture creation");

	AVIOContext* io = nullptr;
	int open_result = avio_open(&io, fixture.Begin(), AVIO_FLAG_READ);
	Check(state, open_result == 0 && io != nullptr,
	      "FFmpeg file protocol opens local UTF-8 path");
	uint8_t readback[sizeof(payload) - 1] = {};
	int read_result = io ? avio_read(io, readback, (int)sizeof(readback)) : AVERROR(EINVAL);
	Check(state, read_result == (int)sizeof(readback) &&
	             memcmp(readback, payload, sizeof(readback)) == 0,
	      "FFmpeg file protocol reads exact bytes");
	int close_result = io ? avio_closep(&io) : AVERROR(EINVAL);
	Check(state, close_result == 0 && io == nullptr,
	      "FFmpeg file protocol closes deterministically");

	AVFormatContext* rejected = nullptr;
	int mov_result = avformat_open_input(&rejected, fixture.Begin(), mov, nullptr);
	Check(state, mov_result < 0,
	      "malformed MOV input is rejected cleanly");
	if(rejected)
		avformat_close_input(&rejected);

	DeleteFile(fixture);
	Check(state, !FileExists(fixture), "local file fixture cleanup");

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
