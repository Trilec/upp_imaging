#include "RegisterHDR.h"

#include <OpenImageIO/imageio.h>

#include <mutex>

extern "C" {
OIIO::ImageInput* hdr_input_imageio_create();
OIIO::ImageOutput* hdr_output_imageio_create();
extern const char* hdr_input_extensions[];
extern const char* hdr_output_extensions[];
const char* hdr_imageio_library_version();
}

namespace UppImaging {

void RegisterOpenImageIOHDRPlugin()
{
    static std::once_flag once;
    std::call_once(once, [] {
        OIIO::declare_imageio_format("hdr", hdr_input_imageio_create,
                                     hdr_input_extensions,
                                     hdr_output_imageio_create,
                                     hdr_output_extensions,
                                     hdr_imageio_library_version());
    });
}

}
