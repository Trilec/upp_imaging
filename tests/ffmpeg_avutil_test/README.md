# ffmpeg_avutil_test

Focused linked-runtime contract for the pinned FFmpeg 9.0.1 scalar `libavutil` package.

It verifies runtime/header version identity, LGPL/configuration reporting, disabled SIMD dispatch, buffer/frame allocation, rational rescaling, the Win32 time path, aligned allocation, UTF-8 file opening/mapping and deterministic cleanup.

Expected result: `SUMMARY passed=13 failed=0`.
