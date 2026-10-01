#ifndef _plugin_imaging_png_h_
#define _plugin_imaging_png_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {
INITIALIZE(ImagingPNGRaster);
class ImagingPNGRaster : public ImagingRaster {
public:
	ImagingPNGRaster() : ImagingRaster("png", ".png") {}
};
}
#endif
