# webp_src

`webp_src` is the repository-pinned libwebp backend for static OpenImageIO WebP support.

## Source boundary

- libwebp release: 1.6.0
- upstream commit: `4fa21912338357f89e4fd51cf2368325b59e9bd9`
- source lives in the `upstream` git submodule
- decoder, encoder, SharpYUV, mux and demux sources are compiled directly through U++
- no system WebP DLL/import library and no CMake build step are required

The first package deliberately compiles libwebp's complete scalar implementation. Architecture-specific SSE/AVX/NEON accelerators are disabled by a repository-owned generated `src/webp/config.h`; they can be introduced later as a measured optimization without changing codec semantics. Thread support and near-lossless encoder capability remain enabled.

Application code should normally use `OpenImageIO` or `ImagingIO`; this backend package exists to own the exact codec dependency and to support focused prerequisite tests.
