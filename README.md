# upp_imaging

`upp_imaging` is a reusable U++ imaging-library nest intended to grow as a set of independently usable packages.

The first package is `zlib`.

Goals:

- keep packages reusable and independently usable
- keep package dependencies minimal
- avoid committing prebuilt binaries or generated build output
- validate the U++ `umk` + CLANG toolchain path first

This repository is separate from `upp_lumapix`.
LumaPix may consume this nest later, but this repo is not LumaPix-specific.

## Current packages

- `zlib`: upstream zlib 1.3.2 packaged for U++
- `zlib_test`: minimal packaging/linkage proof using `Core` + `zlib`

## Layout

- `zlib/`: reusable U++ package and imported upstream source
- `zlib_test/`: minimal proof executable
- `docs/`: package, toolchain, upstream, and license policy notes

## Notes

- no prebuilt binaries are committed
- build output belongs under `out/` and is ignored
- CLANG is the first validated toolchain target
