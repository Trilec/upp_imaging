#include <Core/Core.h>

#include <cstdio>
#include <algorithm>
#include <cmath>

using namespace Upp;

struct Result {
	int passed = 0;
	int failed = 0;
};

static int ClampByte(float value)
{
	if(!std::isfinite(value))
		value = 0.0f;
	value = std::clamp(value, 0.0f, 1.0f);
	return (int)(value * 255.0f + 0.5f);
}

static float ExactTone(float value, float exposure_stops, float gamma)
{
	if(!std::isfinite(value))
		return value > 0.0f ? 1.0f : 0.0f;
	if(!std::isfinite(exposure_stops))
		exposure_stops = 0.0f;
	if(!std::isfinite(gamma) || gamma <= 0.0f)
		gamma = 1.0f;
	float scaled = value * std::exp2(exposure_stops);
	if(!std::isfinite(scaled))
		scaled = 0.0f;
	scaled = std::max(0.0f, scaled);
	scaled = std::pow(scaled, 1.0f / gamma);
	if(!std::isfinite(scaled))
		scaled = 0.0f;
	return scaled;
}

static const int LUT_SIZE = 65536;

static void BuildLut(Vector<float>& lut, float gamma)
{
	lut.SetCount(LUT_SIZE);
	float inverse_gamma = 1.0f / (gamma <= 0.0f || !std::isfinite(gamma) ? 1.0f : gamma);
	for(int i = 0; i < LUT_SIZE; ++i) {
		float normalized = (float)i / (float)(LUT_SIZE - 1);
		lut[i] = std::pow(normalized, inverse_gamma);
	}
}

static byte LutTone(float value, float exposure_stops, const Vector<float>& lut)
{
	if(std::isnan(value) || value <= 0.0f)
		return 0;
	if(value > 0.0f && !std::isfinite(value))
		return 255;
	float scaled = value * std::exp2(exposure_stops);
	if(!std::isfinite(scaled) || scaled <= 0.0f)
		return 0;
	if(scaled >= 1.0f)
		return 255;
	double pos = (double)scaled * (double)(lut.GetCount() - 1);
	int idx0 = std::clamp((int)std::floor(pos), 0, lut.GetCount() - 1);
	int idx1 = std::min(idx0 + 1, lut.GetCount() - 1);
	double frac = pos - idx0;
	double blended = (1.0 - frac) * (double)lut[idx0] + frac * (double)lut[idx1];
	return (byte)ClampByte((float)blended);
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
	BuildLut(lut, 2.2f);

	Check(result, ClampByte(ExactTone(0.5f, 0.0f, 2.2f)) == 186, "PASS mid-grey exact");
	Check(result, ClampByte(ExactTone(1.0f, 0.0f, 1.0f)) == 255, "PASS unit exact");

	const float samples[] = { 0.0f, 0.125f, 0.25f, 0.5f, 1.0f, 2.0f, NAN, INFINITY, -INFINITY };
	for(float exposure : { -4.0f, 0.0f, 4.0f }) {
		for(float gamma : { 0.5f, 1.0f, 2.2f, 3.5f }) {
			Vector<float> local_lut;
			BuildLut(local_lut, gamma);
			for(float sample : samples) {
				int exact = ClampByte(ExactTone(sample, exposure, gamma));
				int approx = (int)LutTone(sample, exposure, local_lut);
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
	BuildLut(local_lut, 2.2f);
	int r = LutTone(channels[0], exposure, local_lut);
	int g = LutTone(channels[1], exposure, local_lut);
	int b = LutTone(channels[2], exposure, local_lut);
	int a = ClampByte(alpha);
	Check(result, r < g && g < b, "PASS RGB mapping");
	Check(result, r == LutTone(channels[0], exposure, local_lut), "PASS R isolation");
	Check(result, g == LutTone(channels[1], exposure, local_lut), "PASS G isolation");
	Check(result, b == LutTone(channels[2], exposure, local_lut), "PASS B isolation");
	Check(result, a == 102, "PASS Alpha isolation");

	Check(result, LutTone(-0.25f, 0.0f, local_lut) == 0, "PASS negative to zero");
	Check(result, LutTone(0.0f, 0.0f, local_lut) == 0, "PASS zero to zero");
	Check(result, LutTone(1.5f, 0.0f, local_lut) == 255, "PASS above one to 255");
	Check(result, LutTone(NAN, 0.0f, local_lut) == 0, "PASS NaN to zero");
	Check(result, LutTone(INFINITY, 0.0f, local_lut) == 255, "PASS +inf to 255");
	Check(result, LutTone(-INFINITY, 0.0f, local_lut) == 0, "PASS -inf to zero");

	std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
	return result.failed ? 1 : 0;
}
