#include <stdio.h>
#include <string.h>

#include <Core/Core.h>
#include <libtiff_src/upstream/libtiff/tiffio.h>

struct TiffRgba8
{
	byte r = 0;
	byte g = 0;
	byte b = 0;
	byte a = 0;
};

using namespace Upp;

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

static bool Check(bool condition, const char* label, int& passed, int& failed)
{
	printf("%s: %s\n", label, condition ? "OK" : "FAIL");
	if(condition)
		++passed;
	else
		++failed;
	return condition;
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;
	String error;
	Check(String(TIFFGetVersion()).Find("4.7.2") >= 0, "version info", passed, failed);

	const String path = GetExeDirFile("libtiff_probe.tif");
	FileDelete(path);

	TiffMessageState state;
	state.error = &error;
	TIFF* tif = OpenTiff(~path, "w", state);
	if(!Check(tif != NULL, "TIFFOpenExt write", passed, failed)) {
		printf("SUMMARY passed=%d failed=%d\n", passed, failed);
		SetExitCode(1);
		return;
	}

	const TiffRgba8 pixel = {11, 22, 33, 44};
	uint16_t extra = EXTRASAMPLE_UNASSALPHA;
	bool ok = TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32_t)1)
		&& TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32_t)1)
		&& TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)4)
		&& TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)8)
		&& TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, (uint16_t)SAMPLEFORMAT_UINT)
		&& TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_RGB)
		&& TIFFSetField(tif, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_CONTIG)
		&& TIFFSetField(tif, TIFFTAG_ORIENTATION, (uint16_t)ORIENTATION_TOPLEFT)
		&& TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, (uint16_t)1, &extra)
		&& TIFFSetField(tif, TIFFTAG_COMPRESSION, (uint16_t)COMPRESSION_NONE)
		&& TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (uint32_t)1);
	Check(ok, "TIFFSetField", passed, failed);
	Check(TIFFWriteScanline(tif, (void*)&pixel, 0, 0) >= 0, "TIFFWriteScanline", passed, failed);
	TIFFClose(tif);
	Check(true, "TIFFClose write", passed, failed);

	TiffMessageState read_state;
	read_state.error = &error;
	TIFF* read_tif = OpenTiff(~path, "r", read_state);
	if(!Check(read_tif != NULL, "TIFFOpenExt read", passed, failed)) {
		printf("SUMMARY passed=%d failed=%d\n", passed, failed);
		SetExitCode(1);
		return;
	}
	TiffRgba8 loaded = {};
	Check(TIFFReadScanline(read_tif, (void*)&loaded, 0, 0) >= 0, "TIFFReadScanline", passed, failed);
	Check(TIFFNumberOfDirectories(read_tif) == 1, "TIFFNumberOfDirectories", passed, failed);
	TIFFClose(read_tif);
	Check(true, "TIFFClose read", passed, failed);
	Check(loaded.r == pixel.r && loaded.g == pixel.g && loaded.b == pixel.b && loaded.a == pixel.a, "pixel round-trip", passed, failed);

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
