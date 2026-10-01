#ifndef _plugin_imaging_heif_h_
#define _plugin_imaging_heif_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {
INITIALIZE(ImagingHEIFRaster);
class ImagingHEIFRaster : public ImagingRaster {
public:
	ImagingHEIFRaster() : ImagingRaster("heif", ".heic") {}
};
}
#endif
