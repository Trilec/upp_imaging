#ifndef _plugin_imaging_raw_h_
#define _plugin_imaging_raw_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {
INITIALIZE(ImagingRAWRaster);
class ImagingRAWRaster : public ImagingRaster {
public:
	ImagingRAWRaster() : ImagingRaster("raw", ".dng") {}
};
}
#endif
