#pragma once

#if defined(_WIN32) || defined(__linux__)
#define HAVE_MALLOC_H 1
#endif

/* Scalar first slice: HAVE_SSE4_1, HAVE_AVX2, HAVE_AVX512,
   HAVE_ARM32, HAVE_ARM64, HAVE_NEON and HAVE_POSIX_MEMALIGN are
   intentionally left undefined, matching CMake's #cmakedefine semantics. */
