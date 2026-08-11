#ifndef _plugin_exr_exr_h_
#define _plugin_exr_exr_h_

#include <Draw/Draw.h>

namespace Upp {

INITIALIZE(EXRRaster);

class EXRRaster : public StreamRaster {
	RasterFormat format;
	Info         info;
	Size         size;
	Vector<RGBA> pixels;

public:
	EXRRaster();

	virtual bool                Create();
	virtual Size                GetSize();
	virtual Info                GetInfo();
	virtual Line                GetLine(int line);
	virtual const RasterFormat *GetFormat();
};

} // namespace Upp

#endif
