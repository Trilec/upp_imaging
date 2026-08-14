#include <Core/Core.h>

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/buffer.h>
#include <libavutil/cpu.h>
#include <libavutil/file.h>
#include <libavutil/frame.h>
#include <libavutil/mathematics.h>
#include <libavutil/mem.h>
#include <libavutil/time.h>
#include <libavutil/version.h>
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

	Check(state, avutil_version() == LIBAVUTIL_VERSION_INT,
	      "libavutil runtime version matches headers");
	Check(state, String(av_version_info()) == "9.0.1",
	      "FFmpeg release version is pinned");
	Check(state, String(avutil_license()).Find("LGPL") >= 0,
	      "libavutil reports LGPL configuration");
	String configuration = avutil_configuration();
	Check(state, configuration.Find("--enable-decoder=h264") >= 0 &&
	             configuration.Find("--disable-asm") >= 0,
	      "libavutil reports the bounded scalar decode configuration");
	Check(state, av_get_cpu_flags() == 0,
	      "scalar configuration exposes no SIMD dispatch flags");

	AVBufferRef* buffer = av_buffer_alloc(64);
	Check(state, buffer && buffer->size == 64,
	      "libavutil buffer allocation works");
	av_buffer_unref(&buffer);

	AVFrame* frame = av_frame_alloc();
	Check(state, frame != nullptr, "libavutil frame allocation works");
	av_frame_free(&frame);

	AVRational frame_time = {1, 25};
	Check(state, av_rescale_q(10, frame_time, AV_TIME_BASE_Q) == 400000,
	      "libavutil rational rescaling is deterministic");
	Check(state, av_gettime() > 0, "Win32 libavutil clock is available");

	void* memory = av_malloc(128);
	Check(state, memory != nullptr, "libavutil aligned allocation works");
	av_free(memory);

	String fixture = AppendFileName(GetTempPath(), "ffmpeg_avutil_map_test.bin");
	const char payload[] = "upp-ffmpeg-avutil";
	Check(state, SaveFile(fixture, String(payload, (int)sizeof(payload) - 1)),
	      "file-map fixture creation");
	uint8_t* mapped = nullptr;
	size_t mapped_size = 0;
	int map_result = av_file_map(fixture.Begin(), &mapped, &mapped_size, 0, nullptr);
	Check(state, map_result == 0 && mapped && mapped_size == sizeof(payload) - 1 &&
	             memcmp(mapped, payload, mapped_size) == 0,
	      "Win32 UTF-8 file mapping roundtrip");
	if(map_result == 0)
		av_file_unmap(mapped, mapped_size);
	DeleteFile(fixture);
	Check(state, !FileExists(fixture), "file-map fixture cleanup");

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
