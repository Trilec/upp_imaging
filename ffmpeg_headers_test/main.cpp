#include <Core/Core.h>

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/version.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/version.h>
#include <libavformat/avformat.h>
#include <libavformat/version.h>
#include <libswscale/swscale.h>
#include <libswscale/version.h>
}

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

	Check(state, AV_HAVE_BIGENDIAN == 0,
	      "FFmpeg public headers use little-endian configuration");
	Check(state, AV_HAVE_FAST_UNALIGNED == 1,
	      "FFmpeg public headers use fast unaligned access");
	Check(state, LIBAVUTIL_VERSION_MAJOR == 61,
	      "libavutil major matches FFmpeg 9.0.1");
	Check(state, LIBAVCODEC_VERSION_MAJOR == 63,
	      "libavcodec major matches FFmpeg 9.0.1");
	Check(state, LIBAVFORMAT_VERSION_MAJOR == 63,
	      "libavformat major matches FFmpeg 9.0.1");
	Check(state, LIBSWSCALE_VERSION_MAJOR == 10,
	      "libswscale major matches FFmpeg 9.0.1");

	AVFrame* frame = nullptr;
	AVCodecContext* codec = nullptr;
	AVFormatContext* format = nullptr;
	SwsContext* scale = nullptr;
	Check(state, frame == nullptr && codec == nullptr &&
	      format == nullptr && scale == nullptr,
	      "core FFmpeg public types are visible together");

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
