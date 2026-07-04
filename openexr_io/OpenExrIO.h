#ifndef UPP_IMAGING_OPENEXR_IO_H
#define UPP_IMAGING_OPENEXR_IO_H

#include <Core/Core.h>
#include <openexr_core/openexr.h>

struct ExrRgbaF
{
	float r;
	float g;
	float b;
	float a;
};

struct ExrRgbaImageF
{
	int width;
	int height;
	Upp::Vector<ExrRgbaF> pixels;
};

bool SaveExrRgbaF(const char* path, const ExrRgbaImageF& image, bool output_half, bool use_zip, Upp::String* error = NULL);
bool LoadExrRgbaF(const char* path, ExrRgbaImageF& image, Upp::String* error = NULL);

#endif
