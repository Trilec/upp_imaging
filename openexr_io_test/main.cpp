#include <stdio.h>

#include <imaging_roundtrip_test_support/ImagingRoundtripTest.h>
#include <openexr_io/OpenExrIO.h>

using namespace Upp;

static ExrRgbaImageF ToExrRgbaImageF(const TestImageF& src)
{
	ExrRgbaImageF out;
	out.width = src.width;
	out.height = src.height;
	if(src.width <= 0 || src.height <= 0)
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

static TestImageF ToTestImageF(const ExrRgbaImageF& src)
{
	TestImageF out;
	out.width = src.width;
	out.height = src.height;
	if(src.width <= 0 || src.height <= 0)
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

static void PrintMetrics(const char* label, int width, int height, const RoundtripComparison& cmp)
{
	printf("%s dimensions=%dx%d different_components=%d max_error_r=%.9g max_error_g=%.9g max_error_b=%.9g max_error_a=%.9g mean_absolute_error=%.9g rmse=%.9g\n",
		label,
		width,
		height,
		cmp.different_components,
		cmp.max_error_r,
		cmp.max_error_g,
		cmp.max_error_b,
		cmp.max_error_a,
		cmp.mean_absolute_error,
		cmp.rmse);
}

static bool PrintChartStage(const char* label, int width, int height, bool include_hdr, TestImageF& image)
{
	image = GenerateRoundtripTestPattern(width, height, include_hdr);
	printf("%s chart: OK\n", label);
	return true;
}

static bool PrintComparisonStage(const char* label, const TestImageF& expected, const TestImageF& actual, int& passed, int& failed)
{
	RoundtripComparison cmp = CompareExact(expected, actual);
	PrintMetrics(label, expected.width, expected.height, cmp);
	if(cmp.dimensions_match && cmp.different_components == 0 && cmp.max_error_r == 0.0 && cmp.max_error_g == 0.0 && cmp.max_error_b == 0.0 && cmp.max_error_a == 0.0 && cmp.mean_absolute_error == 0.0 && cmp.rmse == 0.0) {
		printf("%s comparison: OK\n", label);
		++passed;
		return true;
	}
	printf("%s comparison: FAIL\n", label);
	++failed;
	SetExitCode(1);
	return false;
}

static void PrintFileSize(const char* label, const char* path)
{
	FileIn in(path);
	long long size = in.IsOpen() ? (long long)in.GetSize() : -1;
	printf("%s file_size=%lld bytes\n", label, size);
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;
	String error;

	const char* half_path = "E:/apps/github/upp_imaging/out/openexr_io_half_zip.exr";
	const char* float_path = "E:/apps/github/upp_imaging/out/openexr_io_float.exr";

	TestImageF half_src;
	if(!PrintChartStage("OpenEXR IO HALF/ZIP", 256, 192, false, half_src))
		return;
	++passed;
	ExrRgbaImageF half_file = ToExrRgbaImageF(half_src);
	if(!SaveExrRgbaF(half_path, half_file, true, true, &error)) {
		printf("OpenEXR IO HALF/ZIP save: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("OpenEXR IO HALF/ZIP save: OK\n");
	++passed;
	ExrRgbaImageF half_loaded;
	error.Clear();
	if(!LoadExrRgbaF(half_path, half_loaded, &error)) {
		printf("OpenEXR IO HALF/ZIP load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("OpenEXR IO HALF/ZIP load: OK\n");
	++passed;
	TestImageF half_dst = ToTestImageF(half_loaded);
	if(!PrintComparisonStage("OpenEXR IO HALF/ZIP", half_src, half_dst, passed, failed))
		return;
	PrintFileSize("OpenEXR IO HALF/ZIP", half_path);

	TestImageF float_src;
	if(!PrintChartStage("OpenEXR IO FLOAT", 256, 192, true, float_src))
		return;
	++passed;
	ExrRgbaImageF float_file = ToExrRgbaImageF(float_src);
	error.Clear();
	if(!SaveExrRgbaF(float_path, float_file, false, false, &error)) {
		printf("OpenEXR IO FLOAT save: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("OpenEXR IO FLOAT save: OK\n");
	++passed;
	ExrRgbaImageF float_loaded;
	error.Clear();
	if(!LoadExrRgbaF(float_path, float_loaded, &error)) {
		printf("OpenEXR IO FLOAT load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("OpenEXR IO FLOAT load: OK\n");
	++passed;
	TestImageF float_dst = ToTestImageF(float_loaded);
	if(!PrintComparisonStage("OpenEXR IO FLOAT", float_src, float_dst, passed, failed))
		return;
	PrintFileSize("OpenEXR IO FLOAT", float_path);

	ExrRgbaImageF invalid;
	invalid.width = 3;
	invalid.height = 2;
	invalid.pixels.SetCount(5);
	error.Clear();
	if(SaveExrRgbaF("E:/apps/github/upp_imaging/out/openexr_io_invalid.exr", invalid, true, true, &error)) {
		printf("OpenEXR IO validation failures: FAIL\n");
		++failed;
	}
	else if(IsNull(error)) {
		printf("OpenEXR IO validation failures: FAIL\n");
		++failed;
	}
	else {
		printf("OpenEXR IO validation failures: OK\n");
		++passed;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
