#include <ImagingAnalysis/ImagingAnalysis.h>

#include <cmath>
#include <cstring>
#include <limits>

using namespace Upp;
using namespace Upp::Imaging;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const char* name)
{
	Cout() << (condition ? "PASS " : "FAIL ") << name << '\n';
	(condition ? state.passed : state.failed)++;
}

static bool Near(double a, double b, double epsilon = 1e-6)
{
	return std::fabs(a - b) <= epsilon;
}

static uint64 BufferHash(const ImageBuffer& buffer)
{
	uint64 hash = 1469598103934665603ULL;
	for(int i = 0; i < buffer.GetByteCount(); ++i) {
		hash ^= buffer.Begin()[i];
		hash *= 1099511628211ULL;
	}
	return hash;
}

static bool HasCode(const Diagnostics& diagnostics, const char* code)
{
	for(const DiagnosticEntry& entry : diagnostics.Entries())
		if(entry.code == code)
			return true;
	return false;
}

static ImageData MakeImage(SampleType type, ChannelLayout layout,
                           int width, int height, int depth = 1,
                           int left = 0, int top = 0)
{
	ImageData image;
	image.spec.data_window = {left, top, left + width - 1, top + height - 1};
	image.spec.depth = depth;
	image.spec.sample_type = type;
	image.spec.channel_layout = layout;
	image.spec.channels = CanonicalChannels(layout);
	image.spec.alpha_channel = layout == ChannelLayout::RGBA ? 3
	                         : layout == ChannelLayout::GrayAlpha ? 1 : -1;
	if(layout == ChannelLayout::Gray) {
		image.spec.channel_names.Add("Y");
	}
	else if(layout == ChannelLayout::GrayAlpha) {
		image.spec.channel_names.Add("Y");
		image.spec.channel_names.Add("A");
	}
	else if(layout == ChannelLayout::RGB || layout == ChannelLayout::RGBA) {
		image.spec.channel_names.Add("R");
		image.spec.channel_names.Add("G");
		image.spec.channel_names.Add("B");
		if(layout == ChannelLayout::RGBA)
			image.spec.channel_names.Add("A");
	}
	image.buffer.Allocate(image.spec);
	return image;
}

static void SetUInt8(ImageData& image, int64 sample, byte value)
{
	image.buffer.Begin()[sample] = value;
}

static void SetUInt16(ImageData& image, int64 sample, uint16 value)
{
	memcpy(image.buffer.Begin() + sample * 2, &value, sizeof(value));
}

static void SetHalfBits(ImageData& image, int64 sample, uint16 value)
{
	memcpy(image.buffer.Begin() + sample * 2, &value, sizeof(value));
}

static void SetFloat(ImageData& image, int64 sample, float value)
{
	memcpy(image.buffer.Begin() + sample * 4, &value, sizeof(value));
}

static int64 SumBins(const Vector<int64>& bins)
{
	int64 total = 0;
	for(int64 value : bins)
		total += value;
	return total;
}

CONSOLE_APP_MAIN
{
	State state;
	Diagnostics diagnostics;

	ImageData rgba = MakeImage(SampleType::Float32, ChannelLayout::RGBA, 2, 1, 1, -2, 3);
	const float rgba_values[8] = {
		-0.25f, 0.0f, 0.5f, 1.0f,
		1.25f, std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::infinity(), 0.25f
	};
	for(int i = 0; i < 8; ++i)
		SetFloat(rgba, i, rgba_values[i]);
	rgba.metadata.Set("fixture", Value("analysis"));
	uint64 rgba_hash = BufferHash(rgba.buffer);

	Vector<ChannelStatistics> statistics;
	Result stats_result = ComputeChannelStatistics(rgba, statistics, &diagnostics);
	Check(state, stats_result.IsOk() && diagnostics.IsEmpty(), "Float32 statistics succeeds");
	Check(state, statistics.GetCount() == 4 && statistics[0].name == "R" &&
	      statistics[3].name == "A", "canonical channel names");
	Check(state, statistics[0].finite_samples == 2 && statistics[0].below_range == 1 &&
	      statistics[0].above_range == 1 && statistics[0].in_range_samples == 0,
	      "Float32 R range classification");
	Check(state, Near(statistics[0].min_finite, -0.25) &&
	      Near(statistics[0].max_finite, 1.25) && Near(statistics[0].mean, 0.5),
	      "Float32 R statistics");
	Check(state, statistics[1].finite_samples == 1 && statistics[1].non_finite == 1 &&
	      statistics[1].in_range_samples == 1 && Near(statistics[1].mean, 0.0),
	      "Float32 NaN accounting");
	Check(state, statistics[2].finite_samples == 1 && statistics[2].non_finite == 1 &&
	      Near(statistics[2].mean, 0.5), "Float32 infinity accounting");
	Check(state, statistics[3].finite_samples == 2 && statistics[3].in_range_samples == 2 &&
	      Near(statistics[3].mean, 0.625), "Float32 alpha statistics");
	Check(state, BufferHash(rgba.buffer) == rgba_hash && rgba.metadata.Get("fixture") == Value("analysis"),
	      "statistics leave source unchanged");

	Histogram histogram;
	Result histogram_result = ComputeHistogram(rgba, histogram, 4, &diagnostics);
	Check(state, histogram_result.IsOk() && histogram.IsValid() && histogram.bin_count == 4,
	      "Float32 histogram succeeds");
	Check(state, histogram.analyzed_pixels == 2 && histogram.channels.GetCount() == 4,
	      "histogram dimensions and channels");
	Check(state, SumBins(histogram.channels[0].bins) == 0,
	      "out-of-range values are not binned");
	Check(state, histogram.channels[1].bins[0] == 1 && SumBins(histogram.channels[1].bins) == 1,
	      "zero maps to first bin");
	Check(state, histogram.channels[2].bins[2] == 1 && SumBins(histogram.channels[2].bins) == 1,
	      "midpoint maps to expected bin");
	Check(state, histogram.channels[3].bins[1] == 1 && histogram.channels[3].bins[3] == 1,
	      "quarter and one map to bounded bins");
	Check(state, histogram.channels[0].statistics.below_range == 1 &&
	      histogram.channels[0].statistics.above_range == 1,
	      "histogram retains out-of-range statistics");

	PixelProbe probe;
	Result probe_result = ProbePixel(rgba, -1, 3, 0, probe, &diagnostics);
	Check(state, probe_result.IsOk() && probe.x == -1 && probe.y == 3 && probe.z == 0,
	      "data-window probe succeeds");
	Check(state, probe.values.GetCount() == 4 && Near(probe.values[0].value, 1.25) &&
	      probe.values[0].above_range, "probe above-range value");
	Check(state, !probe.values[1].finite && !probe.values[1].in_range,
	      "probe NaN classification");
	Check(state, !probe.values[2].finite && !probe.values[2].in_range,
	      "probe infinity classification");
	Check(state, Near(probe.values[3].value, 0.25) && probe.values[3].in_range,
	      "probe finite alpha value");

	PixelProbe preserved_probe;
	preserved_probe.x = 77;
	PixelProbeValue sentinel_probe;
	sentinel_probe.channel = 9;
	sentinel_probe.name = "sentinel";
	preserved_probe.values.Add(pick(sentinel_probe));
	Result bad_probe = ProbePixel(rgba, 99, 3, 0, preserved_probe, &diagnostics);
	Check(state, bad_probe.code == ResultCode::InvalidArgument && HasCode(diagnostics, "IMGANALYSIS_COORD"),
	      "invalid probe reports stable code");
	Check(state, preserved_probe.x == 77 && preserved_probe.values.GetCount() == 1 &&
	      preserved_probe.values[0].name == "sentinel", "invalid probe preserves output");

	ImageData u8 = MakeImage(SampleType::UInt8, ChannelLayout::RGB, 1, 1);
	SetUInt8(u8, 0, 0);
	SetUInt8(u8, 1, 128);
	SetUInt8(u8, 2, 255);
	Vector<ChannelStatistics> u8_stats;
	Check(state, ComputeChannelStatistics(u8, u8_stats, &diagnostics).IsOk(),
	      "UInt8 statistics succeeds");
	Check(state, Near(u8_stats[0].mean, 0.0) && Near(u8_stats[1].mean, 128.0 / 255.0) &&
	      Near(u8_stats[2].mean, 1.0), "UInt8 values normalize to unit range");
	Histogram u8_hist;
	Check(state, ComputeHistogram(u8, u8_hist, 4, &diagnostics).IsOk() &&
	      u8_hist.channels[0].bins[0] == 1 && u8_hist.channels[1].bins[2] == 1 &&
	      u8_hist.channels[2].bins[3] == 1, "UInt8 histogram normalization");

	ImageData u16 = MakeImage(SampleType::UInt16, ChannelLayout::Gray, 1, 1);
	SetUInt16(u16, 0, 32768);
	PixelProbe u16_probe;
	Check(state, ProbePixel(u16, 0, 0, 0, u16_probe, &diagnostics).IsOk(),
	      "UInt16 probe succeeds");
	Check(state, Near(u16_probe.values[0].value, 32768.0 / 65535.0),
	      "UInt16 probe normalization");

	ImageData half = MakeImage(SampleType::Float16, ChannelLayout::Gray, 4, 1);
	SetHalfBits(half, 0, 0x3c00);
	SetHalfBits(half, 1, 0x3800);
	SetHalfBits(half, 2, 0x7c00);
	SetHalfBits(half, 3, 0x7e00);
	Vector<ChannelStatistics> half_stats;
	Check(state, ComputeChannelStatistics(half, half_stats, &diagnostics).IsOk(),
	      "Float16 statistics succeeds");
	Check(state, half_stats[0].finite_samples == 2 && half_stats[0].non_finite == 2,
	      "Float16 finite classification");
	Check(state, Near(half_stats[0].min_finite, 0.5) && Near(half_stats[0].max_finite, 1.0) &&
	      Near(half_stats[0].mean, 0.75), "Float16 numeric decoding");

	ImageData multi;
	multi.spec.data_window = {5, -2, 5, -2};
	multi.spec.depth = 1;
	multi.spec.channels = 3;
	multi.spec.channel_layout = ChannelLayout::MultiChannel;
	multi.spec.sample_type = SampleType::Float32;
	multi.spec.alpha_channel = -1;
	multi.spec.channel_names.Add("Z");
	multi.spec.channel_names.Add("mask");
	multi.spec.channel_names.Add("temperature");
	multi.buffer.Allocate(multi.spec);
	SetFloat(multi, 0, -1.0f);
	SetFloat(multi, 1, 0.25f);
	SetFloat(multi, 2, 2.0f);
	Vector<ChannelStatistics> multi_stats;
	Check(state, ComputeChannelStatistics(multi, multi_stats, &diagnostics).IsOk() &&
	      multi_stats.GetCount() == 3, "MultiChannel statistics succeeds");
	Check(state, multi_stats[0].name == "Z" && multi_stats[1].name == "mask" &&
	      multi_stats[2].name == "temperature", "MultiChannel names preserved");
	Check(state, multi_stats[0].below_range == 1 && multi_stats[1].in_range_samples == 1 &&
	      multi_stats[2].above_range == 1, "MultiChannel range evidence");

	ImageData depth = MakeImage(SampleType::Float32, ChannelLayout::Gray, 1, 1, 2, 10, 20);
	SetFloat(depth, 0, 0.25f);
	SetFloat(depth, 1, 0.75f);
	Vector<ChannelStatistics> depth_stats;
	Check(state, ComputeChannelStatistics(depth, depth_stats, &diagnostics).IsOk() &&
	      depth_stats[0].finite_samples == 2 && Near(depth_stats[0].mean, 0.5),
	      "depth slices contribute to statistics");
	PixelProbe depth_probe;
	Check(state, ProbePixel(depth, 10, 20, 1, depth_probe, &diagnostics).IsOk() &&
	      Near(depth_probe.values[0].value, 0.75), "probe selects depth slice");

	Histogram preserved_histogram;
	preserved_histogram.bin_count = 9;
	preserved_histogram.analyzed_pixels = 123;
	Result bad_bins = ComputeHistogram(rgba, preserved_histogram, 1, &diagnostics);
	Check(state, bad_bins.code == ResultCode::InvalidArgument && HasCode(diagnostics, "IMGANALYSIS_BINS"),
	      "invalid bin count reports stable code");
	Check(state, preserved_histogram.bin_count == 9 && preserved_histogram.analyzed_pixels == 123,
	      "invalid bin count preserves histogram output");

	ImageData invalid;
	Vector<ChannelStatistics> preserved_stats;
	ChannelStatistics sentinel_stats;
	sentinel_stats.channel = 42;
	sentinel_stats.name = "sentinel";
	preserved_stats.Add(pick(sentinel_stats));
	Result invalid_stats = ComputeChannelStatistics(invalid, preserved_stats, &diagnostics);
	Check(state, invalid_stats.code == ResultCode::InvalidSpecification && HasCode(diagnostics, "IMGANALYSIS_SPEC"),
	      "invalid image reports stable code");
	Check(state, preserved_stats.GetCount() == 1 && preserved_stats[0].channel == 42,
	      "invalid image preserves statistics output");

	Check(state, ComputeChannelStatistics(u8, preserved_stats, &diagnostics).IsOk() && diagnostics.IsEmpty(),
	      "successful analysis clears prior diagnostics");
	Check(state, BufferHash(rgba.buffer) == rgba_hash,
	      "all analysis operations leave source pixels unchanged");

	Cout() << "SUMMARY passed=" << state.passed
	       << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
