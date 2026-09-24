/* Repository-generated from Expat 2.8.5 expat_config.h.cmake.
 * Validated target: Windows x64 / U++ CLANGx64 static library.
 * The POSIX builds need their own probed configuration; do not reuse this one.
 */
#ifndef EXPAT_CONFIG_H
#define EXPAT_CONFIG_H 1

#if !defined(_WIN64) || (!defined(__x86_64__) && !defined(_M_X64))
#error Expat configuration is generated for Windows x64 only
#endif

#define BYTEORDER 1234
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1

#define PACKAGE "expat"
#define PACKAGE_BUGREPORT "https://github.com/libexpat/libexpat/issues"
#define PACKAGE_NAME "expat"
#define PACKAGE_STRING "expat 2.8.5"
#define PACKAGE_TARNAME "expat"
#define PACKAGE_URL ""
#define PACKAGE_VERSION "2.8.5"
#define VERSION "2.8.5"
#define STDC_HEADERS 1

#define XML_CONTEXT_BYTES 1024
#define XML_DTD 1
#define XML_GE 1
#define XML_NS 1
#define XML_LARGE_SIZE 1

/* Windows entropy is supplied by Expat's rand_s provider. */

#endif /* EXPAT_CONFIG_H */
