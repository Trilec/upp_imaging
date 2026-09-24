# Licenses

This repository contains original repository content plus imported third-party code.

## Repository license

The repository-level license is in `LICENSE`.

## libheif

`third_party/codecs/libheif_src/upstream/` is pinned to libheif 1.23.5
under LGPL-3.0-or-later. The original license and copyright notices are
preserved in the submodule, including `COPYING`. Static redistribution
requires review of the LGPL combined-work, relinking and source obligations.
The generated version header and registry-lifetime overlay are repository
integration files and do not alter the upstream license.

## Expat

`third_party/support/expat_src/upstream/` contains Expat 2.8.5 source
under its MIT/X Consortium license. The complete upstream notice is
preserved in `third_party/support/expat_src/upstream/COPYING`.
The generated Windows configuration header is repository-owned.

## OpenImageIO

`third_party/openimageio/openimageio_plugins_src/upstream/` pins
OpenImageIO 3.1.17.0 at
`73bc189f7d8469a9760ce9c5099b686c77695074` under Apache-2.0.
The copied main, utility and public-header source slices retain upstream
notices; the original license is in the pinned upstream tree. Local
MinGW error-storage wrappers and include adaptations remain separate
integration changes.

## zlib

The `zlib_src/` package includes upstream zlib source code under the zlib License.
The upstream license text is preserved in `third_party/codecs/zlib_src/upstream/LICENSE`.

## libpng

The `libpng_src/` package includes upstream libpng source code under the PNG Reference Library License.
The upstream license text is preserved in `third_party/codecs/libpng_src/upstream/LICENSE`.

## Imath

The `imath_src/` package includes upstream Imath source code under the BSD-3-Clause license.
The upstream license text is preserved in `third_party/imath/imath_src/upstream/LICENSE.md`.

## JPEG XL backend

The `third_party/codecs/jpegxl_src/upstream` submodule pins libjxl 0.12.0, distributed under BSD-3-Clause with the upstream PATENTS grant. Its recursively pinned build dependencies used by `jpegxl_src` are Brotli (MIT), Highway (Apache-2.0/BSD-3-Clause dual license), and skcms (BSD-3-Clause). Their upstream license files remain inside the recursively initialized source tree.

See `THIRD_PARTY.md` for import details.
