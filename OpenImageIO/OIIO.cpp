#include "OIIO.h"

#include <mutex>

#include <openimageio_plugin_openexr/RegisterOpenEXR.h>
#include <openimageio_plugin_png/RegisterPNG.h>
#include <openimageio_plugin_jpegxl/RegisterJPEGXL.h>
#include <openimageio_plugin_hdr/RegisterHDR.h>
#include <openimageio_plugin_dpxcineon/RegisterDPXCineon.h>
#include <openimageio_plugin_raw/RegisterRaw.h>
#include <openimageio_plugin_webp/RegisterWebP.h>
#include <openimageio_plugin_heif/RegisterHEIF.h>
#include <openimageio_plugin_tiff/RegisterTIFF.h>

namespace UppImaging {

void InitializeOpenImageIO()
{
    static std::once_flag once;
    std::call_once(once, [] {
        UppImaging::RegisterOpenImageIOOpenEXRPlugin();
        UppImaging::RegisterOpenImageIOPNGPlugin();
        UppImaging::RegisterOpenImageIOJPEGXLPlugin();
        UppImaging::RegisterOpenImageIOHDRPlugin();
        UppImaging::RegisterOpenImageIODPXCineonPlugins();
        UppImaging::RegisterOpenImageIORawPlugin();
        UppImaging::RegisterOpenImageIOWebPPlugin();
        UppImaging::RegisterOpenImageIOHEIFPlugin();
        UppImaging::RegisterOpenImageIOTIFFPlugin();
    });
}

bool LoadImage(const char* filename, OIIO::ImageBuf& destination,
               std::string* error)
{
    InitializeOpenImageIO();
    if(!filename || !*filename) {
        if(error)
            *error = "empty image filename";
        return false;
    }
    OIIO::ImageBuf loaded(filename);
    if(!loaded.read()) {
        if(error)
            *error = loaded.geterror();
        return false;
    }
    destination = std::move(loaded);
    return true;
}

bool SaveImage(const char* filename, const OIIO::ImageBuf& source,
               std::string* error)
{
    InitializeOpenImageIO();
    if(!filename || !*filename) {
        if(error)
            *error = "empty image filename";
        return false;
    }
    if(!source.write(filename)) {
        if(error)
            *error = source.geterror();
        return false;
    }
    return true;
}

}
