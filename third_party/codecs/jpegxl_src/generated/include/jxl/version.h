/* Repository-owned static-build expansion of libjxl 0.12.0 jxl/version.h.in. */
#ifndef JXL_VERSION_H_
#define JXL_VERSION_H_

#define JPEGXL_MAJOR_VERSION 0
#define JPEGXL_MINOR_VERSION 12
#define JPEGXL_PATCH_VERSION 0

#define JPEGXL_COMPUTE_NUMERIC_VERSION(major, minor, patch) \
    (((major) << 24) | ((minor) << 16) | ((patch) << 8) | 0)
#define JPEGXL_NUMERIC_VERSION \
    JPEGXL_COMPUTE_NUMERIC_VERSION(JPEGXL_MAJOR_VERSION, \
                                    JPEGXL_MINOR_VERSION, \
                                    JPEGXL_PATCH_VERSION)

#endif
