#ifndef _plugin_imaging_hdr_h_
#define _plugin_imaging_hdr_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {
INITIALIZE(ImagingHDRRaster);
class ImagingHDRRaster : public ImagingRaster {
public:
	ImagingHDRRaster() : ImagingRaster("hdr", ".hdr") {}
};
}
#endif
