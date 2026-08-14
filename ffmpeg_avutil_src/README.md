# ffmpeg_avutil_src

Pinned FFmpeg 9.0.1 `libavutil` implementation for the first U++ media decode stack.

## Boundary

- exact upstream: `ffmpeg_headers/upstream` at `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`
- static, LGPL-only configuration
- Windows x86_64 / U++ CLANGx64
- scalar C path: external assembly and inline assembly are disabled
- no threads, network, hardware contexts or external libraries in this bring-up
- explicit source manifest reconstructed from the pinned `libavutil/Makefile`; no recursive source glob

The package includes the full upstream base `libavutil` object set, the no-Vulkan hardware-context stub, the static swscale half-float helper and the x86 CPU source required by the upstream x86 library boundary. With assembly disabled, x86 dispatch remains scalar while architecture identity and alignment stay truthful.

Generated-equivalent build configuration is owned by `ffmpeg_headers/generated`. It is reviewed together with the selected source manifests when the FFmpeg pin or enabled component set changes.
