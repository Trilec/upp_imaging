#include "RegisterPNG.h"

#include <OpenImageIO/imageio.h>

#include <mutex>

extern "C" {
OIIO::ImageInput* png_input_imageio_create();
OIIO::ImageOutput* png_output_imageio_create();
extern const char* png_input_extensions[];
extern const char* png_output_extensions[];
const char* png_imageio_library_version();
}

namespace UppImaging {

void RegisterOpenImageIOPNGPlugin()
{
    static std::once_flag once;
    std::call_once(once, [] {
        OIIO::declare_imageio_format("png", png_input_imageio_create,
                                     png_input_extensions,
                                     png_output_imageio_create,
                                     png_output_extensions,
                                     png_imageio_library_version());
    });
}

}
