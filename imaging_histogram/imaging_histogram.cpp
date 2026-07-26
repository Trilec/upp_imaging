#include "imaging_histogram.h"

#include <algorithm>
#include <cmath>
#include <cfloat>

namespace Upp {

void HistogramData::Clear()
{
	bins = DEFAULT_BINS;
	channels.Clear();
	channel_names.Clear();
	total_samples = 0;
	below_range = 0;
	above_range = 0;
	non_finite = 0;
	min_finite = DBL_MAX;
	max_finite = -DBL_MAX;
	mean = 0.0;
	analyzed_width = 0;
	analyzed_height = 0;
	analyzed_channels = 0;
	has_red = false;
	has_green = false;
	has_blue = false;
	has_alpha = false;
}

int64 HistogramData::TotalClassified() const
{
	return total_samples + below_range + above_range + non_finite;
}

static void AccumulateChannel(HistogramData& data, int ch_idx, const String& name,
                               const float* pixels, int count, int channels)
{
	Vector<int64>& bins = data.channels[ch_idx];
	bins.SetCount(data.bins, 0);
	int64 total = 0;
	int64 below = 0;
	int64 above = 0;
	int64 non_finite = 0;
	double sum = 0.0;
	int64 finite_count = 0;
	double local_min = DBL_MAX;
	double local_max = -DBL_MAX;
	int max_bin = data.bins - 1;
	double range_min = HistogramData::RANGE_MIN;
	double range_max = HistogramData::RANGE_MAX;

	for(int i = 0; i < count; ++i) {
		float v = pixels[i * channels + ch_idx];
		if(!std::isfinite(v)) {
			++non_finite;
			continue;
		}
		if(v < range_min) {
			++below;
			if((double)v < local_min) local_min = v;
			if((double)v > local_max) local_max = v;
			sum += v;
			++finite_count;
			continue;
		}
		if(v >= range_max) {
			++above;
			if((double)v < local_min) local_min = v;
			if((double)v > local_max) local_max = v;
			sum += v;
			++finite_count;
			continue;
		}
		++total;
		if((double)v < local_min) local_min = v;
		if((double)v > local_max) local_max = v;
		sum += v;
		++finite_count;
		int bin = (int)((v - range_min) / (range_max - range_min) * data.bins);
		if(bin >= data.bins) bin = max_bin;
		if(bin < 0) bin = 0;
		++bins[bin];
	}

	data.channel_names[ch_idx] = name;
	data.below_range += below;
	data.above_range += above;
	data.non_finite += non_finite;
	if(finite_count > 0) {
		if(local_min < data.min_finite) data.min_finite = local_min;
		if(local_max > data.max_finite) data.max_finite = local_max;
		data.mean += sum;
	}
}

void ComputeHistogram(HistogramData& data, const float* pixels, int width, int height, int channels,
                      int red_ch, int green_ch, int blue_ch, int alpha_ch, int single_ch,
                      int bin_count)
{
	data.Clear();
	if(!pixels || width <= 0 || height <= 0 || channels <= 0) {
		return;
	}

	data.bins = std::max(16, bin_count);
	data.analyzed_width = width;
	data.analyzed_height = height;
	data.analyzed_channels = channels;

	int num_hist_channels = 0;
	int order[4] = { -1, -1, -1, -1 };
	String names[4];

	auto add = [&](int src_ch, const String& name) {
		if(src_ch >= 0 && src_ch < channels) {
			order[num_hist_channels] = src_ch;
			names[num_hist_channels] = name;
			++num_hist_channels;
		}
	};

	if(single_ch >= 0 && single_ch < channels) {
		add(single_ch, "Gray");
		data.has_red = false;
		data.has_green = false;
		data.has_blue = false;
		data.has_alpha = alpha_ch >= 0 && alpha_ch < channels;
	}
	else {
		add(red_ch, "R");
		add(green_ch, "G");
		add(blue_ch, "B");
		if(alpha_ch >= 0 && alpha_ch < channels) {
			add(alpha_ch, "A");
			data.has_alpha = true;
		}
		data.has_red = red_ch >= 0 && red_ch < channels;
		data.has_green = green_ch >= 0 && green_ch < channels;
		data.has_blue = blue_ch >= 0 && blue_ch < channels;
	}

	if(num_hist_channels == 0)
		return;

	data.channels.SetCount(num_hist_channels);
	data.channel_names.SetCount(num_hist_channels);

	int pixel_count = width * height;
	for(int c = 0; c < num_hist_channels; ++c) {
		data.channels[c].SetCount(data.bins, 0);
	}
	data.total_samples = 0;
	data.below_range = 0;
	data.above_range = 0;
	data.non_finite = 0;
	data.mean = 0.0;
	data.min_finite = DBL_MAX;
	data.max_finite = -DBL_MAX;

	int64 grand_total = 0;
	int64 grand_below = 0;
	int64 grand_above = 0;
	int64 grand_non_finite = 0;
	double grand_sum = 0.0;
	int64 grand_finite = 0;
	double grand_min = DBL_MAX;
	double grand_max = -DBL_MAX;
	int max_bin = data.bins - 1;
	double range_min = HistogramData::RANGE_MIN;
	double range_max = HistogramData::RANGE_MAX;

	for(int i = 0; i < pixel_count; ++i) {
		for(int c = 0; c < num_hist_channels; ++c) {
			int src_ch = order[c];
			float v = pixels[i * channels + src_ch];
			if(!std::isfinite(v)) {
				++grand_non_finite;
				continue;
			}
			if(v < range_min) {
				++grand_below;
				if((double)v < grand_min) grand_min = v;
				if((double)v > grand_max) grand_max = v;
				grand_sum += v;
				++grand_finite;
				continue;
			}
			if(v >= range_max) {
				++grand_above;
				if((double)v < grand_min) grand_min = v;
				if((double)v > grand_max) grand_max = v;
				grand_sum += v;
				++grand_finite;
				continue;
			}
			++grand_total;
			if((double)v < grand_min) grand_min = v;
			if((double)v > grand_max) grand_max = v;
			grand_sum += v;
			++grand_finite;
			int bin = (int)((v - range_min) / (range_max - range_min) * data.bins);
			if(bin >= data.bins) bin = max_bin;
			if(bin < 0) bin = 0;
			++data.channels[c][bin];
		}
	}

	data.total_samples = grand_total;
	data.below_range = grand_below;
	data.above_range = grand_above;
	data.non_finite = grand_non_finite;
	if(grand_finite > 0) {
		data.min_finite = grand_min;
		data.max_finite = grand_max;
		data.mean = grand_sum / grand_finite;
	}

	for(int c = 0; c < num_hist_channels; ++c) {
		data.channel_names[c] = names[c];
	}
}

void ComputeHistogramFromBuffer(HistogramData& data, const Vector<float>& pixels,
                                int width, int height, int channels,
                                int red_ch, int green_ch, int blue_ch, int alpha_ch, int single_ch,
                                int bin_count)
{
	ComputeHistogram(data, pixels.Begin(), width, height, channels,
	                 red_ch, green_ch, blue_ch, alpha_ch, single_ch, bin_count);
}

HistogramStats GetHistogramStats(const HistogramData& data, int channel_index)
{
	HistogramStats stats;
	if(!data.IsValid() || channel_index < 0 || channel_index >= data.channels.GetCount())
		return stats;
	stats.total_pixels = data.total_samples / data.channels.GetCount();
	stats.below_range = data.below_range / data.channels.GetCount();
	stats.above_range = data.above_range / data.channels.GetCount();
	stats.non_finite = data.non_finite / data.channels.GetCount();
	stats.min_finite = data.min_finite;
	stats.max_finite = data.max_finite;
	stats.mean = data.mean;
	return stats;
}

} // namespace Upp
