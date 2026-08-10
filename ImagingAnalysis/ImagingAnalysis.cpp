#include "ImagingAnalysis.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace Upp {
namespace Imaging {

namespace {

static void Reset(Diagnostics* diagnostics)
{
	if(diagnostics)
		diagnostics->Clear();
}

static Result Fail(ResultCode code, Diagnostics* diagnostics,
                   const String& message, const char* diagnostic,
                   const String& context = Null)
{
	if(diagnostics)
		diagnostics->Error(message, diagnostic, context);
	return Result::Failure(code, message, context);
}

static String ChannelName(const ImageSpec& spec, int channel)
{
	if(spec.channel_names.GetCount() == spec.channels)
		return spec.channel_names[channel];

	switch(spec.channel_layout) {
	case ChannelLayout::Gray:
		return "Y";
	case ChannelLayout::GrayAlpha:
		return channel == 0 ? "Y" : "A";
	case ChannelLayout::RGB:
		return channel == 0 ? "R" : channel == 1 ? "G" : "B";
	case ChannelLayout::RGBA:
		return channel == 0 ? "R" : channel == 1 ? "G" : channel == 2 ? "B" : "A";
	default:
		return Format("C%d", channel);
	}
}

static double HalfToDouble(uint16 value)
{
	const bool negative = (value & 0x8000u) != 0;
	const int exponent = (value >> 10) & 0x1f;
	const int mantissa = value & 0x03ff;

	double result;
	if(exponent == 0) {
		result = mantissa == 0 ? 0.0 : std::ldexp((double)mantissa, -24);
	}
	else if(exponent == 31) {
		result = mantissa == 0 ? std::numeric_limits<double>::infinity()
		                       : std::numeric_limits<double>::quiet_NaN();
	}
	else {
		result = std::ldexp(1.0 + (double)mantissa / 1024.0, exponent - 15);
	}
	return negative ? -result : result;
}

static double ReadSample(const ImageData& image, int64 sample)
{
	const byte* bytes = image.buffer.Begin();
	switch(image.spec.sample_type) {
	case SampleType::UInt8:
		return (double)bytes[sample] / 255.0;
	case SampleType::UInt16: {
		uint16 value;
		memcpy(&value, bytes + sample * 2, sizeof(value));
		return (double)value / 65535.0;
	}
	case SampleType::Float16: {
		uint16 value;
		memcpy(&value, bytes + sample * 2, sizeof(value));
		return HalfToDouble(value);
	}
	case SampleType::Float32: {
		float value;
		memcpy(&value, bytes + sample * 4, sizeof(value));
		return (double)value;
	}
	default:
		return std::numeric_limits<double>::quiet_NaN();
	}
}

static Result ValidateImage(const ImageData& image, Diagnostics* diagnostics)
{
	if(!image.IsValid())
		return Fail(ResultCode::InvalidSpecification, diagnostics,
		            "image is invalid", "IMGANALYSIS_SPEC");
	return Result::Success();
}

static void InitStatistics(const ImageSpec& spec,
                           Vector<ChannelStatistics>& statistics)
{
	statistics.SetCount(spec.channels);
	for(int c = 0; c < spec.channels; ++c) {
		ChannelStatistics& item = statistics[c];
		item.channel = c;
		item.name = ChannelName(spec, c);
		item.min_finite = std::numeric_limits<double>::infinity();
		item.max_finite = -std::numeric_limits<double>::infinity();
	}
}

static void FinalizeStatistics(Vector<ChannelStatistics>& statistics,
                               const Vector<long double>& sums)
{
	for(int c = 0; c < statistics.GetCount(); ++c) {
		ChannelStatistics& item = statistics[c];
		if(item.finite_samples == 0) {
			item.min_finite = 0.0;
			item.max_finite = 0.0;
			item.mean = 0.0;
		}
		else {
			item.mean = (double)(sums[c] / (long double)item.finite_samples);
		}
	}
}

static Result Analyze(const ImageData& image,
                      Vector<ChannelStatistics>& statistics,
                      Vector<HistogramChannel>* histogram_channels,
                      int bin_count, Diagnostics* diagnostics)
{
	Result valid = ValidateImage(image, diagnostics);
	if(!valid)
		return valid;

	Vector<ChannelStatistics> temp;
	InitStatistics(image.spec, temp);
	Vector<long double> sums;
	sums.SetCount(image.spec.channels, 0.0L);

	Vector<HistogramChannel> hist;
	if(histogram_channels) {
		hist.SetCount(image.spec.channels);
		for(int c = 0; c < image.spec.channels; ++c)
			hist[c].bins.SetCount(bin_count, 0);
	}

	int64 sample_count = image.buffer.GetSampleCount();
	for(int64 sample = 0; sample < sample_count; ++sample) {
		int channel = (int)(sample % image.spec.channels);
		ChannelStatistics& item = temp[channel];
		double value = ReadSample(image, sample);
		if(!std::isfinite(value)) {
			++item.non_finite;
			continue;
		}

		++item.finite_samples;
		item.min_finite = std::min(item.min_finite, value);
		item.max_finite = std::max(item.max_finite, value);
		sums[channel] += (long double)value;

		if(value < Histogram::RANGE_MIN) {
			++item.below_range;
			continue;
		}
		if(value > Histogram::RANGE_MAX) {
			++item.above_range;
			continue;
		}

		++item.in_range_samples;
		if(histogram_channels) {
			int bin = value >= Histogram::RANGE_MAX
			        ? bin_count - 1
			        : (int)(value * bin_count);
			bin = std::clamp(bin, 0, bin_count - 1);
			++hist[channel].bins[bin];
		}
	}

	FinalizeStatistics(temp, sums);
	if(histogram_channels) {
		for(int c = 0; c < hist.GetCount(); ++c)
			hist[c].statistics = temp[c];
		*histogram_channels = pick(hist);
	}
	statistics = pick(temp);
	return Result::Success();
}

} // namespace

Result ComputeChannelStatistics(const ImageData& image,
                                Vector<ChannelStatistics>& output,
                                Diagnostics* diagnostics)
{
	Reset(diagnostics);
	Vector<ChannelStatistics> temp;
	Result result = Analyze(image, temp, nullptr, 0, diagnostics);
	if(result)
		output = pick(temp);
	return result;
}

Result ComputeHistogram(const ImageData& image, Histogram& output,
                        int bin_count, Diagnostics* diagnostics)
{
	Reset(diagnostics);
	if(bin_count < 2 || bin_count > 65536)
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "histogram bin count must be between 2 and 65536",
		            "IMGANALYSIS_BINS", AsString(bin_count));

	Vector<ChannelStatistics> statistics;
	Vector<HistogramChannel> channels;
	Result result = Analyze(image, statistics, &channels, bin_count, diagnostics);
	if(!result)
		return result;

	int64 pixels = 0;
	if(!image.spec.GetPixelCount(pixels))
		return Fail(ResultCode::Overflow, diagnostics,
		            "image pixel count overflow", "IMGANALYSIS_SPEC");

	output.Clear();
	output.bin_count = bin_count;
	output.analyzed_pixels = pixels;
	output.channels = pick(channels);
	return Result::Success();
}

Result ProbePixel(const ImageData& image, int x, int y, int z,
                  PixelProbe& output, Diagnostics* diagnostics)
{
	Reset(diagnostics);
	Result valid = ValidateImage(image, diagnostics);
	if(!valid)
		return valid;

	if(x < image.spec.data_window.left || x > image.spec.data_window.right ||
	   y < image.spec.data_window.top || y > image.spec.data_window.bottom ||
	   z < 0 || z >= image.spec.depth)
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "probe coordinate is outside the image",
		            "IMGANALYSIS_COORD", Format("%d,%d,%d", x, y, z));

	int64 width = 0;
	int64 height = 0;
	image.spec.GetWidth(width);
	image.spec.GetHeight(height);
	int64 x_offset = (int64)x - image.spec.data_window.left;
	int64 y_offset = (int64)y - image.spec.data_window.top;
	int64 pixel = ((int64)z * height + y_offset) * width + x_offset;
	int64 first_sample = pixel * image.spec.channels;

	Vector<PixelProbeValue> values;
	values.SetCount(image.spec.channels);
	for(int c = 0; c < image.spec.channels; ++c) {
		PixelProbeValue& item = values[c];
		item.channel = c;
		item.name = ChannelName(image.spec, c);
		item.value = ReadSample(image, first_sample + c);
		item.finite = std::isfinite(item.value);
		item.below_range = item.finite && item.value < Histogram::RANGE_MIN;
		item.above_range = item.finite && item.value > Histogram::RANGE_MAX;
		item.in_range = item.finite && !item.below_range && !item.above_range;
	}

	output.Clear();
	output.x = x;
	output.y = y;
	output.z = z;
	output.values = pick(values);
	return Result::Success();
}

} // namespace Imaging
} // namespace Upp
