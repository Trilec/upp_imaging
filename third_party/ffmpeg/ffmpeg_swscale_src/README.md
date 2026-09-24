# ffmpeg_swscale_src

Pinned FFmpeg 9.0.1 scalar `libswscale` implementation for the first U++ video decode stack.

The package contains the exact 22 upstream base libswscale C objects plus the three x86 C dispatch objects. External/inline assembly is disabled by the shared generated configuration, so no `.asm` objects or generated x86 assembly macros are required. The shared-library duplicate helpers `log2_tab` and `half2float` are already supplied once by the static `ffmpeg_avutil_src` boundary and are not duplicated here.

Initial purpose: deterministic decoded-frame conversion to packed RGBA8. No Vulkan/unstable scaling operations are enabled.
