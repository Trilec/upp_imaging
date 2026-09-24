# FFmpeg

Stable application-facing direct native FFmpeg package for the pinned 9.0.1 source build.

The package forwards the standard FFmpeg C API and links the repository-owned scalar `libavutil`, native H.264 `libavcodec`, MOV/local-file `libavformat`, and scalar `libswscale` boundaries. It does not invent replacement FFmpeg types.

Initial supported slice: local MOV/MP4 input, native H.264 video decode, and decoded-frame conversion to packed RGBA8. Audio, network protocols, filters, devices, encoders, muxers, external codecs, SIMD assembly, and hardware acceleration remain outside this slice.
