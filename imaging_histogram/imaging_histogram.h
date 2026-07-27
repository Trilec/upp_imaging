#ifndef _imaging_histogram_imaging_histogram_h_
#define _imaging_histogram_imaging_histogram_h_

#include <Core/Core.h>

namespace Upp {

struct HistogramChannelData : Moveable<HistogramChannelData> {
	String name;
	Vector<int64> bins;
	int64 valid_samples = 0;
	int64 below_range = 0;
	int64 above_range = 0;
	int64 non_finite = 0;
	double min_finite = 0.0;
	double max_finite = 0.0;
	double mean = 0.0;
	bool is_available = false;

	int64 TotalClassified() const { return valid_samples + below_range + above_range + non_finite; }
	bool HasFinite() const { return valid_samples + below_range + above_range > 0; }
};

struct HistogramData : Moveable<HistogramData> {
	static constexpr int DEFAULT_BINS = 256;
	static constexpr double RANGE_MIN = 0.0;
	static constexpr double RANGE_MAX = 1.0;

	int bins = DEFAULT_BINS;
	Vector<HistogramChannelData> channels;
	int64 analyzed_pixels = 0;
	int analyzed_width = 0;
	int analyzed_height = 0;
	int analyzed_channels = 0;

	bool IsValid() const { return analyzed_pixels > 0 && !channels.IsEmpty(); }
	void Clear();
};

void ComputeHistogram(HistogramData& data, const float* pixels, int width, int height, int channels,
                      int red_ch, int green_ch, int blue_ch, int alpha_ch, int single_ch,
                      int bin_count = HistogramData::DEFAULT_BINS);

void ComputeHistogramFromBuffer(HistogramData& data, const Vector<float>& pixels,
                                int width, int height, int channels,
                                int red_ch, int green_ch, int blue_ch, int alpha_ch, int single_ch,
                                int bin_count = HistogramData::DEFAULT_BINS);

// Describes the proxy configuration that produced the histogram data.
// Used by callers to decide whether a stored histogram is still valid.
struct HistogramProxyKey {
	int group_index = -1;
	Size source_size;
	Size proxy_size;
	int channel_count = 0;
	int red = -1;
	int green = -1;
	int blue = -1;
	int alpha = -1;
	int single_channel = -1;

	bool operator==(const HistogramProxyKey& other) const;
	bool operator!=(const HistogramProxyKey& other) const;
	String ToString() const;
};

} // namespace Upp

#endif
