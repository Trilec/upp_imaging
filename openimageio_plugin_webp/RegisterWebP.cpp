#include "RegisterWebP.h"

#include <OpenImageIO/imageio.h>
#include <mutex>

extern "C" {
OIIO::ImageInput* webp_input_imageio_create();
OIIO::ImageOutput* webp_output_imageio_create();
extern const char* webp_input_extensions[];
extern const char* webp_output_extensions[];
const char* webp_imageio_library_version();
}

namespace UppImaging {

void RegisterOpenImageIOWebPPlugin()
{
    static std::once_flag once;
    std::call_once(once, [] {
        OIIO::declare_imageio_format("webp", webp_input_imageio_create,
                                     webp_input_extensions,
                                     webp_output_imageio_create,
                                     webp_output_extensions,
                                     webp_imageio_library_version());
    });
}

}
