#ifndef _openimageio_plugin_tiff_AllocationPolicy_h_
#define _openimageio_plugin_tiff_AllocationPolicy_h_

#include <tiffio.h>
#include <OpenImageIO/imageio.h>
#include <algorithm>

namespace UppImaging {
// Native allocations owned by one TIFF handle. Consumers may lower these
// budgets; OIIO/C++ scratch and other decoders are separate boundaries.
inline TIFFOpenOptions* TiffReadOptions()
{
    TIFFOpenOptions* options = TIFFOpenOptionsAlloc();
    if(!options) return nullptr;
    int single = std::clamp(OIIO::get_int_attribute("limits:imagesize_MB", 256), 1, 256);
    int total = single * 2;
    TIFFOpenOptionsSetMaxSingleMemAlloc(options,
        (tmsize_t)std::clamp(single, 1, 256) * 1024 * 1024);
    TIFFOpenOptionsSetMaxCumulatedMemAlloc(options,
        (tmsize_t)std::clamp(total, 1, 512) * 1024 * 1024);
    return options;
}

inline TIFF* TiffReadClient(const char* name, const char* mode, thandle_t handle,
    TIFFReadWriteProc read, TIFFReadWriteProc write, TIFFSeekProc seek,
    TIFFCloseProc close, TIFFSizeProc size, TIFFMapFileProc map,
    TIFFUnmapFileProc unmap)
{
    TIFFOpenOptions* options = TiffReadOptions();
    if(!options) return nullptr;
    TIFF* result = TIFFClientOpenExt(name, mode, handle, read, write, seek,
                                   close, size, map, unmap, options);
    TIFFOpenOptionsFree(options);
    return result;
}
}

#endif
