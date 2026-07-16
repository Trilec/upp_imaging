#pragma once

#define UPP_IMAGING_LOCAL_OPENIMAGEIO_INCLUDE 1

#include <openimageio_src/OIIO.h>

#include <string>

#ifdef LoadImage
#undef LoadImage
#endif

namespace UppImaging {

void InitializeOpenImageIO();
bool LoadImage(const char* filename, OIIO::ImageBuf& destination,
               std::string* error = nullptr);
bool SaveImage(const char* filename, const OIIO::ImageBuf& source,
               std::string* error = nullptr);

}
