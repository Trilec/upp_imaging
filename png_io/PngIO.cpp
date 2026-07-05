#include "PngIO.h"

#include <libpng/png.h>

#if !defined(PNG_SIMPLIFIED_READ_SUPPORTED) || !defined(PNG_SIMPLIFIED_WRITE_STDIO_SUPPORTED) || !defined(PNG_STDIO_SUPPORTED)
#error "libpng simplified png_image API is unavailable"
#endif

using namespace Upp;

static String MakePngError(const png_image& image, const char* fallback)
{
	if(image.message[0] != 0)
		return image.message;
	return fallback;
}

static bool CheckImageAllocation(const PngRgbaImage8& image, String& error)
{
	if(image.width <= 0 || image.height <= 0) {
		error = "invalid PNG image dimensions";
		return false;
	}
	const int64 pixels = (int64)image.width * (int64)image.height;
	if(pixels <= 0 || pixels > INT_MAX) {
		error = "PNG image pixel count overflow";
		return false;
	}
	if(image.pixels.GetCount() != (int)pixels) {
		error = "PNG image pixel count mismatch";
		return false;
	}
	return true;
}

void PngRgbaImage8::Clear()
{
	width = 0;
	height = 0;
	pixels.Clear();
}

bool PngRgbaImage8::IsValid() const
{
	return width > 0 && height > 0 && pixels.GetCount() == (int64)width * height;
}

bool SavePngRgba8(const char* path, const PngRgbaImage8& image, String* error)
{
	if(error)
		error->Clear();
	if(path == NULL || !*path) {
		if(error)
			*error = "empty PNG path";
		return false;
	}
	String validation_error;
	if(!CheckImageAllocation(image, validation_error)) {
		if(error)
			*error = validation_error;
		return false;
	}
	png_image png;
	memset(&png, 0, sizeof(png));
	png.version = PNG_IMAGE_VERSION;
	png.width = (png_uint_32)image.width;
	png.height = (png_uint_32)image.height;
	png.format = PNG_FORMAT_RGBA;
	png.flags = 0;
	png.colormap_entries = 0;
	const png_int_32 row_stride = (png_int_32)((int64)image.width * (int64)sizeof(PngRgba8));
	if(!png_image_write_to_file(&png, path, 0, image.pixels.Begin(), row_stride, NULL)) {
		String png_error = MakePngError(png, "PNG write failed");
		png_image_free(&png);
		if(error)
			*error = png_error;
		return false;
	}
	png_image_free(&png);
	return true;
}

bool LoadPngRgba8(const char* path, PngRgbaImage8& image, String* error)
{
	if(error)
		error->Clear();
	image.Clear();
	if(path == NULL || !*path) {
		if(error)
			*error = "empty PNG path";
		return false;
	}
	png_image png;
	memset(&png, 0, sizeof(png));
	png.version = PNG_IMAGE_VERSION;
	if(!png_image_begin_read_from_file(&png, path)) {
		String png_error = MakePngError(png, "PNG read header failed");
		png_image_free(&png);
		if(error)
			*error = png_error;
		return false;
	}
	if(png.width == 0 || png.height == 0) {
		png_image_free(&png);
		if(error)
			*error = "PNG has zero dimensions";
		return false;
	}
	const int64 pixels = (int64)png.width * (int64)png.height;
	if(pixels <= 0 || pixels > INT_MAX) {
		png_image_free(&png);
		if(error)
			*error = "PNG image pixel count overflow";
		return false;
	}
	image.width = (int)png.width;
	image.height = (int)png.height;
	image.pixels.SetCount((int)pixels);
	png.format = PNG_FORMAT_RGBA;
	const png_int_32 row_stride = (png_int_32)((int64)image.width * (int64)sizeof(PngRgba8));
	if(!png_image_finish_read(&png, NULL, image.pixels.Begin(), row_stride, NULL)) {
		image.Clear();
		if(error)
			*error = MakePngError(png, "PNG read failed");
		png_image_free(&png);
		return false;
	}
	png_image_free(&png);
	return true;
}
