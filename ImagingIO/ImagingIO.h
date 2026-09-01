#ifndef _ImagingIO_ImagingIO_h_
#define _ImagingIO_ImagingIO_h_

#include <ImagingCore/ImagingCore.h>

namespace Upp {
namespace Imaging {

Result LoadImageFile(const String& path, ImageData& output, Diagnostics* diagnostics = nullptr);
Result SaveImageFile(const String& path, const ImageData& image, Diagnostics* diagnostics = nullptr);

} // namespace Imaging
} // namespace Upp

#endif
