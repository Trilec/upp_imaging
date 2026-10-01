#ifndef _plugin_imaging_dpx_h_
#define _plugin_imaging_dpx_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {
INITIALIZE(ImagingDPXRaster);
class ImagingDPXRaster : public ImagingRaster {
public:
	ImagingDPXRaster() : ImagingRaster("dpx", ".dpx") {}
};
}
#endif
