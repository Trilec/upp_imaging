#ifndef UPP_IMAGING_TIFF_IO_H
#define UPP_IMAGING_TIFF_IO_H

#include <Core/Core.h>

struct TiffRgba8
{
	byte r = 0;
	byte g = 0;
	byte b = 0;
	byte a = 255;
};

static_assert(sizeof(TiffRgba8) == 4, "TiffRgba8 must stay packed");

struct TiffRgba16
{
	Upp::uint16 r = 0;
	Upp::uint16 g = 0;
	Upp::uint16 b = 0;
	Upp::uint16 a = 65535;
};

static_assert(sizeof(TiffRgba16) == 8, "TiffRgba16 must stay packed");

struct TiffRgbaF
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;
};

static_assert(sizeof(TiffRgbaF) == 16, "TiffRgbaF must stay packed");

struct TiffRgbaImage8
{
	int width = 0;
	int height = 0;
	Upp::Vector<TiffRgba8> pixels;

	void Clear();
	bool IsValid() const;
};

struct TiffRgbaImage16
{
	int width = 0;
	int height = 0;
	Upp::Vector<TiffRgba16> pixels;

	void Clear();
	bool IsValid() const;
};

struct TiffRgbaImageF
{
	int width = 0;
	int height = 0;
	Upp::Vector<TiffRgbaF> pixels;

	void Clear();
	bool IsValid() const;
};

enum class TiffCompression
{
	None,
	Lzw,
	Deflate
};

struct TiffSaveOptions
{
	TiffCompression compression = TiffCompression::Deflate;
};

bool SaveTiffRgba8(const char* path, const TiffRgbaImage8& image, const TiffSaveOptions& options, Upp::String* error = NULL);
bool LoadTiffRgba8(const char* path, TiffRgbaImage8& image, Upp::String* error = NULL);

bool SaveTiffRgba16(const char* path, const TiffRgbaImage16& image, const TiffSaveOptions& options, Upp::String* error = NULL);
bool LoadTiffRgba16(const char* path, TiffRgbaImage16& image, Upp::String* error = NULL);

bool SaveTiffRgbaF(const char* path, const TiffRgbaImageF& image, const TiffSaveOptions& options, Upp::String* error = NULL);
bool LoadTiffRgbaF(const char* path, TiffRgbaImageF& image, Upp::String* error = NULL);

#endif
