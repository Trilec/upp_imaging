# Licenses

This repository contains original repository content plus imported third-party code.

## Repository license

The repository-level license is in `LICENSE`.

## zlib

The `zlib_src/` package includes upstream zlib source code under the zlib License.
The upstream license text is preserved in `zlib_src/upstream/LICENSE`.

## libpng

The `libpng_src/` package includes upstream libpng source code under the PNG Reference Library License.
The upstream license text is preserved in `libpng_src/upstream/LICENSE`.

## Imath

The `imath_src/` package includes upstream Imath source code under the BSD-3-Clause license.
The upstream license text is preserved in `imath_src/upstream/LICENSE.md`.

## JPEG XL backend

The `jpegxl_src/upstream` submodule pins libjxl 0.12.0, distributed under BSD-3-Clause with the upstream PATENTS grant. Its recursively pinned build dependencies used by `jpegxl_src` are Brotli (MIT), Highway (Apache-2.0/BSD-3-Clause dual license), and skcms (BSD-3-Clause). Their upstream license files remain inside the recursively initialized source tree.

See `THIRD_PARTY.md` for import details.
