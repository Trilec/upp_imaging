// Generated from upstream OpenColorIO 2.5.2 include/OpenColorIO/OpenColorABI.h.in
// Target: Windows CLANGx64 static library packaging
// Generation: checked in for U++ packaging; OpenColorIO_SKIP_IMPORTS is forced on

#ifndef INCLUDED_OCIO_OPENCOLORABI_H
#define INCLUDED_OCIO_OPENCOLORABI_H

#define OCIO_NAMESPACE OpenColorIO_v2_5

#define OCIO_VERSION_STR        "2.5.2"
#define OCIO_VERSION_STATUS_STR ""
#define OCIO_VERSION_FULL_STR   "2.5.2"
#define OCIO_VERSION            "2.5.2"

#define OCIO_VERSION_HEX ((2 << 24) | (5 << 16) | (2 << 8))

#define OCIO_VERSION_MAJOR 2
#define OCIO_VERSION_MINOR 5

#include <memory>
#define OCIO_SHARED_PTR std::shared_ptr
#define OCIO_DYNAMIC_POINTER_CAST std::dynamic_pointer_cast

#ifndef OCIO_DEPRECATED
#if defined(__cplusplus) && __cplusplus >= 201402L
#define OCIO_DEPRECATED(msg) [[deprecated(msg)]]
#else
#define OCIO_DEPRECATED(msg)
#endif
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#ifndef OpenColorIO_SKIP_IMPORTS
#define OpenColorIO_SKIP_IMPORTS 1
#endif
#define OCIOEXPORT
#define OCIOHIDDEN
#elif defined __GNUC__
#define OCIOEXPORT __attribute__ ((visibility("default")))
#define OCIOHIDDEN __attribute__ ((visibility("hidden")))
#else
#define OCIOEXPORT
#define OCIOHIDDEN
#endif

#endif
