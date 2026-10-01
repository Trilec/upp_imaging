#ifndef _plugin_exr_exr_h_
#define _plugin_exr_exr_h_

#include <ImagingRaster/ImagingRaster.h>

namespace Upp {

INITIALIZE(EXRRaster);

class EXRRaster : public ImagingRaster {
public:
	EXRRaster();
};

}
#endif
