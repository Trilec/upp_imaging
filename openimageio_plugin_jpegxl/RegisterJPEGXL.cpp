#include "RegisterJPEGXL.h"

#include <OpenImageIO/imageio.h>

#include <mutex>

extern "C" {
OIIO::ImageInput* jpegxl_input_imageio_create();
OIIO::ImageOutput* jpegxl_output_imageio_create();
extern const char* jpegxl_input_extensions[];
extern const char* jpegxl_output_extensions[];
const char* jpegxl_imageio_library_version();
}

namespace UppImaging {

void RegisterOpenImageIOJPEGXLPlugin()
{
    static std::once_flag once;
    std::call_once(once, [] {
        OIIO::declare_imageio_format("jpegxl", jpegxl_input_imageio_create,
                                     jpegxl_input_extensions,
                                     jpegxl_output_imageio_create,
                                     jpegxl_output_extensions,
                                     jpegxl_imageio_library_version());
    });
}

}
