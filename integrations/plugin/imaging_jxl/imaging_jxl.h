#ifndef _plugin_imaging_jxl_h_
#define _plugin_imaging_jxl_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {
INITIALIZE(ImagingJXLRaster);
class ImagingJXLRaster : public ImagingRaster {
public:
	ImagingJXLRaster() : ImagingRaster("jpegxl", ".jxl") {}
};
}
#endif
