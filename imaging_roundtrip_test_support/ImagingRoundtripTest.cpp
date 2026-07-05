#include "ImagingRoundtripTest.h"

#include <math.h>

using namespace Upp;

RoundtripComparison::RoundtripComparison()
{
	dimensions_match = true;
	different_components = 0;
	max_error_r = 0.0;
	max_error_g = 0.0;
	max_error_b = 0.0;
	max_error_a = 0.0;
	mean_absolute_error = 0.0;
	rmse = 0.0;
}

static float ClampFloat(float v, float lo, float hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

static byte ClampByte(float v)
{
	v = ClampFloat(v, 0.0f, 1.0f);
	return (byte)(v * 255.0f + 0.5f);
}

static float QuantizeHalfExact(float v)
{
	static const float levels[] = {0.0f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f};
	float best = levels[0];
	float best_err = fabsf(v - best);
	for(int i = 1; i < (int)(sizeof(levels) / sizeof(levels[0])); ++i) {
		float err = fabsf(v - levels[i]);
		if(err < best_err) {
			best = levels[i];
			best_err = err;
		}
	}
	return best;
}

static TestRgbaF QuantizePixel(const TestRgbaF& in)
{
	TestRgbaF out = in;
	out.r = QuantizeHalfExact(out.r);
	out.g = QuantizeHalfExact(out.g);
	out.b = QuantizeHalfExact(out.b);
	out.a = QuantizeHalfExact(out.a);
	return out;
}

static TestRgbaF HsvToRgb(float h, float s, float v)
{
	TestRgbaF out = {0, 0, 0, 1};
	if(s <= 0.0f) {
		out.r = out.g = out.b = v;
		return out;
	}
	h = fmodf(h, 360.0f);
	if(h < 0.0f)
		h += 360.0f;
	float hh = h / 60.0f;
	int i = (int)floorf(hh);
	float ff = hh - (float)i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - s * ff);
	float t = v * (1.0f - s * (1.0f - ff));
	switch(i) {
	case 0: out.r = v; out.g = t; out.b = p; break;
	case 1: out.r = q; out.g = v; out.b = p; break;
	case 2: out.r = p; out.g = v; out.b = t; break;
	case 3: out.r = p; out.g = q; out.b = v; break;
	case 4: out.r = t; out.g = p; out.b = v; break;
	default: out.r = v; out.g = p; out.b = q; break;
	}
	return out;
}

static float ExactPaletteValue(int idx)
{
	static const float levels[] = {0.0f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f};
	return levels[idx % (int)(sizeof(levels) / sizeof(levels[0]))];
}

static TestRgbaF MakeHueValuePixel(int x, int y, bool include_hdr)
{
	float fx = (float)x / 127.0f;
	float fy = (float)y / 63.0f;
	TestRgbaF p = HsvToRgb(360.0f * fx, 1.0f, 1.0f - fy);
	p.a = 1.0f - 0.5f * fy;
	if(!include_hdr)
		p = QuantizePixel(p);
	return p;
}

static TestRgbaF MakeSatValuePixel(int x, int y, bool include_hdr)
{
	float fx = (float)x / 127.0f;
	float fy = (float)y / 63.0f;
	TestRgbaF p = HsvToRgb(210.0f, fx, 1.0f - fy);
	p.a = fx;
	if(!include_hdr)
		p = QuantizePixel(p);
	return p;
}

static TestRgbaF MakeRampPixel(int x, int y, int lane, bool include_hdr)
{
	float fx = (float)x / 63.0f;
	float fy = (float)y / 63.0f;
	TestRgbaF p = {0, 0, 0, 1};
	switch(lane) {
	case 0: p.r = fx; p.g = 0.25f; p.b = 0.0f; break;
	case 1: p.r = 0.0f; p.g = fx; p.b = 0.25f; break;
	case 2: p.r = 0.25f; p.g = 0.0f; p.b = fx; break;
	default: p.r = p.g = p.b = fx; break;
	}
	p.a = fy;
	if(!include_hdr)
		p = QuantizePixel(p);
	return p;
}

static TestRgbaF MakeAlphaTestPixel(int x, int y, bool include_hdr)
{
	TestRgbaF p = {1.0f, 0.25f, 0.5f, 0.0f};
	if(y >= 8)
		p.a = (float)x / 63.0f;
	if((y & 1) == 0)
		p.r = 0.5f;
	if(!include_hdr)
		p = QuantizePixel(p);
	return p;
}

static TestRgbaF MakeHardPatchPixel(int x, int y, bool include_hdr)
{
	static const TestRgbaF patches[] = {
		{1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, {0, 1, 1, 1},
		{1, 0, 1, 1}, {1, 1, 0, 1}, {1, 1, 1, 1}, {0, 0, 0, 1},
		{1, 0.5f, 0.25f, 0}, {0.25f, 1, 0.5f, 0}, {0.5f, 0.25f, 1, 0}, {1, 0.25f, 0.5f, 0},
		{0.25f, 0.5f, 1, 0}, {0.5f, 1, 0.25f, 0}, {1, 0.75f, 0.25f, 0}, {0.25f, 1, 0.75f, 0}
	};
	int px = x / 16;
	int py = y / 16;
	TestRgbaF p = patches[(py * 4 + px) % (int)(sizeof(patches) / sizeof(patches[0]))];
	if(!include_hdr)
		p = QuantizePixel(p);
	return p;
}

static TestRgbaF MakeFineDetailPixel(int x, int y, bool include_hdr)
{
	TestRgbaF p = {0, 0, 0, 1};
	int qx = x / 32;
	int qy = y / 32;
	int lx = x & 31;
	int ly = y & 31;
	if(qx == 0) {
		p.r = (lx & 1) ? 0.0f : 1.0f;
		p.g = 0.25f;
		p.b = 0.5f;
	}
	else if(qx == 1) {
		p.r = 0.25f;
		p.g = (ly & 1) ? 0.0f : 1.0f;
		p.b = 0.5f;
	}
	else if(qx == 2) {
		p.r = (((lx ^ ly) & 1) != 0) ? 1.0f : 0.0f;
		p.g = (((lx + ly) & 1) != 0) ? 0.5f : 0.25f;
		p.b = 0.75f;
	}
	else {
		p.r = ((x + y) & 1) ? 1.0f : 0.0f;
		p.g = ((x & 1) ? 0.25f : 0.5f);
		p.b = ((y & 1) ? 0.5f : 0.25f);
	}
	p.a = ((x + y) & 1) ? 0.5f : 1.0f;
	if(qy == 1)
		p.a = ((x ^ y) & 1) ? 0.0f : 1.0f;
	if(!include_hdr)
		p = QuantizePixel(p);
	return p;
}

static TestRgbaF MakeHdrPixel(int x, int y, bool include_hdr)
{
	static const float levels[] = {0.0f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f};
	int band = x / 11;
	float fx = (float)(x % 11) / 10.0f;
	float fy = (float)y / 63.0f;
	TestRgbaF p;
	if(include_hdr) {
		float base = levels[band % 6];
		p.r = base + fx * 4.0f;
		p.g = levels[(band + 2) % 6] + fy * 2.0f;
		p.b = levels[(band + 4) % 6] + (fx * fy) * 4.0f;
		p.a = levels[(band + 1) % 6] > 0.0f ? 1.0f : 0.0f;
	}
	else {
		p.r = ExactPaletteValue(band);
		p.g = ExactPaletteValue((band + 2) % 6);
		p.b = ExactPaletteValue((band + 4) % 6);
		p.a = 1.0f;
	}
	return include_hdr ? p : QuantizePixel(p);
}

TestImageF GenerateRoundtripTestPattern(int width, int height, bool include_hdr)
{
	TestImageF out;
	out.width = width;
	out.height = height;
	if(width <= 0 || height <= 0) {
		out.pixels.Clear();
		return out;
	}
	out.pixels.SetCount(width * height);

	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			TestRgbaF p = {0, 0, 0, 1};
			if(y < 64) {
				if(x < 128)
					p = MakeHueValuePixel(x, y, include_hdr);
				else
					p = MakeSatValuePixel(x - 128, y, include_hdr);
			}
			else if(y < 128) {
				if(x < 64)
					p = MakeRampPixel(x, y - 64, 0, include_hdr);
				else if(x < 128)
					p = MakeRampPixel(x - 64, y - 64, 1, include_hdr);
				else if(x < 192)
					p = MakeRampPixel(x - 128, y - 64, 2, include_hdr);
				else
					p = MakeRampPixel(x - 192, y - 64, 3, include_hdr);
			}
			else {
				if(x < 64)
					p = MakeAlphaTestPixel(x, y - 128, include_hdr);
				else if(x < 128)
					p = MakeHardPatchPixel(x - 64, y - 128, include_hdr);
				else if(x < 192)
					p = MakeFineDetailPixel(x - 128, y - 128, include_hdr);
				else
					p = MakeHdrPixel(x - 192, y - 128, include_hdr);
			}
			out.pixels[y * width + x] = p;
		}
	}
	return out;
}

void TestImage8::Clear()
{
	width = 0;
	height = 0;
	pixels.Clear();
}

bool TestImage8::IsValid() const
{
	return width > 0 && height > 0 && pixels.GetCount() == (int64)width * height;
}

TestImage8 QuantizeToRgba8(const TestImageF& source)
{
	TestImage8 out;
	out.width = source.width;
	out.height = source.height;
	if(source.width <= 0 || source.height <= 0)
		return out;
	const int64 count = (int64)source.width * source.height;
	out.pixels.SetCount((int)count);
	for(int i = 0; i < source.pixels.GetCount(); ++i) {
		const TestRgbaF& p = source.pixels[i];
		out.pixels[i].r = ClampByte(p.r);
		out.pixels[i].g = ClampByte(p.g);
		out.pixels[i].b = ClampByte(p.b);
		out.pixels[i].a = ClampByte(p.a);
	}
	return out;
}

TestImageF NormalizeToFloat(const TestImage8& source)
{
	TestImageF out;
	out.width = source.width;
	out.height = source.height;
	if(!source.IsValid())
		return out;
	out.pixels.SetCount(source.pixels.GetCount());
	for(int i = 0; i < source.pixels.GetCount(); ++i) {
		const TestRgba8& p = source.pixels[i];
		out.pixels[i].r = (float)p.r / 255.0f;
		out.pixels[i].g = (float)p.g / 255.0f;
		out.pixels[i].b = (float)p.b / 255.0f;
		out.pixels[i].a = (float)p.a / 255.0f;
	}
	return out;
}

static bool IsValidImageBuffer8(const TestImage8& img)
{
	return img.IsValid();
}

static RoundtripComparison8 MakeMalformedComparison8(const char* text)
{
	RoundtripComparison8 cmp;
	cmp.dimensions_match = false;
	cmp.different_components = 1;
	cmp.summary = text;
	return cmp;
}

static RoundtripComparison8 CompareImpl(const TestImage8& expected, const TestImage8& actual)
{
	RoundtripComparison8 cmp;
	if(!IsValidImageBuffer8(expected))
		return MakeMalformedComparison8("malformed expected image object");
	if(!IsValidImageBuffer8(actual))
		return MakeMalformedComparison8("malformed actual image object");
	if(expected.width != actual.width || expected.height != actual.height)
		cmp.dimensions_match = false;

	const int w = expected.width < actual.width ? expected.width : actual.width;
	const int h = expected.height < actual.height ? expected.height : actual.height;
	for(int y = 0; y < h; ++y) {
		for(int x = 0; x < w; ++x) {
			const TestRgba8& e = expected.pixels[y * expected.width + x];
			const TestRgba8& a = actual.pixels[y * actual.width + x];
			const int dr = abs((int)e.r - (int)a.r);
			const int dg = abs((int)e.g - (int)a.g);
			const int db = abs((int)e.b - (int)a.b);
			const int da = abs((int)e.a - (int)a.a);
			cmp.max_error_r = dr > cmp.max_error_r ? dr : cmp.max_error_r;
			cmp.max_error_g = dg > cmp.max_error_g ? dg : cmp.max_error_g;
			cmp.max_error_b = db > cmp.max_error_b ? db : cmp.max_error_b;
			cmp.max_error_a = da > cmp.max_error_a ? da : cmp.max_error_a;
			cmp.different_components += (dr != 0) + (dg != 0) + (db != 0) + (da != 0);
		}
	}
	if(!cmp.dimensions_match)
	{
		int64 pixel_diff = (int64)expected.width * expected.height - (int64)actual.width * actual.height;
		if(pixel_diff < 0)
			pixel_diff = -pixel_diff;
		cmp.different_components += (int)(pixel_diff * 4);
	}
	if(cmp.different_components == 0 && cmp.dimensions_match)
		cmp.summary = "OK";
	else if(!cmp.dimensions_match)
		cmp.summary = Format("dimension mismatch: expected=%dx%d actual=%dx%d",
			expected.width, expected.height, actual.width, actual.height);
	else
		cmp.summary = Format("dimensions_match=%s different_components=%d max_error_r=%d max_error_g=%d max_error_b=%d max_error_a=%d",
			cmp.dimensions_match ? "true" : "false",
			cmp.different_components,
			cmp.max_error_r,
			cmp.max_error_g,
			cmp.max_error_b,
			cmp.max_error_a);
	return cmp;
}

RoundtripComparison8 CompareExact(const TestImage8& expected, const TestImage8& actual)
{
	return CompareImpl(expected, actual);
}

static bool IsValidImageBuffer(const TestImageF& img)
{
	if(img.width <= 0 || img.height <= 0)
		return false;
	const int64 required = (int64)img.width * (int64)img.height;
	return img.pixels.GetCount() >= required;
}

static RoundtripComparison MakeMalformedComparison(const char* text)
{
	RoundtripComparison cmp;
	cmp.dimensions_match = false;
	cmp.different_components = 1;
	cmp.summary = text;
	return cmp;
}

static RoundtripComparison CompareImpl(const TestImageF& expected, const TestImageF& actual, double tolerance)
{
	RoundtripComparison cmp;
	if(!IsValidImageBuffer(expected))
		return MakeMalformedComparison("malformed expected image object");
	if(!IsValidImageBuffer(actual))
		return MakeMalformedComparison("malformed actual image object");
	if(expected.width != actual.width || expected.height != actual.height)
		cmp.dimensions_match = false;

	int w = expected.width < actual.width ? expected.width : actual.width;
	int h = expected.height < actual.height ? expected.height : actual.height;
	double abs_sum = 0.0;
	double sq_sum = 0.0;
	int compared_components = 0;
	for(int y = 0; y < h; ++y) {
		for(int x = 0; x < w; ++x) {
			const TestRgbaF& e = expected.pixels[y * expected.width + x];
			const TestRgbaF& a = actual.pixels[y * actual.width + x];
			const double dr = fabs((double)e.r - (double)a.r);
			const double dg = fabs((double)e.g - (double)a.g);
			const double db = fabs((double)e.b - (double)a.b);
			const double da = fabs((double)e.a - (double)a.a);
			cmp.max_error_r = dr > cmp.max_error_r ? dr : cmp.max_error_r;
			cmp.max_error_g = dg > cmp.max_error_g ? dg : cmp.max_error_g;
			cmp.max_error_b = db > cmp.max_error_b ? db : cmp.max_error_b;
			cmp.max_error_a = da > cmp.max_error_a ? da : cmp.max_error_a;
			const double errs[4] = {dr, dg, db, da};
			for(int i = 0; i < 4; ++i) {
				abs_sum += errs[i];
				sq_sum += errs[i] * errs[i];
				++compared_components;
				if(errs[i] > tolerance)
					++cmp.different_components;
			}
		}
	}
	if(!cmp.dimensions_match)
		cmp.different_components += abs(expected.width * expected.height - actual.width * actual.height) * 4;
	if(compared_components > 0) {
		cmp.mean_absolute_error = abs_sum / (double)compared_components;
		cmp.rmse = sqrt(sq_sum / (double)compared_components);
	}
	if(!cmp.dimensions_match)
		cmp.summary = Format("dimension mismatch: expected=%dx%d actual=%dx%d",
			expected.width, expected.height, actual.width, actual.height);
	else
		cmp.summary = Format("dimensions_match=%s different_components=%d max_error_r=%.9g max_error_g=%.9g max_error_b=%.9g max_error_a=%.9g mean_absolute_error=%.9g rmse=%.9g",
			cmp.dimensions_match ? "true" : "false",
			cmp.different_components,
			cmp.max_error_r,
			cmp.max_error_g,
			cmp.max_error_b,
			cmp.max_error_a,
			cmp.mean_absolute_error,
			cmp.rmse);
	if(cmp.different_components == 0 && cmp.dimensions_match)
		cmp.summary = "OK";
	return cmp;
}

RoundtripComparison CompareExact(const TestImageF& expected, const TestImageF& actual)
{
	return CompareImpl(expected, actual, 0.0);
}

RoundtripComparison CompareTolerance(const TestImageF& expected, const TestImageF& actual, double tolerance)
{
	return CompareImpl(expected, actual, tolerance);
}
