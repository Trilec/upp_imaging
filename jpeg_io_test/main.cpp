#include <stdio.h>

#include <imaging_roundtrip_test_support/ImagingRoundtripTest.h>
#include <jpeg_io/JpegIO.h>

using namespace Upp;

static JpegRgbImage8 ToJpegRgbImage8(const TestRgbImage8& src)
{
	JpegRgbImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
	}
	return out;
}

static TestRgbImage8 ToTestRgbImage8(const JpegRgbImage8& src)
{
	TestRgbImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
	}
	return out;
}

static void WriteTruncatedFile(const char* path)
{
	FileOut out(path);
	if(out.IsOpen()) {
		const byte junk[] = { 0xff, 0xd8, 0xff, 0x00, 0x00, 0x00 };
		out.Put(junk, sizeof(junk));
	}
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;
	String error;

	const String path = GetExeDirFile("jpeg_io_roundtrip.jpg");
	const String bad_path = GetExeDirFile("jpeg_io_truncated.jpg");
	const String missing_path = GetExeDirFile("jpeg_io_missing_input.jpg");
	FileDelete(missing_path);

	TestImageF generated_float = GenerateRoundtripTestPattern(256, 192, false);
	TestImage8 quantized = QuantizeToRgba8(generated_float);
	TestRgbImage8 expected;
	expected.width = quantized.width;
	expected.height = quantized.height;
	expected.pixels.SetCount(quantized.pixels.GetCount());
	for(int i = 0; i < quantized.pixels.GetCount(); ++i) {
		expected.pixels[i].r = quantized.pixels[i].r;
		expected.pixels[i].g = quantized.pixels[i].g;
		expected.pixels[i].b = quantized.pixels[i].b;
	}
	printf("JPEG IO chart: OK\n");
	++passed;

	JpegRgbImage8 save_image = ToJpegRgbImage8(expected);
	JpegSaveOptions options;
	options.quality = 95;
	options.subsampling = JpegSubsampling::S444;
	options.progressive = false;
	options.optimize_coding = true;
	if(!SaveJpegRgb8(~path, save_image, options, &error)) {
		printf("JPEG IO save Q95 4:4:4: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("JPEG IO save Q95 4:4:4: OK\n");
	++passed;

	JpegRgbImage8 loaded_image;
	error.Clear();
	if(!LoadJpegRgb8(~path, loaded_image, &error)) {
		printf("JPEG IO load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("JPEG IO load: OK\n");
	++passed;

	TestRgbImage8 actual = ToTestRgbImage8(loaded_image);
	LossyRgbComparison cmp = CompareLossyRgb8(expected, actual);
	printf("JPEG IO dimensions: %s\n", cmp.dimensions_match ? "OK" : "FAIL");
	if(cmp.dimensions_match)
		++passed;
	else
		++failed;
	printf("JPEG IO metrics: MAE=%.9g RMSE=%.9g PSNR=%.9g dB\n", cmp.mean_absolute_error, cmp.rmse, cmp.psnr);
	if(cmp.dimensions_match && cmp.mean_absolute_error <= 3.5 && cmp.rmse <= 6.5 && cmp.psnr >= 32.0) {
		printf("JPEG IO quality policy: OK\n");
		++passed;
	}
	else {
		printf("JPEG IO quality policy: FAIL\n");
		++failed;
	}

	WriteTruncatedFile(~bad_path);

	bool validation_ok = true;
	JpegRgbImage8 invalid;
	JpegSaveOptions bad_options = options;
	if(SaveJpegRgb8("", save_image, options, &error) || IsNull(error)) validation_ok = false;
	error.Clear();
	JpegRgbImage8 zero;
	if(SaveJpegRgb8(~path, zero, options, &error) || IsNull(error)) validation_ok = false;
	error.Clear();
	JpegRgbImage8 mismatch;
	mismatch.width = save_image.width;
	mismatch.height = save_image.height;
	mismatch.pixels.SetCount(save_image.pixels.GetCount() - 1);
	if(SaveJpegRgb8(~path, mismatch, options, &error) || IsNull(error)) validation_ok = false;
	error.Clear();
	bad_options.quality = 0;
	if(SaveJpegRgb8(~path, save_image, bad_options, &error) || IsNull(error)) validation_ok = false;
	error.Clear();
	if(LoadJpegRgb8("", invalid, &error) || IsNull(error)) validation_ok = false;
	error.Clear();
	if(LoadJpegRgb8(~missing_path, invalid, &error) || IsNull(error)) validation_ok = false;
	error.Clear();
	if(LoadJpegRgb8(~bad_path, invalid, &error) || IsNull(error)) validation_ok = false;
	printf("JPEG IO validation failures: %s\n", validation_ok ? "OK" : "FAIL");
	if(validation_ok)
		++passed;
	else
		++failed;

	FileIn in(~path);
	printf("JPEG IO file_size=%lld bytes\n", in.IsOpen() ? (long long)in.GetSize() : -1LL);
	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
