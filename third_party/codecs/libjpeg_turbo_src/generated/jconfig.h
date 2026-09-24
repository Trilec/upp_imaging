/* Derived from the official libjpeg-turbo 3.2.0 template.
 * Target: Windows CLANGx64 scalar build.
 * JPEG API 62; arithmetic coding enabled; SIMD/TurboJPEG/tools disabled.
 */

/* Version ID for the JPEG library.
 * Might be useful for tests like "#if JPEG_LIB_VERSION >= 60".
 */
#define JPEG_LIB_VERSION  62

/* libjpeg-turbo version */
#define LIBJPEG_TURBO_VERSION  "3.2.0"

/* libjpeg-turbo version in integer form */
#define LIBJPEG_TURBO_VERSION_NUMBER  3002000

/* Support arithmetic encoding when using 8-bit samples */
#define C_ARITH_CODING_SUPPORTED 1

/* Support arithmetic decoding when using 8-bit samples */
#define D_ARITH_CODING_SUPPORTED 1

/* Support in-memory source/destination managers */
#define MEM_SRCDST_SUPPORTED  1

/* Use accelerated SIMD routines when using 8-bit samples */
/* #undef WITH_SIMD */

/* This version of libjpeg-turbo supports run-time selection of data precision,
 * so BITS_IN_JSAMPLE is no longer used to specify the data precision at build
 * time. However, some downstream software expects the macro to be defined.
 */
#ifndef BITS_IN_JSAMPLE
#define BITS_IN_JSAMPLE  8
#endif

#ifdef _WIN32

#undef RIGHT_SHIFT_IS_UNSIGNED

/* Define "boolean" as unsigned char, not int, per Windows custom */
#ifndef __RPCNDR_H__
typedef unsigned char boolean;
#endif
#define HAVE_BOOLEAN

/* Define "INT32" as int, not long, per Windows custom */
#if !(defined(_BASETSD_H_) || defined(_BASETSD_H))
typedef short INT16;
typedef signed int INT32;
#endif
#define XMD_H

#else

/* #undef RIGHT_SHIFT_IS_UNSIGNED */

#endif
