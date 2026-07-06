#include "TiffIO.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <libtiff/tiffio.h>

using namespace Upp;

static float ClampFloat(float v, float lo, float hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

struct TiffMessageState
{
	String* error = NULL;
	int warnings = 0;
	int errors = 0;
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
	if(state) {
		++state->errors;
		if(state->error)
			*state->error = TiffFormatMessage(module, fmt, ap);
	}
	return 1;
}

static int TiffWarningHandler(TIFF*, void* user_data, const char* module, const char* fmt, va_list ap)
{
	TiffMessageState* state = (TiffMessageState*)user_data;
	if(state) {
		++state->warnings;
		if(state->error && IsNull(*state->error))
			*state->error = TiffFormatMessage(module, fmt, ap);
	}
	return 1;
}

static TIFFOpenOptions* OpenOptions(TiffMessageState& state)
{
	TIFFOpenOptions* opts = TIFFOpenOptionsAlloc();
	if(!opts)
		return NULL;
	TIFFOpenOptionsSetErrorHandlerExtR(opts, TiffErrorHandler, &state);
	TIFFOpenOptionsSetWarningHandlerExtR(opts, TiffWarningHandler, &state);
	return opts;
}

static TIFF* OpenTiff(const char* path, const char* mode, TiffMessageState& state)
{
	TIFFOpenOptions* opts = OpenOptions(state);
	if(!opts) {
		if(state.error)
			*state.error = "TIFFOpenOptionsAlloc failed";
		return NULL;
	}
	TIFF* tif = TIFFOpenExt(path, mode, opts);
	TIFFOpenOptionsFree(opts);
	return tif;
}

static uint16_t CompressionTag(TiffCompression compression)
{
	switch(compression) {
	case TiffCompression::None: return COMPRESSION_NONE;
	case TiffCompression::Lzw: return COMPRESSION_LZW;
	case TiffCompression::Deflate: return COMPRESSION_ADOBE_DEFLATE;
	}
	return COMPRESSION_ADOBE_DEFLATE;
}

static bool CheckPath(const char* path, String& error)
{
	if(path == NULL || !*path) {
		error = "empty TIFF path";
		return false;
	}
	return true;
}

static bool CheckCompression(TiffCompression compression, String& error)
{
	switch(compression) {
	case TiffCompression::None:
	case TiffCompression::Lzw:
	case TiffCompression::Deflate:
		return true;
	}
	error = "invalid TIFF compression";
	return false;
}

template <class TImage>
static bool CheckImageCommon(const TImage& image, String& error)
{
	if(image.width <= 0 || image.height <= 0) {
		error = "invalid TIFF image dimensions";
		return false;
	}
	const int64 pixels = (int64)image.width * (int64)image.height;
	if(pixels <= 0 || pixels > INT_MAX) {
		error = "TIFF image pixel count overflow";
		return false;
	}
	if(image.pixels.GetCount() != (int)pixels) {
		error = "TIFF image pixel count mismatch";
		return false;
	}
	return true;
}

static bool SetCommonTags(TIFF* tif, int width, int height, uint16_t bits_per_sample, uint16_t sample_format, TiffCompression compression, String& error)
{
	uint16_t extra = EXTRASAMPLE_UNASSALPHA;
	if(!TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32_t)width) ||
	   !TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32_t)height) ||
	   !TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)4) ||
	   !TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bits_per_sample) ||
	   !TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sample_format) ||
	   !TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_RGB) ||
	   !TIFFSetField(tif, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_CONTIG) ||
	   !TIFFSetField(tif, TIFFTAG_ORIENTATION, (uint16_t)ORIENTATION_TOPLEFT) ||
	   !TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, (uint16_t)1, &extra) ||
	   !TIFFSetField(tif, TIFFTAG_COMPRESSION, CompressionTag(compression)) ||
	   !TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (uint32_t)1)) {
		error = IsNull(error) ? "TIFFSetField failed" : error;
		return false;
	}
	return true;
}

static bool ValidateCommonLoadedTags(TIFF* tif, uint32_t& width, uint32_t& height, uint16_t& bits, uint16_t& sample_format, uint16_t& compression, String& error)
{
	uint16_t spp = 0;
	uint16_t photometric = 0;
	uint16_t planarconfig = 0;
	uint16_t orientation = 0;
	uint32_t rows_per_strip = 0;
	uint16_t extra_count = 0;
	uint16_t* extra_types = NULL;
	if(!TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width) ||
	   !TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height) ||
	   !TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp) ||
	   !TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits) ||
	   !TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sample_format) ||
	   !TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric) ||
	   !TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarconfig) ||
	   !TIFFGetField(tif, TIFFTAG_ORIENTATION, &orientation) ||
	   !TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &extra_count, &extra_types) ||
	   !TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression) ||
	   !TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rows_per_strip)) {
		error = "required TIFF tag missing";
		return false;
	}
	if(spp != 4 || photometric != PHOTOMETRIC_RGB || planarconfig != PLANARCONFIG_CONTIG ||
	   orientation != ORIENTATION_TOPLEFT || extra_count != 1 || extra_types == NULL ||
	   extra_types[0] != EXTRASAMPLE_UNASSALPHA || rows_per_strip == 0) {
		error = "unsupported TIFF tag values";
		return false;
	}
	return true;
}

template <class TPixel>
static bool SaveTiffTyped(const char* path, const TPixel* pixels, int width, int height, uint16_t bits_per_sample, uint16_t sample_format, const TiffSaveOptions& options, String* error)
{
	if(error)
		error->Clear();
	String validation_error;
	if(!CheckPath(path, validation_error) || !CheckCompression(options.compression, validation_error)) {
		if(error)
			*error = validation_error;
		return false;
	}
	const int64 pixels_count = (int64)width * (int64)height;
	if(pixels_count <= 0 || pixels_count > INT_MAX) {
		if(error)
			*error = "TIFF image pixel count overflow";
		return false;
	}
	const size_t row_bytes = (size_t)width * sizeof(TPixel);
	if(row_bytes == 0 || row_bytes > (size_t)INT_MAX) {
		if(error)
			*error = "TIFF row byte overflow";
		return false;
	}

	TiffMessageState state;
	state.error = error;
	TIFF* tif = OpenTiff(path, "w", state);
	if(!tif) {
		if(error && IsNull(*error))
			*error = "TIFF open for write failed";
		return false;
	}
	bool ok = false;
	if(!SetCommonTags(tif, width, height, bits_per_sample, sample_format, options.compression, validation_error))
		goto done;
	if(TIFFScanlineSize(tif) != (tmsize_t)row_bytes) {
		validation_error = "unexpected TIFF scanline size";
		goto done;
	}
	for(int y = 0; y < height; ++y) {
		if(TIFFWriteScanline(tif, (void*)(pixels + (size_t)y * (size_t)width), y, 0) < 0) {
			if(IsNull(validation_error))
				validation_error = "TIFF write scanline failed";
			goto done;
		}
	}
	ok = true;

	done:
	TIFFClose(tif);
	if(ok && state.warnings == 0 && state.errors == 0) {
		if(error)
			error->Clear();
		return true;
	}
	if(error && IsNull(*error))
		*error = IsNull(validation_error) ? "TIFF write failed" : validation_error;
	FileDelete(path);
	return false;
}

template <class TPixel, class TImage>
static bool LoadTiffTyped(const char* path, TImage& image, uint16_t expected_bits, uint16_t expected_sample_format, String* error)
{
	if(error)
		error->Clear();
	image.Clear();
	String validation_error;
	if(!CheckPath(path, validation_error)) {
		if(error)
			*error = validation_error;
		return false;
	}
	TiffMessageState state;
	state.error = error;
	TIFF* tif = OpenTiff(path, "r", state);
	if(!tif) {
		if(error && IsNull(*error))
			*error = "TIFF open for read failed";
		return false;
	}
	bool ok = false;
	uint32_t width = 0;
	uint32_t height = 0;
	uint16_t bits = 0;
	uint16_t sample_format = 0;
	uint16_t compression = 0;
	int64_t pixels_count = 0;
	size_t row_bytes = 0;
	if(TIFFIsTiled(tif)) {
		validation_error = "tiled TIFF is not supported";
		goto done;
	}
	if(TIFFNumberOfDirectories(tif) != 1) {
		validation_error = "multi-directory TIFF is not supported";
		goto done;
	}
	if(!ValidateCommonLoadedTags(tif, width, height, bits, sample_format, compression, validation_error))
		goto done;
	if(bits != expected_bits || sample_format != expected_sample_format) {
		validation_error = "unsupported TIFF sample format or bit depth";
		goto done;
	}
	if(compression != COMPRESSION_NONE && compression != COMPRESSION_LZW &&
	   compression != COMPRESSION_DEFLATE && compression != COMPRESSION_ADOBE_DEFLATE) {
		validation_error = "unsupported TIFF compression";
		goto done;
	}
	pixels_count = (int64_t)width * (int64_t)height;
	if(pixels_count <= 0 || pixels_count > INT_MAX) {
		validation_error = "TIFF image pixel count overflow";
		goto done;
	}
	row_bytes = (size_t)width * sizeof(TPixel);
	if(TIFFScanlineSize(tif) != (tmsize_t)row_bytes) {
		validation_error = "unexpected TIFF scanline size";
		goto done;
	}
	image.width = (int)width;
	image.height = (int)height;
	image.pixels.SetCount((int)pixels_count);
	for(uint32_t y = 0; y < height; ++y) {
		if(TIFFReadScanline(tif, (void*)(image.pixels.Begin() + (size_t)y * (size_t)width), y, 0) < 0) {
			validation_error = "TIFF read scanline failed";
			image.Clear();
			goto done;
		}
	}
	ok = true;

	done:
	TIFFClose(tif);
	if(ok && state.warnings == 0 && state.errors == 0) {
		if(error)
			error->Clear();
		return true;
	}
	image.Clear();
	if(error && IsNull(*error))
		*error = IsNull(validation_error) ? "TIFF read failed" : validation_error;
	return false;
}

void TiffRgbaImage8::Clear()
{
	width = 0;
	height = 0;
	pixels.Clear();
}

bool TiffRgbaImage8::IsValid() const
{
	return width > 0 && height > 0 && pixels.GetCount() == (int64)width * height;
}

void TiffRgbaImage16::Clear()
{
	width = 0;
	height = 0;
	pixels.Clear();
}

bool TiffRgbaImage16::IsValid() const
{
	return width > 0 && height > 0 && pixels.GetCount() == (int64)width * height;
}

void TiffRgbaImageF::Clear()
{
	width = 0;
	height = 0;
	pixels.Clear();
}

bool TiffRgbaImageF::IsValid() const
{
	return width > 0 && height > 0 && pixels.GetCount() == (int64)width * height;
}

bool SaveTiffRgba8(const char* path, const TiffRgbaImage8& image, const TiffSaveOptions& options, String* error)
{
	String validation_error;
	if(!CheckImageCommon(image, validation_error)) {
		if(error)
			*error = validation_error;
		return false;
	}
	return SaveTiffTyped<TiffRgba8>(path, image.pixels.Begin(), image.width, image.height, 8, SAMPLEFORMAT_UINT, options, error);
}

bool LoadTiffRgba8(const char* path, TiffRgbaImage8& image, String* error)
{
	return LoadTiffTyped<TiffRgba8, TiffRgbaImage8>(path, image, 8, SAMPLEFORMAT_UINT, error);
}

bool SaveTiffRgba16(const char* path, const TiffRgbaImage16& image, const TiffSaveOptions& options, String* error)
{
	String validation_error;
	if(!CheckImageCommon(image, validation_error)) {
		if(error)
			*error = validation_error;
		return false;
	}
	return SaveTiffTyped<TiffRgba16>(path, image.pixels.Begin(), image.width, image.height, 16, SAMPLEFORMAT_UINT, options, error);
}

bool LoadTiffRgba16(const char* path, TiffRgbaImage16& image, String* error)
{
	return LoadTiffTyped<TiffRgba16, TiffRgbaImage16>(path, image, 16, SAMPLEFORMAT_UINT, error);
}

bool SaveTiffRgbaF(const char* path, const TiffRgbaImageF& image, const TiffSaveOptions& options, String* error)
{
	String validation_error;
	if(!CheckImageCommon(image, validation_error)) {
		if(error)
			*error = validation_error;
		return false;
	}
	return SaveTiffTyped<TiffRgbaF>(path, image.pixels.Begin(), image.width, image.height, 32, SAMPLEFORMAT_IEEEFP, options, error);
}

bool LoadTiffRgbaF(const char* path, TiffRgbaImageF& image, String* error)
{
	return LoadTiffTyped<TiffRgbaF, TiffRgbaImageF>(path, image, 32, SAMPLEFORMAT_IEEEFP, error);
}
