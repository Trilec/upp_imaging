// Copyright Contributors to the OpenImageIO project.
// SPDX-License-Identifier: Apache-2.0
// https://github.com/AcademySoftwareFoundation/OpenImageIO

#ifndef OPENIMAGEIO_VERSION_H
#define OPENIMAGEIO_VERSION_H

#define OIIO_VERSION_MAJOR 3
#define OIIO_VERSION_MINOR 1
#define OIIO_VERSION_PATCH 15
#define OIIO_VERSION_TWEAK 0
#define OIIO_VERSION_RELEASE_TYPE 

#define OIIO_STRINGIZE_HELPER(a) #a
#define OIIO_STRINGIZE(a) OIIO_STRINGIZE_HELPER(a)

#define OIIO_MAKE_VERSION(major,minor,patch) \
                        (10000 * (major) + 100 * (minor) + (patch))

#define OIIO_VERSION OIIO_MAKE_VERSION(OIIO_VERSION_MAJOR, \
                        OIIO_VERSION_MINOR, OIIO_VERSION_PATCH)

#define OIIO_VERSION_GREATER_EQUAL(major,minor,patch) \
                        OIIO_VERSION >= OIIO_MAKE_VERSION(major,minor,patch)

#define OIIO_VERSION_LESS(major,minor,patch) \
                        OIIO_VERSION < OIIO_MAKE_VERSION(major,minor,patch)

#define OPENIMAGEIO_VERSION OIIO_VERSION

#define OIIO_MAKE_VERSION_STRING2(a,b,c,d,e) #a "." #b "." #c "." #d #e
#define OIIO_MAKE_VERSION_STRING(a,b,c,d,e) OIIO_MAKE_VERSION_STRING2(a,b,c,d,e)
#define OIIO_VERSION_STRING \
    OIIO_MAKE_VERSION_STRING(OIIO_VERSION_MAJOR, \
                             OIIO_VERSION_MINOR, OIIO_VERSION_PATCH, \
                             OIIO_VERSION_TWEAK, OIIO_VERSION_RELEASE_TYPE)
#define OIIO_INTRO_STRING "OpenImageIO " OIIO_VERSION_STRING " http://www.openimageio.org"

#define OIIO_VERSION_STRING_MMPT \
    OIIO_MAKE_VERSION_STRING(OIIO_VERSION_MAJOR, \
                             OIIO_VERSION_MINOR, OIIO_VERSION_PATCH, \
                             OIIO_VERSION_TWEAK, "")

#ifndef OIIO_DISABLE_DEPRECATED
#    define OIIO_DISABLE_DEPRECATED 0
#endif

#define OIIO_OUTER_NAMESPACE OpenImageIO
#define OIIO_CURRENT_INNER_NAMESPACE v3_1

namespace OpenImageIO {
    inline namespace v3_1 { }
    namespace v3_0 { }
}
namespace OIIO = OpenImageIO;

#define OIIO_NAMESPACE_BEGIN namespace OpenImageIO { inline namespace v3_1 {
#define OIIO_NAMESPACE_END } }
#define OIIO_NAMESPACE_USING using namespace OIIO;
#define OIIO_CURRENT_NAMESPACE OpenImageIO::v3_1

#include <OpenImageIO/nsversions.h>

#define OIIO_PLUGIN_VERSION 27

#define OIIO_PLUGIN_NAMESPACE_BEGIN OIIO_NAMESPACE_BEGIN
#define OIIO_PLUGIN_NAMESPACE_END OIIO_NAMESPACE_END

#ifdef EMBED_PLUGINS
#define OIIO_PLUGIN_EXPORTS_BEGIN
#define OIIO_PLUGIN_EXPORTS_END
#else
#define OIIO_PLUGIN_EXPORTS_BEGIN extern "C" {
#define OIIO_PLUGIN_EXPORTS_END }
#endif

#define OIIO_USING_IMATH_VERSION_MAJOR 3
#define OIIO_USING_IMATH_VERSION_MINOR 2

#define OIIO_BUILD_CPP 17
#define OIIO_BUILD_CPP11 (OIIO_BUILD_CPP >= 11)
#define OIIO_BUILD_CPP14 (OIIO_BUILD_CPP >= 14)
#define OIIO_BUILD_CPP17 (OIIO_BUILD_CPP >= 17)
#define OIIO_BUILD_CPP20 (OIIO_BUILD_CPP >= 20)

#endif /* defined(OPENIMAGEIO_VERSION_H) */
