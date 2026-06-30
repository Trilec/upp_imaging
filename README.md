# upp_imaging

`upp_imaging` is a reusable U++ imaging-library nest intended to grow as a set of independently usable packages.

The first packages are `zlib_src`, `zlib`, `libpng_src`, `libpng`, `imath_src`, `imath`, `libdeflate_src`, `libdeflate`, `iex_src`, and `ilmthread_src`.

Goals:

- keep packages reusable and independently usable
- keep package dependencies minimal
- avoid committing prebuilt binaries or generated build output
- validate the U++ `umk` + CLANG toolchain path first

This repository is separate from `upp_lumapix`.
LumaPix may consume this nest later, but this repo is not LumaPix-specific.

## Current packages

- `zlib_src`: strict upstream zlib 1.3.2 source package
- `zlib`: user-facing compatibility package with stable include path
- `zlib_src_test`: strict imported-source linkage proof
- `zlib_test`: `Core` + `zlib` compatibility proof, which currently resolves to U++ `plugin/z` on Windows/Core
- `libpng_src`: strict upstream libpng 1.6.58 source package layered on `zlib_src`
- `libpng`: user-facing libpng package with stable include path, built against `zlib`
- `libpng_src_test`: strict imported-source libpng linkage proof
- `libpng_test`: user-facing libpng package proof
- `libpng_src_roundtrip_test`: strict imported-source libpng memory round-trip proof
- `libpng_roundtrip_test`: user-facing libpng memory round-trip proof
- `upp_png_plugin_test`: U++ `plugin/png` comparison round-trip proof
- `imath_src`: strict upstream Imath 3.2.2 source package
- `imath`: user-facing Imath package with stable include path
- `imath_src_test`: strict imported-source Imath linkage proof
- `imath_test`: user-facing Imath package proof
- `libdeflate_src`: strict standalone libdeflate 1.25 source package
- `libdeflate`: user-facing libdeflate package with stable include path
- `libdeflate_src_test`: strict imported-source libdeflate package proof
- `libdeflate_test`: user-facing libdeflate package proof
- `iex_src`: strict imported-source Iex lower-layer package
- `iex_src_test`: Iex exception/runtime probe
- `ilmthread_src`: strict imported-source IlmThread lower-layer package
- `ilmthread_src_test`: IlmThread runtime/semaphore probe
- `openexr_src`: OpenEXR preflight header/probe skeleton only
- `openexr_src_probe`: minimal OpenEXR compile probe

## Layout

- `zlib_src/`: strict upstream-source package and imported upstream source
- `zlib/`: compatibility package layered on top of either `zlib_src` or U++ `plugin/z`
- `zlib_src_test/`: strict imported-source proof executable
- `zlib_test/`: normal U++ compatibility proof executable
- `libpng_src/`: strict upstream-source libpng package and imported upstream source
- `libpng/`: user-facing libpng package layered on top of `zlib`
- `libpng_src_test/`: strict imported-source libpng proof executable
- `libpng_test/`: user-facing libpng proof executable
- `libpng_src_roundtrip_test/`: strict imported-source libpng encode/decode test
- `libpng_roundtrip_test/`: user-facing libpng encode/decode test
- `upp_png_plugin_test/`: U++ `plugin/png` comparison encode/decode test
- `imath_src/`: strict upstream-source Imath package and imported upstream source
- `imath/`: user-facing Imath package layered on top of `imath_src`
- `imath_src_test/`: strict imported-source Imath proof executable
- `imath_test/`: user-facing Imath proof executable
- `libdeflate_src/`: strict standalone libdeflate package and imported upstream source
- `libdeflate/`: user-facing libdeflate package layered on top of `libdeflate_src`
- `libdeflate_src_test/`: strict imported-source libdeflate proof executable
- `libdeflate_test/`: user-facing libdeflate proof executable
- `iex_src/`: strict imported-source Iex lower-layer package
- `iex_src_test/`: Iex exception/runtime probe executable
- `ilmthread_src/`: strict imported-source IlmThread lower-layer package
- `ilmthread_src_test/`: IlmThread runtime/semaphore probe executable
- `openexr_src/`: OpenEXR preflight package skeleton
- `openexr_src_probe/`: OpenEXR compile-only preflight probe
- `docs/`: package, toolchain, upstream, and license policy notes

## Notes

- no prebuilt binaries are committed
- build output belongs under `out/` and is ignored
- CLANG is the first validated toolchain target
- on Windows with `Core`, `zlib` may reuse U++ `plugin/z`; use `zlib_src_test` to validate imported upstream linkage
- `libpng` currently compiles imported libpng 1.6.58 against `zlib` and does not use U++ `plugin/png`
- `openexr_core_src` is still deferred
- full EXR read/write is not implemented yet
