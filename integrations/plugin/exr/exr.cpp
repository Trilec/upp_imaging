#include "exr.h"

namespace Upp {

EXRRaster::EXRRaster() : ImagingRaster("openexr", ".exr") {}

INITIALIZER(EXRRaster)
{
	StreamRaster::Register<EXRRaster>();
}

}
