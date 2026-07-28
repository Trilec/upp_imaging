#include "imaging_histogram.h"

#include <algorithm>
#include <cmath>
#include <cfloat>

namespace Upp {

void HistogramData::Clear()
{
	bins = DEFAULT_BINS;
	channels.Clear();
	analyzed_pixels = 0;
	analyzed_width = 0;
	analyzed_height = 0;
	analyzed_channels = 0;
}

bool HistogramProxyKey::operator==(const HistogramProxyKey& other) const
{
	return group_index == other.group_index
		&& source_size == other.source_size
		&& proxy_size == other.proxy_size
		&& channel_count == other.channel_count
		&& red == other.red
		&& green == other.green
		&& blue == other.blue
		&& alpha == other.alpha
		&& single_channel == other.single_channel;
}

bool HistogramProxyKey::operator!=(const HistogramProxyKey& other) const
{
	return !(*this == other);
}

String HistogramProxyKey::ToString() const
{
	return Format("G%d_%dx%d_P%dx%d_C%d_R%d_G%d_B%d_A%d_S%d",
		group_index,
		source_size.cx, source_size.cy,
		proxy_size.cx, proxy_size.cy,
		channel_count,
		red, green, blue, alpha, single_channel);
}

void ComputeHistogram(HistogramData& data, const float* pixels, int width, int height, int channels,
                      int red_ch, int green_ch, int blue_ch, int alpha_ch, int single_ch,
                      int bin_count)
{
	data.Clear();
	if(!pixels || width <= 0 || height <= 0 || channels <= 0)
		return;

	data.bins = std::max(16, bin_count);
	data.analyzed_width = width;
	data.analyzed_height = height;
	data.analyzed_channels = channels;
	data.analyzed_pixels = (int64)width * height;

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
	}
	else {
		add(red_ch, "R");
		add(green_ch, "G");
		add(blue_ch, "B");
		add(alpha_ch, "A");
	}

	if(num_hist_channels == 0)
		return;

	data.channels.SetCount(num_hist_channels);

	// Initialise channel metadata.
	for(int c = 0; c < num_hist_channels; ++c) {
		HistogramChannelData& ch = data.channels[c];
		ch.name = names[c];
		ch.bins.SetCount(data.bins, 0);
		ch.valid_samples = 0;
		ch.below_range = 0;
		ch.above_range = 0;
		ch.non_finite = 0;
		ch.min_finite = DBL_MAX;
		ch.max_finite = -DBL_MAX;
		ch.mean = 0.0;
		ch.is_available = true;
	}

	const int max_bin = data.bins - 1;
	const double range_min = HistogramData::RANGE_MIN;
	const double range_max = HistogramData::RANGE_MAX;
	const int pixel_count = width * height;

	double sums[4] = { 0.0, 0.0, 0.0, 0.0 };

	for(int i = 0; i < pixel_count; ++i) {
		const size_t base = (size_t)i * channels;
		for(int c = 0; c < num_hist_channels; ++c) {
			HistogramChannelData& ch = data.channels[c];
			const float v = pixels[base + order[c]];
			if(!std::isfinite(v)) {
				++ch.non_finite;
				continue;
			}
			const double vd = (double)v;
			if(vd < ch.min_finite) ch.min_finite = vd;
			if(vd > ch.max_finite) ch.max_finite = vd;
			sums[c] += vd;
			if(vd < range_min) {
				++ch.below_range;
			}
			else if(vd > range_max) {
				++ch.above_range;
			}
			else {
				++ch.valid_samples;
				int bin = (int)((vd - range_min) / (range_max - range_min) * data.bins);
				if(bin < 0) bin = 0;
				if(bin > max_bin) bin = max_bin;
				++ch.bins[bin];
			}
		}
	}

	for(int c = 0; c < num_hist_channels; ++c) {
		HistogramChannelData& ch = data.channels[c];
		const int64 finite_samples = ch.valid_samples + ch.below_range + ch.above_range;
		if(finite_samples == 0) {
			ch.min_finite = 0.0;
			ch.max_finite = 0.0;
			ch.mean = 0.0;
		}
		else {
			ch.mean = sums[c] / finite_samples;
		}
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

void HistogramProbeData::Clear()
{
	channel_names.Clear();
	source_values.Clear();
	is_finite.Clear();
	in_range.Clear();
	below_range.Clear();
	above_range.Clear();
}

void BuildProbeData(HistogramProbeData& data, const float* source_pixels, int source_channels,
                    int red_ch, int green_ch, int blue_ch, int alpha_ch, int single_ch)
{
	data.Clear();
	if(!source_pixels || source_channels <= 0)
		return;

	auto add = [&](int src_ch, const String& name) {
		if(src_ch < 0 || src_ch >= source_channels) {
			data.channel_names.Add(name);
			data.source_values.Add(0.0);
			data.is_finite.Add(false);
			data.in_range.Add(false);
			data.below_range.Add(false);
			data.above_range.Add(false);
			return;
		}
		const float v = source_pixels[src_ch];
		const bool finite = std::isfinite(v);
		const double vd = finite ? (double)v : 0.0;
		const bool below = finite && vd < HistogramData::RANGE_MIN;
		const bool above = finite && vd > HistogramData::RANGE_MAX;
		const bool in_range = finite && !below && !above;
		data.channel_names.Add(name);
		data.source_values.Add(vd);
		data.is_finite.Add(finite);
		data.in_range.Add(in_range);
		data.below_range.Add(below);
		data.above_range.Add(above);
	};

	auto add_if_valid = [&](int src_ch, const String& name) {
		if(src_ch >= 0 && src_ch < source_channels)
			add(src_ch, name);
	};

	if(single_ch >= 0 && single_ch < source_channels) {
		add(single_ch, "Gray");
	}
	else {
		add_if_valid(red_ch, "R");
		add_if_valid(green_ch, "G");
		add_if_valid(blue_ch, "B");
		add_if_valid(alpha_ch, "A");
	}
}

void BuildProbeDataFromBuffer(HistogramProbeData& data, const Vector<float>& source_pixels,
                              int source_channels,
                              int red_ch, int green_ch, int blue_ch, int alpha_ch, int single_ch)
{
	BuildProbeData(data, source_pixels.Begin(), source_channels,
	               red_ch, green_ch, blue_ch, alpha_ch, single_ch);
}

HistogramMarkerPosition ComputeMarkerPosition(double source_value, int plot_left, int plot_w)
{
	HistogramMarkerPosition pos;
	if(plot_w <= 0) {
		pos.drawable = false;
		return pos;
	}
	if(!std::isfinite(source_value))
		return pos;
	if(source_value <= HistogramData::RANGE_MIN) {
		pos.x = plot_left;
		pos.in_range = (source_value >= HistogramData::RANGE_MIN);
		pos.clamped_left = (source_value < HistogramData::RANGE_MIN);
		pos.drawable = true;
		return pos;
	}
	if(source_value >= HistogramData::RANGE_MAX) {
		pos.x = plot_left + plot_w - 1;
		pos.in_range = (source_value <= HistogramData::RANGE_MAX);
		pos.clamped_right = (source_value > HistogramData::RANGE_MAX);
		pos.drawable = true;
		return pos;
	}
	double t = (source_value - HistogramData::RANGE_MIN) / (HistogramData::RANGE_MAX - HistogramData::RANGE_MIN);
	pos.x = plot_left + (int)(t * plot_w);
	if(pos.x >= plot_left + plot_w) pos.x = plot_left + plot_w - 1;
	if(pos.x < plot_left) pos.x = plot_left;
	pos.in_range = true;
	pos.drawable = true;
	return pos;
}

} // namespace Upp
