#include "RegisterJPEG.h"
#include <OpenImageIO/imageio.h>
#include <mutex>
extern "C" {
OIIO::ImageInput* jpeg_input_imageio_create();
OIIO::ImageOutput* jpeg_output_imageio_create();
extern const char* jpeg_input_extensions[];
extern const char* jpeg_output_extensions[];
const char* jpeg_imageio_library_version();
}
namespace UppImaging {
void RegisterOpenImageIOJPEGPlugin()
{
    static std::once_flag once;
    std::call_once(once, [] {
        OIIO::declare_imageio_format("jpeg", jpeg_input_imageio_create,
            jpeg_input_extensions, jpeg_output_imageio_create,
            jpeg_output_extensions, jpeg_imageio_library_version());
    });
}
}
