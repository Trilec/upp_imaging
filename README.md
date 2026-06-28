# upp_imaging

`upp_imaging` is a reusable U++ imaging-library nest intended to grow as a set of independently usable packages.

The first packages are `zlib_src` and `zlib`.

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

## Layout

- `zlib_src/`: strict upstream-source package and imported upstream source
- `zlib/`: compatibility package layered on top of either `zlib_src` or U++ `plugin/z`
- `zlib_src_test/`: strict imported-source proof executable
- `zlib_test/`: normal U++ compatibility proof executable
- `docs/`: package, toolchain, upstream, and license policy notes

## Notes

- no prebuilt binaries are committed
- build output belongs under `out/` and is ignored
- CLANG is the first validated toolchain target
- on Windows with `Core`, `zlib` may reuse U++ `plugin/z`; use `zlib_src_test` to validate imported upstream linkage
