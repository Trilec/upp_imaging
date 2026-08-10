#ifndef _ImagingAnalysis_ImagingAnalysis_h_
#define _ImagingAnalysis_ImagingAnalysis_h_

#include <ImagingCore/ImagingCore.h>

namespace Upp {
namespace Imaging {

struct ChannelStatistics : Moveable<ChannelStatistics> {
	int channel = -1;
	String name;
	int64 finite_samples = 0;
	int64 in_range_samples = 0;
	int64 below_range = 0;
	int64 above_range = 0;
	int64 non_finite = 0;
	double min_finite = 0.0;
	double max_finite = 0.0;
	double mean = 0.0;

	int64 TotalSamples() const { return finite_samples + non_finite; }
	bool HasFinite() const { return finite_samples > 0; }
};

struct HistogramChannel : Moveable<HistogramChannel> {
	ChannelStatistics statistics;
	Vector<int64> bins;
};

struct Histogram {
	static constexpr int DEFAULT_BINS = 256;
	static constexpr double RANGE_MIN = 0.0;
	static constexpr double RANGE_MAX = 1.0;

	int bin_count = DEFAULT_BINS;
	int64 analyzed_pixels = 0;
	Vector<HistogramChannel> channels;

	bool IsValid() const { return analyzed_pixels > 0 && !channels.IsEmpty(); }
	void Clear() { bin_count = DEFAULT_BINS; analyzed_pixels = 0; channels.Clear(); }
};

struct PixelProbeValue : Moveable<PixelProbeValue> {
	int channel = -1;
	String name;
	double value = 0.0;
	bool finite = false;
	bool in_range = false;
	bool below_range = false;
	bool above_range = false;
};

struct PixelProbe {
	int x = 0;
	int y = 0;
	int z = 0;
	Vector<PixelProbeValue> values;

	bool IsValid() const { return !values.IsEmpty(); }
	void Clear() { x = y = z = 0; values.Clear(); }
};

Result ComputeChannelStatistics(const ImageData& image,
                                Vector<ChannelStatistics>& output,
                                Diagnostics* diagnostics = nullptr);

Result ComputeHistogram(const ImageData& image, Histogram& output,
                        int bin_count = Histogram::DEFAULT_BINS,
                        Diagnostics* diagnostics = nullptr);

Result ProbePixel(const ImageData& image, int x, int y, int z,
                  PixelProbe& output, Diagnostics* diagnostics = nullptr);

} // namespace Imaging
} // namespace Upp

#endif
