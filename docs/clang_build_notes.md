# CLANG Build Notes

This repository is being validated first with the local U++ `umk` + CLANG toolchain on Windows.

The first expected build method is `CLANGx64`.
Observed local compiler:

- `clang++ 21.1.1`
- target: `x86_64-w64-windows-gnu`

Observed package behavior on Windows:

- `Core` already links `plugin/z`
- a second static zlib package causes duplicate linker symbols
- the current `zlib` package reuses `plugin/z` linkage on this target so `Core + zlib` builds succeed
- `zlib_src` exists specifically to prove the imported upstream source can be compiled and linked without that compatibility layer
- the OCIO foundation intentionally uses the stable Windows provider, not `zlib_src`, so GUI/plugin links stay singular

Current validation result:

- `zlib_src_test` builds without `Core` and reports zlib `1.3.2`
- `zlib_test` builds with `Core + zlib` and reports zlib `1.3.1` on Windows/Core

libpng validation goals:

- `libpng_src_test` should prove imported libpng 1.6.58 linkage on top of `zlib_src`
- `libpng_test` should prove the user-facing include path `<libpng/png.h>`
- this first libpng pass should not use U++ `plugin/png`

Windows/Core note for libpng:

- `libpng_src` links cleanly in the strict no-`Core` test
- `libpng -> libpng_src -> zlib_src` duplicates zlib symbols in `Core` builds on Windows
- `libpng` therefore compiles imported libpng sources against `zlib` for compatibility builds on this target

Round-trip validation goals:

- `libpng_src_roundtrip_test` should prove strict in-memory PNG encode/decode and exact pixel recovery
- `libpng_roundtrip_test` should prove the same behavior through the user-facing package
- `upp_png_plugin_test` should document how the built-in U++ PNG path behaves with premultiplied alpha images

Current round-trip result:

- `libpng_src_roundtrip_test` passes with exact 2x2 RGBA pixel recovery
- `libpng_roundtrip_test` passes with exact 2x2 RGBA pixel recovery
- `upp_png_plugin_test` passes for size, opaque pixels, and partial-alpha preservation of alpha
- the tested U++ `Image`/`plugin/png` path does not preserve the low-level RGB payload of the partial-alpha pixel as an exact channel value

Imath preflight goal:

- `imath_src_test` should prove imported Imath 3.2.2 compiles and links under `umk` + `CLANGx64`
- `imath_test` should prove the user-facing include path works in a normal `Core` executable
- this step does not include OpenEXR yet

OpenEXR preflight goal:

- inspect current upstream OpenEXR packaging complexity before attempting read/write support
- confirm compatibility with local `imath_src` 3.2.2
- keep this task to a minimal compile probe, not a real library implementation
