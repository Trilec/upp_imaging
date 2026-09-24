# libde265_src

Pinned scalar libde265 HEVC decoder backend used by the HEIF/HEIC import stack.

- release: 1.1.1
- exact commit: `4dd701fffac01632ffd5cabc5ef10deb56accba1`
- license: LGPL-3.0-or-later
- encoder: disabled/not compiled
- SIMD: disabled in the first portable U++ source slice

The package compiles the upstream fallback decoder source list plus the Windows condition-variable shim. Repository-owned `config.h` and `libde265/de265-version.h` replace CMake-generated headers. `LIBDE265_STATIC_BUILD` keeps the public ABI from declaring DLL imports/exports.

Because libde265 is LGPL-3.0, redistribution of a statically linked application has LGPL combined-work/relinking obligations. This repository keeps the exact source pin and license provenance explicit; downstream distributors must satisfy those terms. No x265 or other HEVC encoder is part of this package.
