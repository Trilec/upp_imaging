#ifndef _plugin_imaging_webp_h_
#define _plugin_imaging_webp_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {
INITIALIZE(ImagingWebPRaster);
class ImagingWebPRaster : public ImagingRaster {
public:
	ImagingWebPRaster() : ImagingRaster("webp", ".webp") {}
};
}
#endif
