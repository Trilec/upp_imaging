#ifndef _plugin_imaging_tiff_h_
#define _plugin_imaging_tiff_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {
INITIALIZE(ImagingTIFFRaster);
class ImagingTIFFRaster : public ImagingRaster {
public:
	ImagingTIFFRaster() : ImagingRaster("tiff", ".tif") {}
};
}
#endif
