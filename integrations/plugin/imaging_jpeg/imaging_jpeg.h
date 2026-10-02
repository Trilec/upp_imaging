#ifndef _plugin_imaging_jpeg_h_
#define _plugin_imaging_jpeg_h_
#include <ImagingRaster/ImagingRaster.h>
namespace Upp {
INITIALIZE(ImagingJPEGRaster);
class ImagingJPEGRaster : public ImagingRaster {
public:
    ImagingJPEGRaster() : ImagingRaster("jpeg", ".jpg") {}
};
}
#endif
