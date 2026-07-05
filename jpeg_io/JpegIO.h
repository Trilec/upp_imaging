#ifndef UPP_IMAGING_JPEG_IO_H
#define UPP_IMAGING_JPEG_IO_H

#include <Core/Core.h>

struct JpegRgb8
{
	byte r = 0;
	byte g = 0;
	byte b = 0;
};

static_assert(sizeof(JpegRgb8) == 3, "JpegRgb8 must stay packed");

struct JpegRgbImage8
{
	int width = 0;
	int height = 0;
	Upp::Vector<JpegRgb8> pixels;

	void Clear();
	bool IsValid() const;
};

enum class JpegSubsampling
{
	S444,
	S422,
	S420
};

struct JpegSaveOptions
{
	int quality = 95;
	JpegSubsampling subsampling = JpegSubsampling::S444;
	bool progressive = false;
	bool optimize_coding = true;
};

bool SaveJpegRgb8(const char* path, const JpegRgbImage8& image, const JpegSaveOptions& options, Upp::String* error = NULL);
bool LoadJpegRgb8(const char* path, JpegRgbImage8& image, Upp::String* error = NULL);

#endif
