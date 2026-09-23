#include "RegisterHEIF.h"

#include <OpenImageIO/imageio.h>
#include <libheif/heif.h>

#include <mutex>

extern "C" {
OIIO::ImageInput* heif_input_imageio_create();
extern const char* heif_input_extensions[];
const char* heif_imageio_library_version();
}

OIIO_NAMESPACE_BEGIN
void oiio_heif_init();
OIIO_NAMESPACE_END

namespace UppImaging {

void RegisterOpenImageIOHEIFPlugin()
{
    static std::once_flag once;
    std::call_once(once, [] {
        OIIO::declare_imageio_format("heif", heif_input_imageio_create,
                                     heif_input_extensions,
                                     nullptr, nullptr,
                                     heif_imageio_library_version());
    });
}

void InitializeOpenImageIOHEIFPlugin()
{
    OIIO::oiio_heif_init();
}

void ShutdownOpenImageIOHEIFPlugin()
{
    heif_deinit();
}

}
