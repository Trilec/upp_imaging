#ifndef _ImagingIO_FormatPolicy_h_
#define _ImagingIO_FormatPolicy_h_

#include <ImagingIO/ImagingIO.h>
#include <OpenImageIO/imageio.h>

namespace Upp {
namespace Imaging {
namespace IOFormatPolicy {

String Extension(const String& path);
bool IsSupportedExtension(const String& extension);
bool RequiresZeroOrigin(const String& extension);
const char* FormatName(const String& extension);

void ConfigureRead(const String& extension, OIIO::ImageSpec& config);
void ConfigureWriteTarget(const String& extension, const ImageData& image,
                          OIIO::ImageSpec& target);

Result ValidateLoaded(const String& extension, const ImageSpec& spec,
                      Diagnostics* diagnostics, const String& path);
Result ValidateSave(const String& extension, const ImageData& image,
                    Diagnostics* diagnostics, const String& path);

} // namespace IOFormatPolicy
} // namespace Imaging
} // namespace Upp

#endif
