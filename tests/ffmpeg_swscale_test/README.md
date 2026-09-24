# ffmpeg_swscale_test

Focused linked-runtime contract for the pinned FFmpeg 9.0.1 scalar `libswscale` package.

A 2x2 limited-range ITU-601 YUV420P fixture with neutral chroma is converted to packed RGBA8. The test verifies runtime/config/license identity, explicit colour-range policy, complete conversion, opaque alpha, neutral grayscale, black/white endpoints, deterministic midtones and monotonic output.

Expected result: `SUMMARY passed=13 failed=0`.
