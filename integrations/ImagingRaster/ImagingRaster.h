#ifndef _ImagingRaster_ImagingRaster_h_
#define _ImagingRaster_ImagingRaster_h_

#include <Draw/Draw.h>

namespace Upp {

// These are preview budgets, separate from full-fidelity ImagingIO.
// Backend metadata and native decoder working memory are additional limits.
struct ImagingRasterLimits {
	int64 encoded_bytes = 64 * 1024 * 1024;
	int64 decoded_bytes = 256 * 1024 * 1024;
	int64 pixels = 16 * 1024 * 1024;
	int dimension = 16384;
	int channels = 64;
};

class ImagingRaster : public StreamRaster {
	String backend;
	String extension;
	ImagingRasterLimits limits;
	RasterFormat format;
	Info info;
	Size size;
	Vector<RGBA> pixels;

public:
	ImagingRaster(const char* reader, const char* suffix);
	void SetLimits(const ImagingRasterLimits& value) { limits = value; }
	bool Create() override;
	Size GetSize() override;
	Info GetInfo() override;
	Line GetLine(int line) override;
	const RasterFormat* GetFormat() override;
};

}
#endif
