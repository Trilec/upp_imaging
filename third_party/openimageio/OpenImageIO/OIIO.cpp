#include "OIIO.h"

#include <cstdlib>
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

#if defined(_WIN32) && defined(__MINGW32__)
OIIO_NAMESPACE_3_1_BEGIN
namespace pvt {
void ReleaseCurrentThreadInputErrors();
void ReleaseCurrentThreadOutputErrors();
void ReleaseCurrentThreadGlobalError();
}
OIIO_NAMESPACE_3_1_END
#endif

namespace UppImaging {

namespace {

void ShutdownHEIFAtExit()
{
    ShutdownOpenImageIOHEIFPlugin();
}

#if defined(_WIN32) && defined(__MINGW32__)
void ShutdownErrorStorageAtExit()
{
    // MinGW retains main-thread TLS beyond U++'s static heap audit. The
    // default pool owns its workers and may already be destroyed here;
    // touch only the exiting thread's error storage.
    OIIO::pvt::ReleaseCurrentThreadInputErrors();
    OIIO::pvt::ReleaseCurrentThreadOutputErrors();
    OIIO::pvt::ReleaseCurrentThreadGlobalError();
}
#endif

}

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
        UppImaging::InitializeOpenImageIOHEIFPlugin();
        std::atexit(ShutdownHEIFAtExit);
#if defined(_WIN32) && defined(__MINGW32__)
        std::atexit(ShutdownErrorStorageAtExit);
#endif
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
