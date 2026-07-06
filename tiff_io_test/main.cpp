#include <stdio.h>

#include <imaging_roundtrip_test_support/ImagingRoundtripTest.h>
#include <libtiff/tiffio.h>
#include <tiff_io/TiffIO.h>

using namespace Upp;

static TiffRgbaImage8 ToTiffRgbaImage8(const TestImage8& src)
{
	TiffRgbaImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
	return out;
}

static TiffRgbaImage16 ToTiffRgbaImage16(const TestImage16& src)
{
	TiffRgbaImage16 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
	return out;
}

static TiffRgbaImageF ToTiffRgbaImageF(const TestImageF& src)
{
	TiffRgbaImageF out;
	out.width = src.width;
	out.height = src.height;
	if(src.width <= 0 || src.height <= 0 || src.pixels.GetCount() != (int64)src.width * src.height)
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
	return out;
}

static TestImage8 ToTestImage8(const TiffRgbaImage8& src)
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

static TestImage16 ToTestImage16(const TiffRgbaImage16& src)
{
	TestImage16 out;
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

static TestImageF ToTestImageF(const TiffRgbaImageF& src)
{
	TestImageF out;
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

struct TiffMessageState
{
	String* error = NULL;
};

static String TiffFormatMessage(const char* module, const char* fmt, va_list ap)
{
	char buffer[1024];
	buffer[0] = 0;
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	if(module && *module)
		return Format("%s: %s", module, buffer);
	return buffer;
}

static int TiffErrorHandler(TIFF*, void* user_data, const char* module, const char* fmt, va_list ap)
{
	TiffMessageState* state = (TiffMessageState*)user_data;
	if(state && state->error)
		*state->error = TiffFormatMessage(module, fmt, ap);
	return 1;
}

static int TiffWarningHandler(TIFF*, void* user_data, const char* module, const char* fmt, va_list ap)
{
	TiffMessageState* state = (TiffMessageState*)user_data;
	if(state && state->error && IsNull(*state->error))
		*state->error = TiffFormatMessage(module, fmt, ap);
	return 1;
}

static TIFF* OpenTiff(const char* path, const char* mode, TiffMessageState& state)
{
	TIFFOpenOptions* opts = TIFFOpenOptionsAlloc();
	if(!opts)
		return NULL;
	TIFFOpenOptionsSetErrorHandlerExtR(opts, TiffErrorHandler, &state);
	TIFFOpenOptionsSetWarningHandlerExtR(opts, TiffWarningHandler, &state);
	TIFF* tif = TIFFOpenExt(path, mode, opts);
	TIFFOpenOptionsFree(opts);
	return tif;
}

static bool ValidateTiffTags(const char* path, int width, int height, uint16_t bits, uint16_t sample_format, uint16_t compression)
{
	String error;
	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(path, "r", state);
	if(!tif)
		return false;
	uint32_t w = 0;
	uint32_t h = 0;
	uint16_t spp = 0;
	uint16_t bps = 0;
	uint16_t sf = 0;
	uint16_t photometric = 0;
	uint16_t planar = 0;
	uint16_t orient = 0;
	uint16_t comp = 0;
	uint32_t rps = 0;
	uint16_t extra_count = 0;
	uint16_t* extra_types = NULL;
	bool ok = TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w)
		&& TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h)
		&& TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp)
		&& TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps)
		&& TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sf)
		&& TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric)
		&& TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar)
		&& TIFFGetField(tif, TIFFTAG_ORIENTATION, &orient)
		&& TIFFGetField(tif, TIFFTAG_COMPRESSION, &comp)
		&& TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rps)
		&& TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &extra_count, &extra_types)
		&& !TIFFIsTiled(tif)
		&& TIFFNumberOfDirectories(tif) == 1
		&& w == (uint32_t)width
		&& h == (uint32_t)height
		&& spp == 4
		&& bps == bits
		&& sf == sample_format
		&& photometric == PHOTOMETRIC_RGB
		&& planar == PLANARCONFIG_CONTIG
		&& orient == ORIENTATION_TOPLEFT
		&& comp == compression
		&& rps > 0
		&& extra_count == 1
		&& extra_types != NULL
		&& extra_types[0] == EXTRASAMPLE_UNASSALPHA;
	TIFFClose(tif);
	return ok;
}

static bool WriteClassicRgba8(const char* path, const TiffRgbaImage8& image, TiffCompression compression)
{
	String error;
	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(path, "w", state);
	if(!tif)
		return false;
	uint16_t extra = EXTRASAMPLE_UNASSALPHA;
	bool ok = TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32_t)image.width)
		&& TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32_t)image.height)
		&& TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)4)
		&& TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)8)
		&& TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, (uint16_t)SAMPLEFORMAT_UINT)
		&& TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_RGB)
		&& TIFFSetField(tif, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_CONTIG)
		&& TIFFSetField(tif, TIFFTAG_ORIENTATION, (uint16_t)ORIENTATION_TOPLEFT)
		&& TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, (uint16_t)1, &extra)
		&& TIFFSetField(tif, TIFFTAG_COMPRESSION, (uint16_t)(compression == TiffCompression::None ? COMPRESSION_NONE : compression == TiffCompression::Lzw ? COMPRESSION_LZW : COMPRESSION_ADOBE_DEFLATE))
		&& TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (uint32_t)1);
	for(int y = 0; ok && y < image.height; ++y)
		ok = TIFFWriteScanline(tif, (void*)(image.pixels.Begin() + (size_t)y * (size_t)image.width), y, 0) >= 0;
	TIFFClose(tif);
	return ok && !state.error->GetCount() ? true : ok;
}

static bool WriteTiledRgba8(const char* path)
{
	String error;
	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(path, "w", state);
	if(!tif)
		return false;
	uint16_t extra = EXTRASAMPLE_UNASSALPHA;
	TiffRgba8 pixel = {1, 2, 3, 4};
	bool ok = TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32_t)2)
		&& TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32_t)2)
		&& TIFFSetField(tif, TIFFTAG_TILEWIDTH, (uint32_t)2)
		&& TIFFSetField(tif, TIFFTAG_TILELENGTH, (uint32_t)2)
		&& TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)4)
		&& TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)8)
		&& TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, (uint16_t)SAMPLEFORMAT_UINT)
		&& TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_RGB)
		&& TIFFSetField(tif, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_CONTIG)
		&& TIFFSetField(tif, TIFFTAG_ORIENTATION, (uint16_t)ORIENTATION_TOPLEFT)
		&& TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, (uint16_t)1, &extra)
		&& TIFFSetField(tif, TIFFTAG_COMPRESSION, (uint16_t)COMPRESSION_NONE);
	if(ok) {
		TiffRgba8 tile[4] = {pixel, pixel, pixel, pixel};
		ok = TIFFWriteEncodedTile(tif, 0, (void*)tile, sizeof(tile)) >= 0;
	}
	TIFFClose(tif);
	return ok;
}

static bool WriteTwoDirectoryRgba8(const char* path)
{
	String error;
	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(path, "w", state);
	if(!tif)
		return false;
	TiffRgba8 pixel = {5, 6, 7, 8};
	uint16_t extra = EXTRASAMPLE_UNASSALPHA;
	auto write_dir = [&]() {
		return TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32_t)1)
			&& TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32_t)1)
			&& TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)4)
			&& TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)8)
			&& TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, (uint16_t)SAMPLEFORMAT_UINT)
			&& TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_RGB)
			&& TIFFSetField(tif, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_CONTIG)
			&& TIFFSetField(tif, TIFFTAG_ORIENTATION, (uint16_t)ORIENTATION_TOPLEFT)
			&& TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, (uint16_t)1, &extra)
			&& TIFFSetField(tif, TIFFTAG_COMPRESSION, (uint16_t)COMPRESSION_NONE)
			&& TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (uint32_t)1)
			&& TIFFWriteScanline(tif, (void*)&pixel, 0, 0) >= 0;
	};
	bool ok = write_dir();
	if(ok)
		ok = TIFFWriteDirectory(tif);
	if(ok)
		ok = write_dir();
	TIFFClose(tif);
	return ok;
}

static bool WritePlanarSeparateRgba8(const char* path)
{
	String error;
	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(path, "w", state);
	if(!tif)
		return false;
	uint16_t extra = EXTRASAMPLE_UNASSALPHA;
	byte r = 11, g = 22, b = 33, a = 44;
	bool ok = TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32_t)1)
		&& TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32_t)1)
		&& TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)4)
		&& TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)8)
		&& TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, (uint16_t)SAMPLEFORMAT_UINT)
		&& TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_RGB)
		&& TIFFSetField(tif, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_SEPARATE)
		&& TIFFSetField(tif, TIFFTAG_ORIENTATION, (uint16_t)ORIENTATION_TOPLEFT)
		&& TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, (uint16_t)1, &extra)
		&& TIFFSetField(tif, TIFFTAG_COMPRESSION, (uint16_t)COMPRESSION_NONE)
		&& TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (uint32_t)1);
	if(ok) {
		ok = TIFFWriteScanline(tif, &r, 0, 0) >= 0;
		ok = ok && TIFFWriteScanline(tif, &g, 0, 1) >= 0;
		ok = ok && TIFFWriteScanline(tif, &b, 0, 2) >= 0;
		ok = ok && TIFFWriteScanline(tif, &a, 0, 3) >= 0;
	}
	TIFFClose(tif);
	return ok;
}

static String LoadBytes(const char* path)
{
	return LoadFile(path);
}

static bool WriteTruncatedValidFile(const char* path, const String& valid_data)
{
	FileOut out(path);
	if(!out.IsOpen())
		return false;
	if(valid_data.GetCount() <= 16)
		return false;
	out.Put(valid_data.Begin(), valid_data.GetCount() - 16);
	return true;
}

static bool WriteMalformedHeaderFile(const char* path)
{
	FileOut out(path);
	if(!out.IsOpen())
		return false;
	const byte junk[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
	out.Put(junk, sizeof(junk));
	return true;
}

static bool RunProfileStage(const char* save_label, const char* compare_label, const String& path, const String& compare_summary, const String& file_size, int& passed, int& failed)
{
	printf("%s: OK\n", save_label);
	++passed;
	printf("%s\n", ~compare_summary);
	printf("%s: OK\n", compare_label);
	++passed;
	printf("%s\n", ~file_size);
	return true;
}

static bool CheckLoadFailure(const char* label, const char* path, int& passed, int& failed)
{
	TiffRgbaImage8 image;
	String error;
	bool ok = !LoadTiffRgba8(path, image, &error) && !image.IsValid() && !IsNull(error);
	printf("%s: %s\n", label, ok ? "OK" : "FAIL");
	if(ok)
		++passed;
	else
		++failed;
	return ok;
}

static bool CheckLoadFailure16(const char* label, const char* path, int& passed, int& failed)
{
	TiffRgbaImage16 image;
	String error;
	bool ok = !LoadTiffRgba16(path, image, &error) && !image.IsValid() && !IsNull(error);
	printf("%s: %s\n", label, ok ? "OK" : "FAIL");
	if(ok)
		++passed;
	else
		++failed;
	return ok;
}

static bool CheckLoadFailureF(const char* label, const char* path, int& passed, int& failed)
{
	TiffRgbaImageF image;
	String error;
	bool ok = !LoadTiffRgbaF(path, image, &error) && !image.IsValid() && !IsNull(error);
	printf("%s: %s\n", label, ok ? "OK" : "FAIL");
	if(ok)
		++passed;
	else
		++failed;
	return ok;
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	const String rgba8_path = GetExeDirFile("tiff_io_rgba8_deflate.tif");
	const String rgba16_path = GetExeDirFile("tiff_io_rgba16_deflate.tif");
	const String float_path = GetExeDirFile("tiff_io_float_none.tif");
	const String malformed_path = GetExeDirFile("tiff_io_malformed.tif");
	const String truncated_path = GetExeDirFile("tiff_io_truncated.tif");
	const String tiled_path = GetExeDirFile("tiff_io_tiled.tif");
	const String multipage_path = GetExeDirFile("tiff_io_multipage.tif");
	const String separate_path = GetExeDirFile("tiff_io_separate.tif");

	FileDelete(rgba8_path);
	FileDelete(rgba16_path);
	FileDelete(float_path);
	FileDelete(malformed_path);
	FileDelete(truncated_path);
	FileDelete(tiled_path);
	FileDelete(multipage_path);
	FileDelete(separate_path);

	// RGBA8 + Deflate
	TestImageF src8 = GenerateRoundtripTestPattern(256, 192, false);
	TestImage8 expected8 = QuantizeToRgba8(src8);
	TiffRgbaImage8 tiff8 = ToTiffRgbaImage8(expected8);
	TiffSaveOptions deflate_options;
	deflate_options.compression = TiffCompression::Deflate;
	String error;
	if(!SaveTiffRgba8(~rgba8_path, tiff8, deflate_options, &error)) {
		printf("TIFF IO RGBA8 Deflate: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	String rgba8_data = LoadBytes(rgba8_path);
	printf("TIFF IO RGBA8 Deflate: OK\n");
	++passed;
	printf("TIFF IO RGBA8 file_size=%lld bytes\n", (long long)rgba8_data.GetCount());
	printf("TIFF IO RGBA8 tags: %s\n", ValidateTiffTags(~rgba8_path, 256, 192, 8, SAMPLEFORMAT_UINT, COMPRESSION_ADOBE_DEFLATE) ? "OK" : "FAIL");
	if(!ValidateTiffTags(~rgba8_path, 256, 192, 8, SAMPLEFORMAT_UINT, COMPRESSION_ADOBE_DEFLATE)) {
		++failed;
	} else {
		++passed;
	}
	TiffRgbaImage8 loaded8;
	if(!LoadTiffRgba8(~rgba8_path, loaded8, &error)) {
		printf("TIFF IO RGBA8 load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	TestImage8 actual8 = ToTestImage8(loaded8);
	RoundtripComparison8 cmp8 = CompareExact(expected8, actual8);
	printf("TIFF IO RGBA8 exact comparison: %s\n", cmp8.different_components == 0 && cmp8.dimensions_match ? "OK" : "FAIL");
	printf("TIFF IO RGBA8 exact metrics: different_components=%d max_error_r=%d max_error_g=%d max_error_b=%d max_error_a=%d\n",
		cmp8.different_components, cmp8.max_error_r, cmp8.max_error_g, cmp8.max_error_b, cmp8.max_error_a);
	if(cmp8.different_components == 0 && cmp8.dimensions_match)
		++passed;
	else
		++failed;

	// RGBA16 + Deflate
	TestImageF src16 = GenerateRoundtripTestPattern(257, 193, false);
	TestImage16 expected16 = QuantizeToRgba16(src16);
	TiffRgbaImage16 tiff16 = ToTiffRgbaImage16(expected16);
	if(!SaveTiffRgba16(~rgba16_path, tiff16, deflate_options, &error)) {
		printf("TIFF IO RGBA16 Deflate: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	String rgba16_data = LoadBytes(rgba16_path);
	printf("TIFF IO RGBA16 Deflate: OK\n");
	++passed;
	printf("TIFF IO RGBA16 file_size=%lld bytes\n", (long long)rgba16_data.GetCount());
	printf("TIFF IO RGBA16 tags: %s\n", ValidateTiffTags(~rgba16_path, 257, 193, 16, SAMPLEFORMAT_UINT, COMPRESSION_ADOBE_DEFLATE) ? "OK" : "FAIL");
	if(ValidateTiffTags(~rgba16_path, 257, 193, 16, SAMPLEFORMAT_UINT, COMPRESSION_ADOBE_DEFLATE))
		++passed;
	else
		++failed;
	TiffRgbaImage16 loaded16;
	if(!LoadTiffRgba16(~rgba16_path, loaded16, &error)) {
		printf("TIFF IO RGBA16 load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	TestImage16 actual16 = ToTestImage16(loaded16);
	RoundtripComparison16 cmp16 = CompareExact(expected16, actual16);
	printf("TIFF IO RGBA16 exact comparison: %s\n", cmp16.different_components == 0 && cmp16.dimensions_match ? "OK" : "FAIL");
	printf("TIFF IO RGBA16 exact metrics: different_components=%d max_error_r=%d max_error_g=%d max_error_b=%d max_error_a=%d\n",
		cmp16.different_components, cmp16.max_error_r, cmp16.max_error_g, cmp16.max_error_b, cmp16.max_error_a);
	if(cmp16.different_components == 0 && cmp16.dimensions_match)
		++passed;
	else
		++failed;

	// Float32 + NONE
	TestImageF expectedf = GenerateRoundtripTestPattern(255, 191, true);
	TiffRgbaImageF tiffF = ToTiffRgbaImageF(expectedf);
	TiffSaveOptions none_options;
	none_options.compression = TiffCompression::None;
	if(!SaveTiffRgbaF(~float_path, tiffF, none_options, &error)) {
		printf("TIFF IO Float32 NONE: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	String float_data = LoadBytes(float_path);
	printf("TIFF IO Float32 NONE: OK\n");
	++passed;
	printf("TIFF IO Float32 file_size=%lld bytes\n", (long long)float_data.GetCount());
	printf("TIFF IO Float32 tags: %s\n", ValidateTiffTags(~float_path, 255, 191, 32, SAMPLEFORMAT_IEEEFP, COMPRESSION_NONE) ? "OK" : "FAIL");
	if(ValidateTiffTags(~float_path, 255, 191, 32, SAMPLEFORMAT_IEEEFP, COMPRESSION_NONE))
		++passed;
	else
		++failed;
	TiffRgbaImageF loadedf;
	if(!LoadTiffRgbaF(~float_path, loadedf, &error)) {
		printf("TIFF IO Float32 load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		++failed;
		SetExitCode(1);
		return;
	}
	TestImageF actualf = ToTestImageF(loadedf);
	RoundtripComparison cmpf = CompareExact(expectedf, actualf);
	printf("TIFF IO Float32 exact comparison: %s\n", cmpf.different_components == 0 && cmpf.dimensions_match ? "OK" : "FAIL");
	printf("TIFF IO Float32 exact metrics: different_components=%d max_error_r=%.9g max_error_g=%.9g max_error_b=%.9g max_error_a=%.9g mean_absolute_error=%.9g rmse=%.9g\n",
		cmpf.different_components, cmpf.max_error_r, cmpf.max_error_g, cmpf.max_error_b, cmpf.max_error_a, cmpf.mean_absolute_error, cmpf.rmse);
	if(cmpf.different_components == 0 && cmpf.dimensions_match)
		++passed;
	else
		++failed;

	// Validation failures
	bool validation_ok = true;
	TiffRgbaImage8 invalid8;
	if(SaveTiffRgba8("", tiff8, deflate_options, &error) || IsNull(error)) validation_ok = false;
	error.Clear();
	TiffRgbaImage8 zero8;
	if(SaveTiffRgba8(~rgba8_path, zero8, deflate_options, &error) || IsNull(error)) validation_ok = false;
	error.Clear();
	TiffRgbaImage8 mismatch8;
	mismatch8.width = tiff8.width;
	mismatch8.height = tiff8.height;
	mismatch8.pixels.SetCount(tiff8.pixels.GetCount() - 1);
	if(SaveTiffRgba8(~rgba8_path, mismatch8, deflate_options, &error) || IsNull(error)) validation_ok = false;
	error.Clear();
	if(LoadTiffRgba8("", invalid8, &error) || IsNull(error) || invalid8.IsValid()) validation_ok = false;
	error.Clear();
	if(LoadTiffRgba8("does_not_exist.tif", invalid8, &error) || IsNull(error) || invalid8.IsValid()) validation_ok = false;
	error.Clear();
	WriteMalformedHeaderFile(~malformed_path);
	if(LoadTiffRgba8(~malformed_path, invalid8, &error) || IsNull(error) || invalid8.IsValid()) validation_ok = false;
	error.Clear();
	WriteTruncatedValidFile(~truncated_path, rgba8_data);
	if(LoadTiffRgba8(~truncated_path, invalid8, &error) || IsNull(error) || invalid8.IsValid()) validation_ok = false;
	error.Clear();
	WriteTiledRgba8(~tiled_path);
	if(LoadTiffRgba8(~tiled_path, invalid8, &error) || IsNull(error) || invalid8.IsValid()) validation_ok = false;
	error.Clear();
	WriteTwoDirectoryRgba8(~multipage_path);
	if(LoadTiffRgba8(~multipage_path, invalid8, &error) || IsNull(error) || invalid8.IsValid()) validation_ok = false;
	error.Clear();
	WritePlanarSeparateRgba8(~separate_path);
	if(LoadTiffRgba8(~separate_path, invalid8, &error) || IsNull(error) || invalid8.IsValid()) validation_ok = false;

	printf("TIFF IO validation failures: %s\n", validation_ok ? "OK" : "FAIL");
	if(validation_ok)
		++passed;
	else
		++failed;

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
