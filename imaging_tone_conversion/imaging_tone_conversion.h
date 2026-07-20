#ifndef _imaging_tone_conversion_imaging_tone_conversion_h_
#define _imaging_tone_conversion_imaging_tone_conversion_h_

#include <Core/Core.h>

namespace Upp {

struct ToneLutConstants {
	double display_gamma = 1.0;
	float inverse_gamma = 1.0f;
};

// Policy:
// NaN -> black, -inf -> black, +inf -> white.
// Finite values that overflow after exposure also saturate white/black by sign.
ToneLutConstants PrepareToneLut(double display_gamma);
float ApplyToneExposureGamma(float value, double exposure_stops, double gamma);
void BuildToneLut(Vector<float>& lut, double display_gamma);
byte ToneToByte(float value, double exposure_stops, const Vector<float>& lut);

} // namespace Upp

#endif
