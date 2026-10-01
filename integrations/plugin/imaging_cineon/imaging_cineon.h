#ifndef _plugin_imaging_cineon_h_
#define _plugin_imaging_cineon_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {
INITIALIZE(ImagingCineonRaster);
class ImagingCineonRaster : public ImagingRaster {
public:
	ImagingCineonRaster() : ImagingRaster("cineon", ".cin") {}
};
}
#endif
