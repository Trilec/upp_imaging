# ffmpeg_headers_test

Compile-contract test for the pinned FFmpeg 9.0.1 public-header boundary.

The test links no FFmpeg implementation objects. It verifies that the repository-owned public `avconfig.h`, the four initial media-library header families, their expected release majors, and the core public context types can coexist in one U++ CLANGx64 translation unit.

Expected result: `SUMMARY passed=7 failed=0`.
