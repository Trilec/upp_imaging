#include <stdio.h>
#include <stdint.h>

#include <imaging_roundtrip_test_support/ImagingRoundtripTest.h>
#include <libtiff/tiffio.h>
#include <tiff_io/TiffIO.h>

using namespace Upp;

struct TiffMessageState
{
	String* error = NULL;
};

static bool IsValidImage(const TestImageF& img)
{
	return img.width > 0 && img.height > 0 && img.pixels.GetCount() == (int64_t)img.width * img.height;
}

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
	if(!IsValidImage(src))
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
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
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
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
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
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
	return out;
}

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

static void Stage(const char* label, bool ok, int& passed, int& failed)
{
	printf("%s: %s\n", label, ok ? "OK" : "FAIL");
	if(ok)
		++passed;
	else
		++failed;
}

static bool ValidateRgbaTags(const char* path, int width, int height, uint16_t bits, uint16_t sample_format, uint16_t compression)
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

static bool ValidateTiledFixture(const char* path)
{
	String error;
	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(path, "r", state);
	if(!tif)
		return false;
	const bool ok = TIFFIsTiled(tif) != 0 && TIFFNumberOfDirectories(tif) == 1;
	TIFFClose(tif);
	return ok;
}

static bool ValidateMultiDirectoryFixture(const char* path)
{
	String error;
	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(path, "r", state);
	if(!tif)
		return false;
	const bool ok = TIFFNumberOfDirectories(tif) == 2;
	TIFFClose(tif);
	return ok;
}

static bool ValidatePlanarSeparateFixture(const char* path)
{
	String error;
	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(path, "r", state);
	if(!tif)
		return false;
	uint16_t planar = 0;
	bool ok = TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar) && planar == PLANARCONFIG_SEPARATE;
	TIFFClose(tif);
	return ok;
}

static bool WriteTiledRgba8(const char* path, const TiffRgbaImage8& image)
{
	String error;
	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(path, "w", state);
	if(!tif)
		return false;
	uint16_t extra = EXTRASAMPLE_UNASSALPHA;
	bool ok = TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32_t)16)
		&& TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32_t)16)
		&& TIFFSetField(tif, TIFFTAG_TILEWIDTH, (uint32_t)16)
		&& TIFFSetField(tif, TIFFTAG_TILELENGTH, (uint32_t)16)
		&& TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)4)
		&& TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)8)
		&& TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, (uint16_t)SAMPLEFORMAT_UINT)
		&& TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_RGB)
		&& TIFFSetField(tif, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_CONTIG)
		&& TIFFSetField(tif, TIFFTAG_ORIENTATION, (uint16_t)ORIENTATION_TOPLEFT)
		&& TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, (uint16_t)1, &extra)
		&& TIFFSetField(tif, TIFFTAG_COMPRESSION, (uint16_t)COMPRESSION_NONE);
	if(ok) {
		ok = TIFFWriteEncodedTile(tif, 0, (void*)image.pixels.Begin(), (tmsize_t)(image.pixels.GetCount() * sizeof(TiffRgba8))) >= 0;
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
	uint16_t extra = EXTRASAMPLE_UNASSALPHA;
	TiffRgba8 pixel = {5, 6, 7, 8};
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

static bool LoadFailure8(const char* path, const char* expected_error)
{
	TiffRgbaImage8 image;
	String error;
	const bool ok = !LoadTiffRgba8(path, image, &error) && !image.IsValid() && !IsNull(error);
	return ok && (expected_error == NULL || error == expected_error);
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;
	String error;

	const String rgba8_path = GetExeDirFile("tiff_io_rgba8_deflate.tif");
	const String rgba8_lzw_path = GetExeDirFile("tiff_io_rgba8_lzw.tif");
	const String rgba16_path = GetExeDirFile("tiff_io_rgba16_deflate.tif");
	const String float_path = GetExeDirFile("tiff_io_float_none.tif");
	const String malformed_path = GetExeDirFile("tiff_io_malformed.tif");
	const String truncated_path = GetExeDirFile("tiff_io_truncated.tif");
	const String tiled_path = GetExeDirFile("tiff_io_tiled.tif");
	const String multipage_path = GetExeDirFile("tiff_io_multipage.tif");
	const String separate_path = GetExeDirFile("tiff_io_separate.tif");

	FileDelete(rgba8_path);
	FileDelete(rgba8_lzw_path);
	FileDelete(rgba16_path);
	FileDelete(float_path);
	FileDelete(malformed_path);
	FileDelete(truncated_path);
	FileDelete(tiled_path);
	FileDelete(multipage_path);
	FileDelete(separate_path);

	TiffSaveOptions deflate_options;
	deflate_options.compression = TiffCompression::Deflate;
	TiffSaveOptions lzw_options;
	lzw_options.compression = TiffCompression::Lzw;
	TiffSaveOptions none_options;
	none_options.compression = TiffCompression::None;

	TestImageF src8 = GenerateRoundtripTestPattern(256, 192, false);
	TestImage8 expected8 = QuantizeToRgba8(src8);
	TiffRgbaImage8 tiff8 = ToTiffRgbaImage8(expected8);
	if(!SaveTiffRgba8(~rgba8_path, tiff8, deflate_options, &error)) {
		printf("TIFF IO RGBA8 Deflate: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		SetExitCode(1);
		return;
	}
	Stage("TIFF IO RGBA8 Deflate", true, passed, failed);
	String rgba8_data = LoadFile(rgba8_path);
	printf("TIFF IO RGBA8 file_size=%lld bytes\n", (long long)rgba8_data.GetCount());
	Stage("TIFF IO RGBA8 tags", ValidateRgbaTags(~rgba8_path, 256, 192, 8, SAMPLEFORMAT_UINT, COMPRESSION_ADOBE_DEFLATE), passed, failed);
	TiffRgbaImage8 loaded8;
	if(!LoadTiffRgba8(~rgba8_path, loaded8, &error)) {
		printf("TIFF IO RGBA8 load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		SetExitCode(1);
		return;
	}
	TestImage8 actual8 = ToTestImage8(loaded8);
	RoundtripComparison8 cmp8 = CompareExact(expected8, actual8);
	Stage("TIFF IO RGBA8 exact comparison", cmp8.different_components == 0 && cmp8.dimensions_match, passed, failed);
	printf("TIFF IO RGBA8 exact metrics: different_components=%d max_error_r=%d max_error_g=%d max_error_b=%d max_error_a=%d\n",
		cmp8.different_components, cmp8.max_error_r, cmp8.max_error_g, cmp8.max_error_b, cmp8.max_error_a);

	TestImageF src8_lzw = GenerateRoundtripTestPattern(129, 97, false);
	TestImage8 expected8_lzw = QuantizeToRgba8(src8_lzw);
	TiffRgbaImage8 tiff8_lzw = ToTiffRgbaImage8(expected8_lzw);
	if(!SaveTiffRgba8(~rgba8_lzw_path, tiff8_lzw, lzw_options, &error)) {
		printf("TIFF IO RGBA8 LZW: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		SetExitCode(1);
		return;
	}
	Stage("TIFF IO RGBA8 LZW", true, passed, failed);
	String rgba8_lzw_data = LoadFile(rgba8_lzw_path);
	printf("TIFF IO RGBA8 LZW file_size=%lld bytes\n", (long long)rgba8_lzw_data.GetCount());
	Stage("TIFF IO RGBA8 LZW tags", ValidateRgbaTags(~rgba8_lzw_path, 129, 97, 8, SAMPLEFORMAT_UINT, COMPRESSION_LZW), passed, failed);
	TiffRgbaImage8 loaded8_lzw;
	if(!LoadTiffRgba8(~rgba8_lzw_path, loaded8_lzw, &error)) {
		printf("TIFF IO RGBA8 LZW load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		SetExitCode(1);
		return;
	}
	TestImage8 actual8_lzw = ToTestImage8(loaded8_lzw);
	RoundtripComparison8 cmp8_lzw = CompareExact(expected8_lzw, actual8_lzw);
	Stage("TIFF IO RGBA8 LZW exact comparison", cmp8_lzw.different_components == 0 && cmp8_lzw.dimensions_match, passed, failed);
	printf("TIFF IO RGBA8 LZW exact metrics: different_components=%d max_error_r=%d max_error_g=%d max_error_b=%d max_error_a=%d\n",
		cmp8_lzw.different_components, cmp8_lzw.max_error_r, cmp8_lzw.max_error_g, cmp8_lzw.max_error_b, cmp8_lzw.max_error_a);

	TestImageF src16 = GenerateRoundtripTestPattern(257, 193, false);
	TestImage16 expected16 = QuantizeToRgba16(src16);
	TiffRgbaImage16 tiff16 = ToTiffRgbaImage16(expected16);
	if(!SaveTiffRgba16(~rgba16_path, tiff16, deflate_options, &error)) {
		printf("TIFF IO RGBA16 Deflate: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		SetExitCode(1);
		return;
	}
	Stage("TIFF IO RGBA16 Deflate", true, passed, failed);
	String rgba16_data = LoadFile(rgba16_path);
	printf("TIFF IO RGBA16 file_size=%lld bytes\n", (long long)rgba16_data.GetCount());
	Stage("TIFF IO RGBA16 tags", ValidateRgbaTags(~rgba16_path, 257, 193, 16, SAMPLEFORMAT_UINT, COMPRESSION_ADOBE_DEFLATE), passed, failed);
	TiffRgbaImage16 loaded16;
	if(!LoadTiffRgba16(~rgba16_path, loaded16, &error)) {
		printf("TIFF IO RGBA16 load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		SetExitCode(1);
		return;
	}
	TestImage16 actual16 = ToTestImage16(loaded16);
	RoundtripComparison16 cmp16 = CompareExact(expected16, actual16);
	Stage("TIFF IO RGBA16 exact comparison", cmp16.different_components == 0 && cmp16.dimensions_match, passed, failed);
	printf("TIFF IO RGBA16 exact metrics: different_components=%d max_error_r=%d max_error_g=%d max_error_b=%d max_error_a=%d\n",
		cmp16.different_components, cmp16.max_error_r, cmp16.max_error_g, cmp16.max_error_b, cmp16.max_error_a);

	TestImageF expectedf = GenerateRoundtripTestPattern(255, 191, true);
	TiffRgbaImageF tiffF = ToTiffRgbaImageF(expectedf);
	if(!SaveTiffRgbaF(~float_path, tiffF, none_options, &error)) {
		printf("TIFF IO Float32 NONE: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		SetExitCode(1);
		return;
	}
	Stage("TIFF IO Float32 NONE", true, passed, failed);
	String float_data = LoadFile(float_path);
	printf("TIFF IO Float32 file_size=%lld bytes\n", (long long)float_data.GetCount());
	Stage("TIFF IO Float32 tags", ValidateRgbaTags(~float_path, 255, 191, 32, SAMPLEFORMAT_IEEEFP, COMPRESSION_NONE), passed, failed);
	TiffRgbaImageF loadedf;
	if(!LoadTiffRgbaF(~float_path, loadedf, &error)) {
		printf("TIFF IO Float32 load: FAIL (%s)\n", IsNull(error) ? "unknown" : ~error);
		SetExitCode(1);
		return;
	}
	TestImageF actualf = ToTestImageF(loadedf);
	RoundtripComparison cmpf = CompareExact(expectedf, actualf);
	Stage("TIFF IO Float32 exact comparison", cmpf.different_components == 0 && cmpf.dimensions_match, passed, failed);
	printf("TIFF IO Float32 exact metrics: different_components=%d max_error_r=%.9g max_error_g=%.9g max_error_b=%.9g max_error_a=%.9g mean_absolute_error=%.9g rmse=%.9g\n",
		cmpf.different_components, cmpf.max_error_r, cmpf.max_error_g, cmpf.max_error_b, cmpf.max_error_a, cmpf.mean_absolute_error, cmpf.rmse);

	bool fixture_ok = true;
	TestImageF fixture_src = GenerateRoundtripTestPattern(16, 16, false);
	TestImage8 fixture_image = QuantizeToRgba8(fixture_src);
	TiffRgbaImage8 tiled_image = ToTiffRgbaImage8(fixture_image);
	fixture_ok = WriteTiledRgba8(~tiled_path, tiled_image);
	Stage("TIFF fixture tiled creation", fixture_ok, passed, failed);
	fixture_ok = fixture_ok && ValidateTiledFixture(~tiled_path);
	Stage("TIFF fixture tiled low-level validation", fixture_ok, passed, failed);
	fixture_ok = fixture_ok && LoadFailure8(~tiled_path, "tiled TIFF is not supported");
	Stage("TIFF fixture tiled rejection", fixture_ok, passed, failed);

	fixture_ok = WriteTwoDirectoryRgba8(~multipage_path);
	Stage("TIFF fixture multi-directory creation", fixture_ok, passed, failed);
	fixture_ok = fixture_ok && ValidateMultiDirectoryFixture(~multipage_path);
	Stage("TIFF fixture multi-directory low-level validation", fixture_ok, passed, failed);
	fixture_ok = fixture_ok && LoadFailure8(~multipage_path, "multi-directory TIFF is not supported");
	Stage("TIFF fixture multi-directory rejection", fixture_ok, passed, failed);

	fixture_ok = WritePlanarSeparateRgba8(~separate_path);
	Stage("TIFF fixture planar-separate creation", fixture_ok, passed, failed);
	fixture_ok = fixture_ok && ValidatePlanarSeparateFixture(~separate_path);
	Stage("TIFF fixture planar-separate low-level validation", fixture_ok, passed, failed);
	fixture_ok = fixture_ok && LoadFailure8(~separate_path, "unsupported TIFF tag values");
	Stage("TIFF fixture planar-separate rejection", fixture_ok, passed, failed);

	fixture_ok = WriteMalformedHeaderFile(~malformed_path);
	Stage("TIFF fixture malformed-header creation", fixture_ok, passed, failed);
	fixture_ok = fixture_ok && LoadFailure8(~malformed_path, NULL);
	Stage("TIFF fixture malformed-header rejection", fixture_ok, passed, failed);

	const bool source_valid = !rgba8_data.IsEmpty() && LoadTiffRgba8(~rgba8_path, loaded8, &error) && loaded8.IsValid() && ValidateRgbaTags(~rgba8_path, 256, 192, 8, SAMPLEFORMAT_UINT, COMPRESSION_ADOBE_DEFLATE);
	Stage("TIFF truncated source validation", source_valid, passed, failed);
	const bool truncated_written = source_valid && WriteTruncatedValidFile(~truncated_path, rgba8_data);
	Stage("TIFF fixture truncated creation", truncated_written, passed, failed);
	const int64_t valid_size = rgba8_data.GetCount();
	const String truncated_data = LoadFile(truncated_path);
	const bool truncated_smaller = truncated_written && truncated_data.GetCount() > 0 && truncated_data.GetCount() < valid_size;
	Stage("TIFF truncated size validation", truncated_smaller, passed, failed);
	TiffRgbaImage8 truncated_image;
	String truncated_error;
	const bool truncated_rejected = truncated_smaller && !LoadTiffRgba8(~truncated_path, truncated_image, &truncated_error) && !IsNull(truncated_error) && !truncated_image.IsValid();
	Stage("TIFF truncated rejection", truncated_rejected, passed, failed);

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
