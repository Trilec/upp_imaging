#include <Core/Core.h>

extern "C" {
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
#include <libswscale/version.h>
}

#include <cstdlib>

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

static bool Near(int value, int expected, int tolerance = 2)
{
	return value >= expected - tolerance && value <= expected + tolerance;
}

CONSOLE_APP_MAIN
{
	State state;

	Check(state, swscale_version() == LIBSWSCALE_VERSION_INT,
	      "libswscale runtime version matches headers");
	Check(state, String(swscale_license()).Find("LGPL") >= 0,
	      "libswscale reports LGPL configuration");
	String configuration = swscale_configuration();
	Check(state, configuration.Find("--enable-swscale") >= 0 &&
	             configuration.Find("--disable-asm") >= 0,
	      "libswscale reports scalar decode configuration");

	SwsContext* sws = sws_getContext(2, 2, AV_PIX_FMT_YUV420P,
	                                 2, 2, AV_PIX_FMT_RGBA,
	                                 SWS_POINT, nullptr, nullptr, nullptr);
	Check(state, sws != nullptr, "YUV420P to RGBA8 context creation works");
	const int* coeff = sws_getCoefficients(SWS_CS_ITU601);
	int colorspace_result = sws ? sws_setColorspaceDetails(sws, coeff, 0, coeff, 1,
	                                                       0, 1 << 16, 1 << 16) : -1;
	Check(state, colorspace_result == 0,
	      "limited-range ITU-601 conversion policy is explicit");

	uint8_t y[4] = {16, 82, 145, 235};
	uint8_t u[1] = {128};
	uint8_t v[1] = {128};
	const uint8_t* src[4] = {y, u, v, nullptr};
	int src_stride[4] = {2, 1, 1, 0};
	uint8_t rgba[16] = {};
	uint8_t* dst[4] = {rgba, nullptr, nullptr, nullptr};
	int dst_stride[4] = {8, 0, 0, 0};
	int rows = sws ? sws_scale(sws, src, src_stride, 0, 2, dst, dst_stride) : -1;
	Check(state, rows == 2, "libswscale converts both source rows");

	bool opaque = true;
	bool neutral = true;
	for(int i = 0; i < 4; ++i) {
		opaque = opaque && rgba[i * 4 + 3] == 255;
		neutral = neutral && std::abs((int)rgba[i * 4 + 0] - (int)rgba[i * 4 + 1]) <= 1 &&
		                     std::abs((int)rgba[i * 4 + 1] - (int)rgba[i * 4 + 2]) <= 1;
	}
	Check(state, opaque, "RGBA conversion emits opaque alpha");
	Check(state, neutral, "neutral chroma remains grayscale");
	Check(state, rgba[0] <= 2, "limited-range black maps to black");
	Check(state, rgba[12] >= 253, "limited-range white maps to white");
	Check(state, Near(rgba[4], 77), "first midtone conversion is deterministic");
	Check(state, Near(rgba[8], 150), "second midtone conversion is deterministic");
	Check(state, rgba[0] < rgba[4] && rgba[4] < rgba[8] && rgba[8] < rgba[12],
	      "grayscale conversion remains monotonic");

	if(sws)
		sws_freeContext(sws);

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
