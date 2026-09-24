#ifndef WEBP_CONFIG_H_
#define WEBP_CONFIG_H_

#define PACKAGE "libwebp"
#define PACKAGE_NAME "libwebp"
#define PACKAGE_STRING "libwebp 1.6.0"
#define PACKAGE_TARNAME "libwebp"
#define PACKAGE_VERSION "1.6.0"
#define VERSION "1.6.0"

#define WEBP_NEAR_LOSSLESS 1
#define WEBP_USE_THREAD 1

/* SIMD is deliberately disabled in the first portable U++ backend slice.
   The generic scalar codec remains complete; optimized architecture-specific
   objects can be added later as a measured acceleration task. */

#endif
