#include "imaging_tone_conversion.h"

#include <algorithm>
#include <cmath>

namespace Upp {

void BuildToneLut(Vector<float>& lut, double display_gamma);

namespace {

static int ClampByte(float value)
{
	if(!std::isfinite(value))
		value = 0.0f;
	value = std::clamp(value, 0.0f, 1.0f);
	return (int)(value * 255.0f + 0.5f);
}

} // namespace

void UpdateToneExposure(ToneConversionState& tone, double exposure_stops)
{
	if(!std::isfinite(exposure_stops))
		exposure_stops = 0.0;
	tone.exposure_scale = (float)std::pow(2.0, exposure_stops);
	if(!std::isfinite(tone.exposure_scale))
		tone.exposure_scale = exposure_stops > 0.0 ? 1.0e30f : 0.0f;
}

ToneConversionState PrepareToneConversion(double exposure_stops, double display_gamma)
{
	ToneConversionState tone;
	UpdateToneExposure(tone, exposure_stops);
	double gamma = std::isfinite(display_gamma) && display_gamma > 0.0 ? display_gamma : 1.0;
	tone.inverse_gamma = 1.0f / (float)gamma;
	BuildToneLut(tone.lut, gamma);
	return tone;
}

float ApplyToneExposureGamma(float value, double exposure_stops, double gamma)
{
	if(std::isnan(value) || value == 0.0f)
		return 0.0f;
	if(value > 0.0f && !std::isfinite(value))
		return 1.0f;
	if(value < 0.0f && !std::isfinite(value))
		return 0.0f;
	if(!std::isfinite(exposure_stops))
		exposure_stops = 0.0;
	if(!std::isfinite(gamma) || gamma <= 0.0)
		gamma = 1.0;
	double scaled = (double)value * std::pow(2.0, exposure_stops);
	if(!std::isfinite(scaled))
		return scaled > 0.0 ? 1.0f : 0.0f;
	if(scaled <= 0.0)
		return 0.0f;
	double corrected = std::pow(scaled, 1.0 / gamma);
	if(!std::isfinite(corrected))
		return corrected > 0.0 ? 1.0f : 0.0f;
	if(corrected <= 0.0)
		return 0.0f;
	if(corrected >= 1.0)
		return 1.0f;
	return (float)corrected;
}

void BuildToneLut(Vector<float>& lut, double display_gamma)
{
	static constexpr int TONE_LUT_SIZE = 65536;
	lut.SetCount(TONE_LUT_SIZE);
	float inverse_gamma = 1.0f / (float)((!std::isfinite(display_gamma) || display_gamma <= 0.0) ? 1.0 : display_gamma);
	for(int i = 0; i < TONE_LUT_SIZE; ++i) {
		float normalized = (float)i / (float)(TONE_LUT_SIZE - 1);
		lut[i] = std::pow(normalized, inverse_gamma);
	}
}

byte ToneToByte(float source_value, const ToneConversionState& tone)
{
	if(tone.lut.IsEmpty())
		return 0;
	if(std::isnan(source_value) || source_value <= 0.0f)
		return 0;
	if(source_value > 0.0f && !std::isfinite(source_value))
		return 255;
	double scaled = (double)source_value * (double)tone.exposure_scale;
	if(!std::isfinite(scaled))
		return scaled > 0.0 ? 255 : 0;
	if(scaled <= 0.0)
		return 0;
	if(scaled >= 1.0)
		return 255;
	double pos = scaled * (double)(tone.lut.GetCount() - 1);
	int idx0 = std::clamp((int)std::floor(pos), 0, tone.lut.GetCount() - 1);
	int idx1 = std::min(idx0 + 1, tone.lut.GetCount() - 1);
	double frac = pos - idx0;
	double blended = (1.0 - frac) * (double)tone.lut[idx0] + frac * (double)tone.lut[idx1];
	return (byte)ClampByte((float)blended);
}

} // namespace Upp
