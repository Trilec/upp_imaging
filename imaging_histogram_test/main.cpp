#include <cmath>
#include <cstdio>
#include <cfloat>

#include <imaging_histogram/imaging_histogram.h>

using namespace Upp;

struct Result {
	int passed = 0;
	int failed = 0;
};

static bool Check(Result& result, bool condition, const char* label)
{
	std::printf("%s: %s\n", label, condition ? "PASS" : "FAIL");
	if(condition) ++result.passed; else ++result.failed;
	return condition;
}

int main()
{
	Result result;

	// ---- Single-channel uniform
	{
		int W = 4, H = 4, C = 1;
		Vector<float> buf(W * H * C, 0.5f);
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, -1, -1, -1, 0);
		Check(result, hd.IsValid(), "single uniform: IsValid");
		Check(result, hd.channels.GetCount() == 1, "single uniform: 1 channel");
		Check(result, hd.total_samples == 16, "single uniform: all in range");
		Check(result, hd.non_finite == 0, "single uniform: no nan");
		Check(result, hd.below_range == 0, "single uniform: no below");
		Check(result, hd.above_range == 0, "single uniform: no above");
		Check(result, hd.channels[0][128] == 16, "single uniform: bin128 = 16");
	}

	// ---- Black-to-white ramp 256x1
	{
		int W = 256, H = 1, C = 1;
		Vector<float> buf(W * H * C);
		for(int i = 0; i < W; ++i)
			buf[i] = i / 255.0f;
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, -1, -1, -1, 0);
		Check(result, hd.IsValid(), "ramp 256: IsValid");
		Check(result, hd.total_samples == 255, "ramp 256: 255 in-range (1.0 clips above)");
		int non_empty = 0;
		for(int i = 0; i < hd.bins; ++i)
			if(hd.channels[0][i] > 0) ++non_empty;
		Check(result, non_empty == 255, "ramp 256: 255 bins filled");
		Check(result, std::fabs(hd.mean - 0.5) < 0.005, "ramp 256: mean ~0.5");
	}

	// ---- Below / above / NaN values
	{
		int W = 1, H = 5, C = 1;
		Vector<float> buf(W * H * C);
		buf[0] = -0.5f;
		buf[1] = 2.0f;
		buf[2] = std::nanf("");
		buf[3] = INFINITY;
		buf[4] = 0.5f;
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, -1, -1, -1, 0);
		Check(result, hd.IsValid(), "edge vals: IsValid");
		Check(result, hd.total_samples == 1, "edge vals: one in-range");
		Check(result, hd.below_range == 1, "edge vals: one below");
		Check(result, hd.above_range == 1, "edge vals: one above");
		Check(result, hd.non_finite == 2, "edge vals: two non-finite");
	}

	// ---- RGB channels
	{
		int W = 1, H = 2, C = 3;
		Vector<float> buf(W * H * C);
		buf[0] = 0.1f; buf[1] = 0.2f; buf[2] = 0.3f;
		buf[3] = 0.4f; buf[4] = 0.5f; buf[5] = 0.6f;
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, 1, 2, -1, -1);
		Check(result, hd.IsValid(), "RGB: IsValid");
		Check(result, hd.channels.GetCount() == 3, "RGB: 3 channels");
		Check(result, hd.has_red && hd.has_green && hd.has_blue, "RGB: flags set");
		Check(result, !hd.has_alpha, "RGB: no alpha");
		Check(result, hd.total_samples == 6, "RGB: total_samples = 6");
	}

	// ---- RGBA with alpha
	{
		int W = 1, H = 1, C = 4;
		Vector<float> buf(W * H * C);
		buf[0] = 1.0f; buf[1] = 0.0f; buf[2] = 0.0f; buf[3] = 0.5f;
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, 1, 2, 3, -1);
		Check(result, hd.IsValid(), "RGBA: IsValid");
		Check(result, hd.channels.GetCount() == 4, "RGBA: 4 channels");
		Check(result, hd.has_alpha, "RGBA: has_alpha true");
	}

	// ---- Empty / null input
	{
		HistogramData hd;
		ComputeHistogram(hd, nullptr, 0, 0, 0, 0, -1, -1, -1, -1);
		Check(result, !hd.IsValid(), "null input: not valid");
	}

	// ---- Single-channel with single_ch index
	{
		int W = 2, H = 2, C = 3;
		Vector<float> buf(W * H * C);
		for(int i = 0; i < W * H; ++i) {
			buf[i * 3 + 0] = 0.2f;
			buf[i * 3 + 1] = 0.5f;
			buf[i * 3 + 2] = 0.8f;
		}
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, 1, 2, -1, 1);
		Check(result, hd.IsValid(), "single_ch: IsValid");
		Check(result, hd.channels.GetCount() == 1, "single_ch: 1 hist channel");
		Check(result, hd.channel_names[0] == "Gray", "single_ch: named Gray");
		// Should have pulled channel 1 (0.5) as Gray
		Check(result, hd.channels[0][128] == 4, "single_ch: 4 pixels in bin128");
	}

	// ---- Below-range min/max tracking
	{
		int W = 1, H = 3, C = 1;
		Vector<float> buf(W * H * C);
		buf[0] = -10.0f;
		buf[1] = 0.5f;
		buf[2] = 20.0f;
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, -1, -1, -1, -1);
		Check(result, std::fabs(hd.min_finite - (-10.0)) < 0.001, "min tracking: -10.0");
		Check(result, std::fabs(hd.max_finite - 20.0) < 0.001, "max tracking: 20.0");
		Check(result, std::fabs(hd.mean - 3.5) < 0.001, "mean: ~3.5 (sum of -10+0.5+20 / 3)");
	}

	std::printf("\n---\nSUMMARY passed=%d failed=%d\n", result.passed, result.failed);
	return result.failed > 0 ? 1 : 0;
}
