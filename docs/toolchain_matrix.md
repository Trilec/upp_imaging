# Toolchain Matrix

## windows-clang-x64

- status: pass with documented split behavior
- U++ build method: `CLANGx64`
- compiler: `clang++ 21.1.1`, target `x86_64-w64-windows-gnu`
- zlib_src: pass (`zlib_src_test` reports `1.3.2`)
- zlib: `Core + zlib` package proof passes and resolves to U++ `plugin/z` (`1.3.1`) on this target
- libpng_src: pass (`libpng_src_test` reports `1.6.58`)
- libpng: pass (`libpng_test` reports `1.6.58`)
- libpng_src round trip: pass
- libpng round trip: pass
- U++ plugin/png round trip: pass with documented partial-alpha RGB caveat in the `Image` path
- imath_src: pass (`imath_src_test` reports version 3.2.2, half conversion, V3f math, and Box/M44 smoke)
- imath: pass (`imath_test` reports user-facing wrapper OK, half conversion, V3f math, and Box smoke)
- libdeflate_src: pass (`libdeflate_src_test` allocates compressor/decompressor and performs zlib-wrapper compress/decompress round trip)
- libdeflate: pass (`libdeflate_test` performs the same round trip through Core + libdeflate)
- openjph_src: pass (`openjph_src_test` checks version macros and basic public types through the strict package)
- openjph: pass (`openjph_test` checks version macros and basic public types through Core + openjph)
- openexr_src preflight: pass (`openexr_src_probe` reports version 3.4.13 and namespace/header probe OK)
- openexr_core_src attempt: pass (`openexr_core_src_probe` reports version 3.4.13, public headers OK, and compression enums OK)
- iex_src: pass (`iex_src_test` throws and catches IEX_NAMESPACE::ArgExc through BaseExc)
- ilmthread_src: pass (`ilmthread_src_test` reports supportsThreads() and verifies semaphore post/wait)
- opencolorio_src: pass (`opencolorio_src_test` reports source boundary, CPU/GPU coverage, builtin registry checks, and headless monitor policy)
- opencolorio: pass (`opencolorio_test` reports stable boundary and the same CPU/GPU coverage)
- opencolorio_gui_link_test: pass with `upp_Ui` and `upp_AnimationEasing`

Notes:

- `Core` on Windows already links U++ `plugin/z`.
- The `zlib` package therefore reuses `plugin/z` linkage on this target to avoid duplicate static symbols in `Core + zlib` builds.
- `zlib_src_test` is the required proof for imported upstream 1.3.2 linkage.
- `zlib` on Windows is the stable provider used by the OCIO foundation and satisfies the minimum zlib version required by the aggregate consumer tests.
- `libpng_src` depends on `zlib_src`, not `zlib`.
- `libpng` currently compiles imported libpng sources against `zlib` and does not use `plugin/png`.
- `plugin/png` is available as an independent U++ comparison path and uses the U++ image/raster stack.
- `imath_src` is independent of OpenEXR and is the first OpenEXR-branch preflight package.
- `libdeflate_src` is the standalone deflate package intended to satisfy future OpenEXRCore ZIP/ZIPS compression needs.
- `openjph_src` is the standalone HTJ2K/OpenJPH package intended to satisfy future OpenEXRCore HTJ2K dependency needs.
- `openexr_core_src` now exists as a first modest package attempt; full EXR read/write remains deferred.
- `opencolorio_src` uses a source-only wrapper, generated builtin blobs, and a headless Windows monitor path.
- `opencolorio` stays a separate stable wrapper so the two boundaries do not blur and start sharing the same shoes.

## Future targets

- `linux-clang-x64`
- `macos-clang-arm64`
- `macos-clang-x64`
