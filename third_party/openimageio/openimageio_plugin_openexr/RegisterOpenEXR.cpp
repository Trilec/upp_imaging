#include "RegisterOpenEXR.h"

#include <OpenImageIO/imageio.h>

#include <mutex>

extern "C" {
OIIO::ImageInput* openexr_input_imageio_create();
OIIO::ImageOutput* openexr_output_imageio_create();
extern const char* openexr_input_extensions[];
extern const char* openexr_output_extensions[];
const char* openexr_imageio_library_version();
}

namespace UppImaging {

void RegisterOpenImageIOOpenEXRPlugin()
{
    static std::once_flag once;
    std::call_once(once, [] {
        OIIO::declare_imageio_format("openexr", openexr_input_imageio_create,
                                     openexr_input_extensions,
                                     openexr_output_imageio_create,
                                     openexr_output_extensions,
                                     openexr_imageio_library_version());
    });
}

}
