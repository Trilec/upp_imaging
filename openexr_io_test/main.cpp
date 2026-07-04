#include <stdio.h>

#include <imaging_roundtrip_test_support/ImagingRoundtripTest.h>
#include <openexr_io/OpenExrIO.h>

using namespace Upp;

static int PrintFail(const char* label, const String& detail, int& failed)
{
	if(IsNull(detail))
		printf("%s: FAIL\n", label);
	else
		printf("%s: FAIL (%s)\n", label, ~detail);
	++failed;
	return 1;
}

static int PrintCmp(const char* label, const RoundtripComparison& cmp, int& failed, int& passed)
{
	if(cmp.different_components == 0 && cmp.dimensions_match) {
		printf("%s: OK\n", label);
		++passed;
		return 0;
	}
	printf("%s: FAIL (%s)\n", label, ~cmp.summary);
	++failed;
	return 1;
}

static bool SaveThenLoad(const char* path, const TestImageF& src, bool output_half, bool use_zip, TestImageF& dst, String& error)
{
	ExrRgbaImageF image;
	image.width = src.width;
	image.height = src.height;
	image.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		image.pixels[i].r = src.pixels[i].r;
		image.pixels[i].g = src.pixels[i].g;
		image.pixels[i].b = src.pixels[i].b;
		image.pixels[i].a = src.pixels[i].a;
	}
	if(!SaveExrRgbaF(path, image, output_half, use_zip, &error))
		return false;
	ExrRgbaImageF loaded;
	if(!LoadExrRgbaF(path, loaded, &error))
		return false;
	dst.width = loaded.width;
	dst.height = loaded.height;
	dst.pixels.SetCount(loaded.pixels.GetCount());
	for(int i = 0; i < loaded.pixels.GetCount(); ++i) {
		dst.pixels[i].r = loaded.pixels[i].r;
		dst.pixels[i].g = loaded.pixels[i].g;
		dst.pixels[i].b = loaded.pixels[i].b;
		dst.pixels[i].a = loaded.pixels[i].a;
	}
	return true;
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	String error;
	const char* half_path = "E:/apps/github/upp_imaging/out/openexr_io_half_zip.exr";
	const char* float_path = "E:/apps/github/upp_imaging/out/openexr_io_float.exr";

	TestImageF half_src = GenerateRoundtripTestPattern(256, 192, false);
	TestImageF half_dst;
	if(!SaveThenLoad(half_path, half_src, true, true, half_dst, error))
	{
		PrintFail("OpenEXR IO half ZIP round-trip", error, failed);
		SetExitCode(1);
		return;
	}
	PrintCmp("OpenEXR IO half ZIP round-trip", CompareExact(half_src, half_dst), failed, passed);

	TestImageF float_src = GenerateRoundtripTestPattern(256, 192, true);
	TestImageF float_dst;
	error.Clear();
	if(!SaveThenLoad(float_path, float_src, false, false, float_dst, error))
	{
		PrintFail("OpenEXR IO float round-trip", error, failed);
		SetExitCode(1);
		return;
	}
	PrintCmp("OpenEXR IO float round-trip", CompareExact(float_src, float_dst), failed, passed);

	ExrRgbaImageF invalid;
	invalid.width = 3;
	invalid.height = 2;
	invalid.pixels.SetCount(5);
	error.Clear();
	if(SaveExrRgbaF("E:/apps/github/upp_imaging/out/openexr_io_invalid.exr", invalid, true, true, &error)) {
		printf("OpenEXR IO validation error: FAIL\n");
		++failed;
	}
	else if(IsNull(error)) {
		printf("OpenEXR IO validation error: FAIL\n");
		++failed;
	}
	else {
		printf("OpenEXR IO validation error: OK (%s)\n", ~error);
		++passed;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
