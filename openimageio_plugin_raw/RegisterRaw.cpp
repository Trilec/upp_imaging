#include "RegisterRaw.h"

#include <OpenImageIO/imageio.h>

#include <mutex>

extern "C" {
OIIO::ImageInput* raw_input_imageio_create();
const char* raw_imageio_library_version();
}

namespace UppImaging {

void RegisterOpenImageIORawPlugin()
{
    static std::once_flag once;
    std::call_once(once, [] {
        // OIIO's upstream LibRaw adapter also advertises "hdr" because a few
        // camera formats use that suffix. This framework reserves .hdr for
        // Radiance RGBE, so RAW registration deliberately excludes it.
        static const char* extensions[] = {
            "bay", "bmq", "cr2", "cr3", "crw", "cs1", "dc2", "dcr",
            "dng", "erf", "fff", "k25", "kdc", "mdc", "mos", "mrw",
            "nef", "orf", "pef", "pxn", "raf", "raw", "rdc", "sr2",
            "srf", "x3f", "arw", "3fr", "cine", "ia", "kc2", "mef",
            "nrw", "qtk", "rw2", "sti", "rwl", "srw", "drf", "dsc",
            "ptx", "cap", "iiq", "rwz", nullptr
        };
        OIIO::declare_imageio_format("raw", raw_input_imageio_create,
                                     extensions, nullptr, nullptr,
                                     raw_imageio_library_version());
    });
}

}
