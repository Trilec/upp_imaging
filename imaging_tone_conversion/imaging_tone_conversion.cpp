#include "imaging_tone_conversion.h"

#include <algorithm>
#include <cmath>

namespace Upp {

namespace {

static int ClampByte(float value)
{
	if(!std::isfinite(value))
		value = 0.0f;
	value = std::clamp(value, 0.0f, 1.0f);
	return (int)(value * 255.0f + 0.5f);
}

} // namespace

ToneLutConstants PrepareToneLut(double display_gamma)
{
	ToneLutConstants constants;
	constants.display_gamma = std::isfinite(display_gamma) && display_gamma > 0.0 ? display_gamma : 1.0;
	constants.inverse_gamma = 1.0f / (float)constants.display_gamma;
	return constants;
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
	ToneLutConstants constants = PrepareToneLut(display_gamma);
	static constexpr int TONE_LUT_SIZE = 65536;
	lut.SetCount(TONE_LUT_SIZE);
	for(int i = 0; i < TONE_LUT_SIZE; ++i) {
		float normalized = (float)i / (float)(TONE_LUT_SIZE - 1);
		lut[i] = std::pow(normalized, constants.inverse_gamma);
	}
}

byte ToneToByte(float value, double exposure_stops, const Vector<float>& lut)
{
	if(lut.IsEmpty())
		return 0;
	if(std::isnan(value) || value <= 0.0f)
		return 0;
	if(value > 0.0f && !std::isfinite(value))
		return 255;
	if(!std::isfinite(exposure_stops))
		exposure_stops = 0.0;
	double scaled = (double)value * std::pow(2.0, exposure_stops);
	if(!std::isfinite(scaled))
		return scaled > 0.0 ? 255 : 0;
	if(scaled <= 0.0)
		return 0;
	if(scaled >= 1.0)
		return 255;
	double pos = scaled * (double)(lut.GetCount() - 1);
	int idx0 = std::clamp((int)std::floor(pos), 0, lut.GetCount() - 1);
	int idx1 = std::min(idx0 + 1, lut.GetCount() - 1);
	double frac = pos - idx0;
	double blended = (1.0 - frac) * (double)lut[idx0] + frac * (double)lut[idx1];
	return (byte)ClampByte((float)blended);
}

} // namespace Upp
