#include <stdio.h>

#include <imaging_roundtrip_test_support/ImagingRoundtripTest.h>
#include <png_io/PngIO.h>

using namespace Upp;

static PngRgbaImage8 ToPngRgbaImage8(const TestImage8& src)
{
	PngRgbaImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
		out.pixels[i].a = src.pixels[i].a;
	}
	return out;
}

static TestImage8 ToTestImage8(const PngRgbaImage8& src)
{
	TestImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
		out.pixels[i].a = src.pixels[i].a;
	}
	return out;
}

static void PrintByteMetrics(const char* label, int width, int height, const RoundtripComparison8& cmp)
{
	printf("%s dimensions=%dx%d different_components=%d max_error_r=%d max_error_g=%d max_error_b=%d max_error_a=%d\n",
		label,
		width,
		height,
		cmp.different_components,
		cmp.max_error_r,
		cmp.max_error_g,
		cmp.max_error_b,
		cmp.max_error_a);
}

static bool IsExactPass(const RoundtripComparison8& cmp)
{
	return cmp.dimensions_match && cmp.different_components == 0 && cmp.max_error_r == 0 && cmp.max_error_g == 0 && cmp.max_error_b == 0 && cmp.max_error_a == 0;
}

static bool HasHiddenRgb(const TestImage8& expected, const TestImage8& actual)
{
	if(!expected.IsValid() || !actual.IsValid() || expected.width != actual.width || expected.height != actual.height)
		return false;
	for(int i = 0; i < expected.pixels.GetCount(); ++i) {
		const TestRgba8& e = expected.pixels[i];
		const TestRgba8& a = actual.pixels[i];
		if(e.a == 0 && a.a == 0)
			return e.r == a.r && e.g == a.g && e.b == a.b;
	}
	return false;
}

static bool WriteTruncatedFile(const char* path)
{
	FileOut out(path);
	if(!out.IsOpen())
		return false;
	const byte junk[] = {0x89, 0x50, 0x4e, 0x47, 0x00, 0x01};
	out.Put(junk, sizeof(junk));
	return true;
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;
	String error;

	const String path = GetExeDirFile("png_io_roundtrip.png");
	const String bad_path = GetExeDirFile("png_io_truncated.png");
	const String missing_path = GetExeDirFile("png_io_missing_input.png");
	FileDelete(missing_path);

	TestImageF generated_float = GenerateRoundtripTestPattern(256, 192, false);
	printf("PNG IO chart: OK\n");
	++passed;

	TestImage8 expected = QuantizeToRgba8(generated_float);
	PngRgbaImage8 save_image = ToPngRgbaImage8(expected);
	if(!SavePngRgba8(~path, save_image, &error)) {
		printf("PNG IO save: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("PNG IO save: OK\n");
	++passed;

	PngRgbaImage8 loaded_image;
	error.Clear();
	if(!LoadPngRgba8(~path, loaded_image, &error)) {
		printf("PNG IO load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("PNG IO load: OK\n");
	++passed;

	TestImage8 actual = ToTestImage8(loaded_image);
	RoundtripComparison8 cmp = CompareExact(expected, actual);
	PrintByteMetrics("PNG IO", expected.width, expected.height, cmp);
	if(IsExactPass(cmp)) {
		printf("PNG IO dimensions: OK\n");
		++passed;
		printf("PNG IO hidden RGB under zero alpha: %s\n", HasHiddenRgb(expected, actual) ? "OK" : "FAIL");
		if(HasHiddenRgb(expected, actual))
			++passed;
		else
			++failed;
		printf("PNG IO exact byte comparison: OK\n");
		++passed;
	}
	else {
		printf("PNG IO dimensions: FAIL\n");
		printf("PNG IO hidden RGB under zero alpha: FAIL\n");
		printf("PNG IO exact byte comparison: FAIL\n");
		failed += 3;
	}

	WriteTruncatedFile(~bad_path);

	bool validation_ok = true;
	PngRgbaImage8 invalid_image;
	error.Clear();
	if(SavePngRgba8("", save_image, &error) || IsNull(error))
		validation_ok = false;
	error.Clear();
	PngRgbaImage8 zero;
	if(SavePngRgba8(~path, zero, &error) || IsNull(error))
		validation_ok = false;
	error.Clear();
	PngRgbaImage8 mismatch;
	mismatch.width = save_image.width;
	mismatch.height = save_image.height;
	mismatch.pixels.SetCount(save_image.pixels.GetCount());
	for(int i = 0; i < save_image.pixels.GetCount(); ++i)
		mismatch.pixels[i] = save_image.pixels[i];
	mismatch.pixels.SetCount(mismatch.pixels.GetCount() - 1);
	if(SavePngRgba8(~path, mismatch, &error) || IsNull(error))
		validation_ok = false;
	error.Clear();
	if(LoadPngRgba8("", invalid_image, &error) || IsNull(error))
		validation_ok = false;
	error.Clear();
	if(LoadPngRgba8(~missing_path, invalid_image, &error) || IsNull(error))
		validation_ok = false;
	error.Clear();
	if(LoadPngRgba8(~bad_path, invalid_image, &error) || IsNull(error))
		validation_ok = false;
	printf("PNG IO validation failures: %s\n", validation_ok ? "OK" : "FAIL");
	if(validation_ok)
		++passed;
	else
		++failed;

	FileIn in(~path);
	printf("PNG IO file_size=%lld bytes\n", in.IsOpen() ? (long long)in.GetSize() : -1LL);
	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
