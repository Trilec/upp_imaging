#ifndef UPP_IMAGING_PNG_IO_H
#define UPP_IMAGING_PNG_IO_H

#include <Core/Core.h>

struct PngRgba8
{
	byte r = 0;
	byte g = 0;
	byte b = 0;
	byte a = 255;
};

static_assert(sizeof(PngRgba8) == 4, "PngRgba8 must stay packed");

struct PngRgbaImage8
{
	int width = 0;
	int height = 0;
	Upp::Vector<PngRgba8> pixels;

	void Clear();
	bool IsValid() const;
};

bool SavePngRgba8(const char* path, const PngRgbaImage8& image, Upp::String* error = NULL);
bool LoadPngRgba8(const char* path, PngRgbaImage8& image, Upp::String* error = NULL);

#endif
