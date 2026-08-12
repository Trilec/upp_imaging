#include "RegisterTIFF.h"

#include <OpenImageIO/imageio.h>

#include <mutex>

extern "C" {
OIIO::ImageInput* tiff_input_imageio_create();
OIIO::ImageOutput* tiff_output_imageio_create();
extern const char* tiff_input_extensions[];
extern const char* tiff_output_extensions[];
const char* tiff_imageio_library_version();
}

namespace UppImaging {

void RegisterOpenImageIOTIFFPlugin()
{
    static std::once_flag once;
    std::call_once(once, [] {
        OIIO::declare_imageio_format("tiff", tiff_input_imageio_create,
                                     tiff_input_extensions,
                                     tiff_output_imageio_create,
                                     tiff_output_extensions,
                                     tiff_imageio_library_version());
    });
}

}
