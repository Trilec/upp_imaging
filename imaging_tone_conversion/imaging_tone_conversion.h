#ifndef _imaging_tone_conversion_imaging_tone_conversion_h_
#define _imaging_tone_conversion_imaging_tone_conversion_h_

#include <Core/Core.h>

namespace Upp {

struct ToneConversionState {
	float exposure_scale = 1.0f;
	float inverse_gamma = 1.0f;
	Vector<float> lut;
};

// Policy:
// NaN -> black, -inf -> black, +inf -> white.
// Finite values that overflow after exposure also saturate white/black by sign.
ToneConversionState PrepareToneConversion(double exposure_stops, double display_gamma);
void UpdateToneExposure(ToneConversionState& tone, double exposure_stops);
float ApplyToneExposureGamma(float value, double exposure_stops, double gamma);
void BuildToneLut(Vector<float>& lut, double display_gamma);
byte ToneToByte(float source_value, const ToneConversionState& tone);

} // namespace Upp

#endif
