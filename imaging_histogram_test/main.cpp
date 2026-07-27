#include <cmath>
#include <cstdio>
#include <cfloat>
#include <chrono>

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

static bool CheckNear(Result& result, double got, double want, double eps, const char* label)
{
	return Check(result, std::fabs(got - want) <= eps, label);
}

static int64 SumBins(const HistogramChannelData& ch)
{
	int64 sum = 0;
	for(int i = 0; i < ch.bins.GetCount(); ++i) sum += ch.bins[i];
	return sum;
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
		Check(result, hd.analyzed_pixels == 16, "single uniform: pixels");
		Check(result, hd.channels[0].name == "Gray", "single uniform: named Gray");
		Check(result, hd.channels[0].valid_samples == 16, "single uniform: all in range");
		Check(result, hd.channels[0].below_range == 0, "single uniform: no below");
		Check(result, hd.channels[0].above_range == 0, "single uniform: no above");
		Check(result, hd.channels[0].non_finite == 0, "single uniform: no nan");
		Check(result, hd.channels[0].bins[128] == 16, "single uniform: bin128 = 16");
		Check(result, SumBins(hd.channels[0]) == 16, "single uniform: bin conservation");
		CheckNear(result, hd.channels[0].min_finite, 0.5, 0.001, "single uniform: min");
		CheckNear(result, hd.channels[0].max_finite, 0.5, 0.001, "single uniform: max");
		CheckNear(result, hd.channels[0].mean, 0.5, 0.001, "single uniform: mean");
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
		Check(result, hd.channels[0].valid_samples == 256, "ramp 256: all in range (0..255/255)");
		int non_empty = 0;
		for(int i = 0; i < hd.bins; ++i)
			if(hd.channels[0].bins[i] > 0) ++non_empty;
		Check(result, non_empty == 256, "ramp 256: every bin filled");
		CheckNear(result, hd.channels[0].mean, 0.5, 0.005, "ramp 256: mean ~0.5");
		Check(result, SumBins(hd.channels[0]) + hd.channels[0].below_range + hd.channels[0].above_range + hd.channels[0].non_finite
		           == hd.channels[0].TotalClassified(), "ramp 256: conservation");
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
		Check(result, hd.channels[0].valid_samples == 1, "edge vals: one in-range");
		Check(result, hd.channels[0].below_range == 1, "edge vals: one below");
		Check(result, hd.channels[0].above_range == 1, "edge vals: one above");
		Check(result, hd.channels[0].non_finite == 2, "edge vals: two non-finite");
		Check(result, hd.channels[0].TotalClassified() == 5, "edge vals: total classified");
		CheckNear(result, hd.channels[0].min_finite, -0.5, 0.001, "edge vals: min");
		CheckNear(result, hd.channels[0].max_finite, 2.0, 0.001, "edge vals: max");
	}

	// ---- RGB with different per-channel statistics
	{
		int W = 2, H = 2, C = 3;
		Vector<float> buf(W * H * C);
		for(int i = 0; i < W * H; ++i) {
			buf[i * 3 + 0] = 0.1f;  // R: all 0.1
			buf[i * 3 + 1] = 0.5f;  // G: all 0.5
			buf[i * 3 + 2] = 0.9f;  // B: all 0.9
		}
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, 1, 2, -1, -1);
		Check(result, hd.IsValid(), "RGB diff: IsValid");
		Check(result, hd.channels.GetCount() == 3, "RGB diff: 3 channels");
		Check(result, hd.channels[0].name == "R", "RGB diff: R name");
		Check(result, hd.channels[1].name == "G", "RGB diff: G name");
		Check(result, hd.channels[2].name == "B", "RGB diff: B name");
		CheckNear(result, hd.channels[0].mean, 0.1, 0.001, "RGB diff: R mean");
		CheckNear(result, hd.channels[1].mean, 0.5, 0.001, "RGB diff: G mean");
		CheckNear(result, hd.channels[2].mean, 0.9, 0.001, "RGB diff: B mean");
		CheckNear(result, hd.channels[0].min_finite, 0.1, 0.001, "RGB diff: R min");
		CheckNear(result, hd.channels[2].max_finite, 0.9, 0.001, "RGB diff: B max");
	}

	// ---- RGBA with independent alpha statistics
	{
		int W = 2, H = 2, C = 4;
		Vector<float> buf(W * H * C);
		for(int i = 0; i < W * H; ++i) {
			buf[i * 4 + 0] = 0.2f;  // R
			buf[i * 4 + 1] = 0.4f;  // G
			buf[i * 4 + 2] = 0.6f;  // B
			buf[i * 4 + 3] = 0.8f;  // A
		}
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, 1, 2, 3, -1);
		Check(result, hd.IsValid(), "RGBA: IsValid");
		Check(result, hd.channels.GetCount() == 4, "RGBA: 4 channels");
		Check(result, hd.channels[3].name == "A", "RGBA: A name");
		CheckNear(result, hd.channels[3].mean, 0.8, 0.001, "RGBA: A mean independent");
		CheckNear(result, hd.channels[0].mean, 0.2, 0.001, "RGBA: R mean");
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
		Check(result, hd.channels[0].name == "Gray", "single_ch: named Gray");
		Check(result, hd.channels[0].bins[128] == 4, "single_ch: 4 pixels in bin128");
		CheckNear(result, hd.channels[0].mean, 0.5, 0.001, "single_ch: mean");
	}

	// ---- One channel entirely below range, another above, another NaN
	{
		int W = 1, H = 3, C = 3;
		Vector<float> buf(W * H * C);
		buf[0] = -1.0f; buf[1] = 2.0f; buf[2] = std::nanf("");
		buf[3] = -1.0f; buf[4] = 2.0f; buf[5] = std::nanf("");
		buf[6] = -1.0f; buf[7] = 2.0f; buf[8] = std::nanf("");
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, 1, 2, -1, -1);
		Check(result, hd.channels[0].below_range == 3, "channel below: R below");
		Check(result, hd.channels[0].valid_samples == 0, "channel below: R no in-range");
		Check(result, hd.channels[1].above_range == 3, "channel above: G above");
		Check(result, hd.channels[2].non_finite == 3, "channel nan: B non-finite");
		Check(result, hd.channels[2].valid_samples == 0, "channel nan: B no in-range");
		Check(result, hd.channels[0].HasFinite(), "channel below: R HasFinite (below is finite)");
		CheckNear(result, hd.channels[0].min_finite, -1.0, 0.001, "channel below: R min");
		CheckNear(result, hd.channels[0].max_finite, -1.0, 0.001, "channel below: R max");
		CheckNear(result, hd.channels[0].mean, -1.0, 0.001, "channel below: R mean");
		Check(result, hd.channels[1].HasFinite(), "channel above: G HasFinite (above is finite)");
		CheckNear(result, hd.channels[1].min_finite, 2.0, 0.001, "channel above: G min");
		CheckNear(result, hd.channels[1].max_finite, 2.0, 0.001, "channel above: G max");
		CheckNear(result, hd.channels[1].mean, 2.0, 0.001, "channel above: G mean");
		Check(result, !hd.channels[2].HasFinite(), "channel nan: B not HasFinite");
		CheckNear(result, hd.channels[2].min_finite, 0.0, 0.001, "channel nan: B min cleared");
		CheckNear(result, hd.channels[2].max_finite, 0.0, 0.001, "channel nan: B max cleared");
		CheckNear(result, hd.channels[2].mean, 0.0, 0.001, "channel nan: B mean cleared");
	}

	// ---- Mixed finite and non-finite per channel with mean
	{
		int W = 1, H = 4, C = 1;
		Vector<float> buf(W * H * C);
		buf[0] = std::nanf("");
		buf[1] = -1.0f;
		buf[2] = 2.0f;
		buf[3] = 0.5f;
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, -1, -1, -1, 0);
		Check(result, hd.channels[0].valid_samples == 1, "mixed: one in-range");
		Check(result, hd.channels[0].below_range == 1, "mixed: one below");
		Check(result, hd.channels[0].above_range == 1, "mixed: one above");
		Check(result, hd.channels[0].non_finite == 1, "mixed: one non-finite");
		Check(result, hd.channels[0].HasFinite(), "mixed: HasFinite true");
		CheckNear(result, hd.channels[0].mean, ((-1.0 + 2.0 + 0.5) / 3.0), 0.001, "mixed: mean of finites");
	}

	// ---- No finite samples in one channel
	{
		int W = 1, H = 2, C = 2;
		Vector<float> buf(W * H * C);
		buf[0] = std::nanf(""); buf[1] = 0.5f;
		buf[2] = INFINITY;      buf[3] = 0.5f;
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, 1, -1, -1, -1);
		Check(result, !hd.channels[0].HasFinite(), "no finite: R not HasFinite");
		CheckNear(result, hd.channels[0].min_finite, 0.0, 0.001, "no finite: R min cleared");
		CheckNear(result, hd.channels[0].max_finite, 0.0, 0.001, "no finite: R max cleared");
		CheckNear(result, hd.channels[0].mean, 0.0, 0.001, "no finite: R mean cleared");
		Check(result, hd.channels[1].valid_samples == 2, "no finite: G still valid");
	}

	// ---- Exact boundary: 0.0 in first bin, 1.0 in last bin
	{
		int W = 1, H = 2, C = 1;
		Vector<float> buf(W * H * C);
		buf[0] = 0.0f;
		buf[1] = 1.0f;
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, -1, -1, -1, 0);
		Check(result, hd.channels[0].bins[0] == 1, "boundary: 0.0 in bin 0");
		Check(result, hd.channels[0].bins[hd.bins - 1] == 1, "boundary: 1.0 in last bin");
		Check(result, hd.channels[0].below_range == 0, "boundary: no below");
		Check(result, hd.channels[0].above_range == 0, "boundary: no above");
	}

	// ---- Reset and reuse of HistogramData
	{
		Vector<float> buf1(4, 0.5f);
		Vector<float> buf2(4, 0.8f);
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf1, 2, 2, 1, 0, -1, -1, -1, 0);
		Check(result, hd.channels[0].bins[(int)(0.5 * hd.bins)] == 4, "reuse: first counts");
		ComputeHistogramFromBuffer(hd, buf2, 2, 2, 1, 0, -1, -1, -1, 0);
		Check(result, hd.channels[0].bins[(int)(0.5 * hd.bins)] == 0, "reuse: old counts cleared");
		Check(result, hd.channels[0].bins[(int)(0.8 * hd.bins)] == 4, "reuse: new counts present");
	}

	// ---- Configurable bin count
	{
		Vector<float> buf(64, 0.5f);
		HistogramData hd;
		ComputeHistogramFromBuffer(hd, buf, 8, 8, 1, 0, -1, -1, -1, 0, 64);
		Check(result, hd.bins == 64, "bins: custom count");
		Check(result, hd.channels[0].bins.GetCount() == 64, "bins: channel bins match");
	}

	// ---- HistogramProxyKey equality and string
	{
		HistogramProxyKey a{0, Size(100, 50), Size(50, 25), 4, 0, 1, 2, 3, -1};
		HistogramProxyKey b{0, Size(100, 50), Size(50, 25), 4, 0, 1, 2, 3, -1};
		HistogramProxyKey c{1, Size(100, 50), Size(50, 25), 4, 0, 1, 2, 3, -1};
		Check(result, a == b, "proxy key: equal");
		Check(result, a != c, "proxy key: not equal");
		Check(result, !a.ToString().IsEmpty(), "proxy key: string not empty");
	}

	// ---- Invalidation simulation: recompute only on key change
	{
		int recomputes = 0;
		HistogramData hd;
		HistogramProxyKey last_key;
		auto maybe_compute = [&](const HistogramProxyKey& key, const Vector<float>& pixels, int w, int h, int c) {
			if(key != last_key) {
				++recomputes;
				ComputeHistogramFromBuffer(hd, pixels, w, h, c, 0, 1, 2, -1, -1);
				last_key = key;
			}
		};

		Vector<float> px(16, 0.5f);
		HistogramProxyKey k1{0, Size(4, 4), Size(4, 4), 3, 0, 1, 2, -1, -1};
		maybe_compute(k1, px, 4, 4, 3);
		maybe_compute(k1, px, 4, 4, 3); // same key
		maybe_compute(k1, px, 4, 4, 3); // same key
		Check(result, recomputes == 1, "invalidation: same key no recompute");

		HistogramProxyKey k2{1, Size(4, 4), Size(4, 4), 3, 0, 1, 2, -1, -1};
		maybe_compute(k2, px, 4, 4, 3);
		Check(result, recomputes == 2, "invalidation: group change recomputes");

		HistogramProxyKey k3{1, Size(8, 8), Size(4, 4), 3, 0, 1, 2, -1, -1};
		maybe_compute(k3, px, 4, 4, 3);
		Check(result, recomputes == 3, "invalidation: source size change recomputes");
	}

	// ---- Performance measurements
	{
		auto measure = [&](int W, int H, int C, const char* label) {
			Vector<float> buf((size_t)W * H * C);
			for(int i = 0; i < W * H; ++i) {
				for(int c = 0; c < C; ++c)
					buf[i * C + c] = (float)((i + c * 85) % 256) / 255.0f;
			}
			HistogramData hd;
			auto t0 = std::chrono::steady_clock::now();
			if(C == 1)
				ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, -1, -1, -1, 0);
			else if(C == 3)
				ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, 1, 2, -1, -1);
			else if(C == 4)
				ComputeHistogramFromBuffer(hd, buf, W, H, C, 0, 1, 2, 3, -1);
			auto ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
			std::printf("perf %s: %.2f ms\n", label, ms);
			return hd;
		};

		auto hd800rgb = measure(800, 600, 3, "800x600 RGB");
		Check(result, hd800rgb.IsValid(), "perf 800 RGB: valid");

		auto hd1080rgb = measure(1920, 1080, 3, "1920x1080 RGB");
		Check(result, hd1080rgb.IsValid(), "perf 1080 RGB: valid");

		// Approximate the current maximum bounded proxy size (long edge 2048, area 2M).
		auto hdBounded = measure(2000, 1000, 4, "2000x1000 RGBA bounded");
		Check(result, hdBounded.IsValid(), "perf bounded RGBA: valid");
	}

	std::printf("\n---\nSUMMARY passed=%d failed=%d\n", result.passed, result.failed);
	return result.failed > 0 ? 1 : 0;
}
