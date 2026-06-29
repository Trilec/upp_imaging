# Toolchain Matrix

## windows-clang-x64

- status: pass with documented split behavior
- U++ build method: `CLANGx64`
- compiler: `clang++ 21.1.1`, target `x86_64-w64-windows-gnu`
- zlib_src: pass (`zlib_src_test` reports `1.3.2`)
- zlib: `Core + zlib` package proof passes, but runtime linkage currently resolves to U++ `plugin/z` (`1.3.1`) on this target
- libpng_src: pass (`libpng_src_test` reports `1.6.58`)
- libpng: pass (`libpng_test` reports `1.6.58`)
- libpng_src round trip: pass
- libpng round trip: pass
- U++ plugin/png round trip: pass with documented partial-alpha RGB caveat in the `Image` path
- imath_src: pending
- imath: pending

Notes:

- `Core` on Windows already links U++ `plugin/z`.
- The `zlib` package therefore reuses `plugin/z` linkage on this target to avoid duplicate static symbols in `Core + zlib` builds.
- `zlib_src_test` is the required proof for imported upstream 1.3.2 linkage.
- `libpng_src` depends on `zlib_src`, not `zlib`.
- `libpng` currently compiles imported libpng sources against `zlib` and does not use `plugin/png`.
- `plugin/png` is available as an independent U++ comparison path and uses the U++ image/raster stack.
- `imath_src` is independent of OpenEXR and is the first OpenEXR-branch preflight package.

## Future targets

- `linux-clang-x64`
- `macos-clang-arm64`
- `macos-clang-x64`
