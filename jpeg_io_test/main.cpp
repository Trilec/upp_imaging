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

static bool MeetsPolicy(const LossyRgbComparison& cmp)
{
	return cmp.dimensions_match && cmp.mean_absolute_error <= 3.5 && cmp.rmse <= 6.5 && cmp.psnr >= 32.0;
}

static bool EndsWithEoi(const String& data)
{
	return data.GetCount() >= 2 && (byte)data[data.GetCount() - 2] == 0xff && (byte)data[data.GetCount() - 1] == 0xd9;
}

static void WriteMalformedHeaderFile(const char* path)
{
	FileOut out(path);
	if(out.IsOpen()) {
		const byte junk[] = { 0xff, 0xd8, 0xff, 0x00, 0x00, 0x00 };
		out.Put(junk, sizeof(junk));
	}
}

static void WriteTruncatedValidFile(const char* path, const String& valid_data)
{
	FileOut out(path);
	if(out.IsOpen() && valid_data.GetCount() >= 2)
		out.Put(valid_data.Begin(), valid_data.GetCount() - 2);
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;
	String error;

	const String path = GetExeDirFile("jpeg_io_roundtrip.jpg");
	const String progressive_path = GetExeDirFile("jpeg_io_roundtrip_progressive.jpg");
	const String bad_path = GetExeDirFile("jpeg_io_truncated.jpg");
	const String truncated_path = GetExeDirFile("jpeg_io_truncated_valid.jpg");
	const String missing_path = GetExeDirFile("jpeg_io_missing_input.jpg");
	FileDelete(missing_path);
	FileDelete(progressive_path);
	FileDelete(truncated_path);

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
	String valid_data = LoadFile(path);
	printf("JPEG IO EOI marker: %s\n", EndsWithEoi(valid_data) ? "OK" : "FAIL");
	if(EndsWithEoi(valid_data))
		++passed;
	else
		++failed;

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
	printf("JPEG IO metrics: max_error_r=%d max_error_g=%d max_error_b=%d MAE=%.9g RMSE=%.9g PSNR=%.9g dB\n",
		cmp.max_error_r, cmp.max_error_g, cmp.max_error_b, cmp.mean_absolute_error, cmp.rmse, cmp.psnr);
	if(cmp.dimensions_match && cmp.mean_absolute_error <= 3.5 && cmp.rmse <= 6.5 && cmp.psnr >= 32.0) {
		printf("JPEG IO quality policy: OK\n");
		++passed;
	}
	else {
		printf("JPEG IO quality policy: FAIL\n");
		++failed;
	}

	JpegSaveOptions progressive_options = options;
	progressive_options.progressive = true;
	if(!SaveJpegRgb8(~progressive_path, save_image, progressive_options, &error)) {
		printf("JPEG IO progressive save: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("JPEG IO progressive save: OK\n");
	++passed;

	JpegRgbImage8 progressive_loaded;
	error.Clear();
	if(!LoadJpegRgb8(~progressive_path, progressive_loaded, &error)) {
		printf("JPEG IO progressive load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	printf("JPEG IO progressive load: OK\n");
	++passed;
	TestRgbImage8 progressive_actual = ToTestRgbImage8(progressive_loaded);
	LossyRgbComparison progressive_cmp = CompareLossyRgb8(expected, progressive_actual);
	printf("JPEG IO progressive dimensions: %s\n", progressive_cmp.dimensions_match ? "OK" : "FAIL");
	if(progressive_cmp.dimensions_match)
		++passed;
	else
		++failed;
	printf("JPEG IO progressive metrics: max_error_r=%d max_error_g=%d max_error_b=%d MAE=%.9g RMSE=%.9g PSNR=%.9g dB\n",
		progressive_cmp.max_error_r, progressive_cmp.max_error_g, progressive_cmp.max_error_b, progressive_cmp.mean_absolute_error, progressive_cmp.rmse, progressive_cmp.psnr);
	printf("JPEG IO progressive policy: %s\n", MeetsPolicy(progressive_cmp) ? "OK" : "FAIL");
	if(MeetsPolicy(progressive_cmp))
		++passed;
	else
		++failed;

	String progressive_file_data = LoadFile(progressive_path);
	printf("JPEG IO progressive file_size=%lld bytes\n", (long long)progressive_file_data.GetCount());

	WriteMalformedHeaderFile(~bad_path);
	WriteTruncatedValidFile(~truncated_path, valid_data);
	bool malformed_header_ok = true;
	bool truncated_valid_ok = true;

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
	if(LoadJpegRgb8(~bad_path, invalid, &error) || IsNull(error) || invalid.IsValid()) malformed_header_ok = false;
	printf("JPEG IO malformed header rejection: %s\n", malformed_header_ok ? "OK" : "FAIL");
	if(malformed_header_ok)
		++passed;
	else
		++failed;
	error.Clear();
	if(LoadJpegRgb8(~truncated_path, invalid, &error) || IsNull(error) || invalid.IsValid()) truncated_valid_ok = false;
	printf("JPEG IO truncated valid file rejection: %s\n", truncated_valid_ok ? "OK" : "FAIL");
	if(truncated_valid_ok)
		++passed;
	else
		++failed;
	error.Clear();
	if(!EndsWithEoi(valid_data) || !EndsWithEoi(progressive_file_data)) validation_ok = false;
	printf("JPEG IO validation failures: %s\n", validation_ok ? "OK" : "FAIL");
	if(validation_ok)
		++passed;
	else
		++failed;

	FileIn in(~path);
	printf("JPEG IO file_size=%lld bytes\n", in.IsOpen() ? (long long)in.GetSize() : -1LL);
	FileIn progressive_in(~progressive_path);
	printf("JPEG IO progressive_file_size=%lld bytes\n", progressive_in.IsOpen() ? (long long)progressive_in.GetSize() : -1LL);
	printf("JPEG IO baseline metrics: max_error_r=%d max_error_g=%d max_error_b=%d MAE=%.9g RMSE=%.9g PSNR=%.9g dB\n",
		cmp.max_error_r, cmp.max_error_g, cmp.max_error_b, cmp.mean_absolute_error, cmp.rmse, cmp.psnr);
	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
