#ifndef _imaging_histogram_imaging_histogram_h_
#define _imaging_histogram_imaging_histogram_h_

#include <Core/Core.h>

namespace Upp {

struct HistogramData {
	static constexpr int DEFAULT_BINS = 256;
	static constexpr double RANGE_MIN = 0.0;
	static constexpr double RANGE_MAX = 1.0;

	int bins = DEFAULT_BINS;
	Vector<Vector<int64>> channels;
	Vector<String> channel_names;
	int64 total_samples = 0;
	int64 below_range = 0;
	int64 above_range = 0;
	int64 non_finite = 0;
	double min_finite = DBL_MAX;
	double max_finite = -DBL_MAX;
	double mean = 0.0;
	int analyzed_width = 0;
	int analyzed_height = 0;
	int analyzed_channels = 0;
	bool has_red = false;
	bool has_green = false;
	bool has_blue = false;
	bool has_alpha = false;

	bool IsValid() const { return total_samples > 0 && !channels.IsEmpty(); }

	void Clear();
	int64 TotalClassified() const;
};

void ComputeHistogram(HistogramData& data, const float* pixels, int width, int height, int channels,
                      int red_ch, int green_ch, int blue_ch, int alpha_ch, int single_ch,
                      int bin_count = HistogramData::DEFAULT_BINS);

void ComputeHistogramFromBuffer(HistogramData& data, const Vector<float>& pixels,
                                int width, int height, int channels,
                                int red_ch, int green_ch, int blue_ch, int alpha_ch, int single_ch,
                                int bin_count = HistogramData::DEFAULT_BINS);

struct HistogramStats {
	int64 total_pixels = 0;
	int64 below_range = 0;
	int64 above_range = 0;
	int64 non_finite = 0;
	double min_finite = 0.0;
	double max_finite = 0.0;
	double mean = 0.0;
};

HistogramStats GetHistogramStats(const HistogramData& data, int channel_index);

} // namespace Upp

#endif
