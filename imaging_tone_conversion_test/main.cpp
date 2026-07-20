#include <Core/Core.h>

#include <cstdio>
#include <cmath>

#include <imaging_tone_conversion/imaging_tone_conversion.h>

using namespace Upp;

struct Result {
	int passed = 0;
	int failed = 0;
};

static int ClampByte(float value)
{
	return (int)(std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
}

static bool Check(Result& result, bool condition, const char* label)
{
	std::printf("%s: %s\n", label, condition ? "PASS" : "FAIL");
	if(condition)
		++result.passed;
	else
		++result.failed;
	return condition;
}

static bool CheckByte(Result& result, int got, int want, int tolerance, const char* label, float sample, float exposure, float gamma)
{
	if(std::abs(got - want) > tolerance)
		std::printf("  sample=%g exposure=%g gamma=%g got=%d want=%d\n", (double)sample, (double)exposure, (double)gamma, got, want);
	return Check(result, std::abs(got - want) <= tolerance, label);
}

int main()
{
	Result result;
	Vector<float> lut;
	BuildToneLut(lut, 2.2f);

	Check(result, ClampByte(ApplyToneExposureGamma(0.5f, 0.0f, 2.2f)) == 186, "PASS mid-grey exact");
	Check(result, ClampByte(ApplyToneExposureGamma(1.0f, 0.0f, 1.0f)) == 255, "PASS unit exact");

	const float samples[] = { 0.0f, 0.125f, 0.25f, 0.5f, 1.0f, 2.0f, NAN, INFINITY, -INFINITY };
	for(float exposure : { -4.0f, 0.0f, 4.0f }) {
		for(float gamma : { 0.5f, 1.0f, 2.2f, 3.5f }) {
			Vector<float> local_lut;
			BuildToneLut(local_lut, gamma);
			for(float sample : samples) {
				int exact = ClampByte(ApplyToneExposureGamma(sample, exposure, gamma));
				int approx = (int)ToneToByte(sample, exposure, local_lut);
				CheckByte(result, approx, exact, 1, "PASS LUT vs exact", sample, exposure, gamma);
			}
		}
	}

	Vector<float> channels;
	channels.Add(0.25f);
	channels.Add(0.5f);
	channels.Add(0.75f);
	channels.Add(0.9f);
	float exposure = 0.0f;
	float alpha = 0.4f;
	Vector<float> local_lut;
	BuildToneLut(local_lut, 2.2f);
	int r = ToneToByte(channels[0], exposure, local_lut);
	int g = ToneToByte(channels[1], exposure, local_lut);
	int b = ToneToByte(channels[2], exposure, local_lut);
	int a = ClampByte(alpha);
	Check(result, r < g && g < b, "PASS RGB mapping");
	Check(result, r == ToneToByte(channels[0], exposure, local_lut), "PASS R isolation");
	Check(result, g == ToneToByte(channels[1], exposure, local_lut), "PASS G isolation");
	Check(result, b == ToneToByte(channels[2], exposure, local_lut), "PASS B isolation");
	Check(result, a == 102, "PASS Alpha isolation");

	Check(result, ToneToByte(-0.25f, 0.0f, local_lut) == 0, "PASS negative to zero");
	Check(result, ToneToByte(0.0f, 0.0f, local_lut) == 0, "PASS zero to zero");
	Check(result, ToneToByte(1.5f, 0.0f, local_lut) == 255, "PASS above one to 255");
	Check(result, ToneToByte(NAN, 0.0f, local_lut) == 0, "PASS NaN to zero");
	Check(result, ToneToByte(INFINITY, 0.0f, local_lut) == 255, "PASS +inf to 255");
	Check(result, ToneToByte(-INFINITY, 0.0f, local_lut) == 0, "PASS -inf to zero");

	std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
	return result.failed ? 1 : 0;
}
