#include "RegisterDPXCineon.h"

#include <OpenImageIO/imageio.h>

#include <mutex>

extern "C" {
OIIO::ImageInput* dpx_input_imageio_create();
OIIO::ImageOutput* dpx_output_imageio_create();
extern const char* dpx_input_extensions[];
extern const char* dpx_output_extensions[];
const char* dpx_imageio_library_version();

OIIO::ImageInput* cineon_input_imageio_create();
extern const char* cineon_input_extensions[];
const char* cineon_imageio_library_version();
}

namespace UppImaging {

void RegisterOpenImageIODPXCineonPlugins()
{
    static std::once_flag once;
    std::call_once(once, [] {
        OIIO::declare_imageio_format("dpx", dpx_input_imageio_create,
                                     dpx_input_extensions,
                                     dpx_output_imageio_create,
                                     dpx_output_extensions,
                                     dpx_imageio_library_version());
        OIIO::declare_imageio_format("cineon", cineon_input_imageio_create,
                                     cineon_input_extensions,
                                     nullptr, nullptr,
                                     cineon_imageio_library_version());
    });
}

}
