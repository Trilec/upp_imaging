#ifndef UPP_IMAGING_OPENEXR_IO_H
#define UPP_IMAGING_OPENEXR_IO_H

#include <Core/Core.h>
#include <openexr_core/openexr.h>

struct ExrRgbaF
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;
};

struct ExrRgbaImageF
{
	int width = 0;
	int height = 0;
	Upp::Vector<ExrRgbaF> pixels;

	void Clear()
	{
		width = 0;
		height = 0;
		pixels.Clear();
	}
};

bool SaveExrRgbaF(const char* path, const ExrRgbaImageF& image, bool output_half, bool use_zip, Upp::String* error = NULL);
bool LoadExrRgbaF(const char* path, ExrRgbaImageF& image, Upp::String* error = NULL);

#endif
