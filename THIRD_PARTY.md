# Third-Party Code

## zlib

- Upstream name: zlib
- Upstream version: 1.3.2
- Upstream authors: Jean-loup Gailly and Mark Adler
- Source archive used: `https://zlib.net/zlib-1.3.2.tar.gz`
- SHA-256: `BB329A0A2CD0274D05519D61C667C062E06990D72E125EE2DFA8DE64F0119D16`
- License: zlib License
- Date imported: 2026-06-28
- Files modified after import: No upstream files modified

Notes:

- The imported upstream source tree is preserved under `zlib_src/upstream/`.
- `zlib_src` is the strict package that compiles imported upstream zlib 1.3.2 sources directly.
- `zlib` is a compatibility package. On Windows/Core builds it can reuse U++ `plugin/z` linkage to avoid duplicate static zlib symbols, because `Core` already links `plugin/z` on Windows.

## libpng

- Upstream name: libpng
- Upstream version: 1.6.58
- Upstream authors: Guy Eric Schalnat, Andreas Dilger, Glenn Randers-Pehrson, Cosmin Truta, John Bowler, and other libpng contributors
- Source archive used: `https://download.sourceforge.net/libpng/libpng-1.6.58.tar.xz`
- SHA-256: `28EB403F51F0F7405249132CECFE82EA5C0EF97F1B32C5A65828814AE0D34775`
- License: PNG Reference Library License version 2
- Date imported: 2026-06-28
- Files modified after import: No upstream files modified

Notes:

- The imported upstream source tree is preserved under `libpng_src/upstream/`.
- `libpng_src` is the strict package that compiles imported upstream libpng 1.6.58 sources directly.
- `libpng_src` depends on `zlib_src`.
- The release-provided file `scripts/pnglibconf.h.prebuilt` was copied into `libpng_src/upstream/pnglibconf.h` to avoid local config generation.
- A local bridge header `libpng_src/upstream/zlib.h` was added so the imported libpng sources can include zlib from `zlib_src` without modifying upstream libpng files.
- `libpng` is the current user-facing package and compiles the same imported libpng sources against `zlib` for compatibility with normal U++ applications.

## Imath

- Upstream name: Imath
- Upstream version: 3.2.2
- Upstream authors: Contributors to the OpenEXR Project / Academy Software Foundation
- Source archive used: `https://github.com/AcademySoftwareFoundation/Imath/archive/refs/tags/v3.2.2.tar.gz`
- SHA-256: `B4275D83FB95521510E389B8D13AF10298ED5BED1C8E13EFD961D91B1105E462`
- License: BSD-3-Clause
- Date imported: 2026-06-28
- Files modified after import: No upstream files modified

Notes:

- The imported upstream source tree is preserved under `imath_src/upstream/`.
- `imath_src` is the strict package that compiles imported upstream Imath 3.2.2 sources directly.
- `imath` is the current user-facing package and delegates to `imath_src`.
- A repository-owned generated config header `imath_src/upstream/ImathConfig.h` was created from upstream `config/ImathConfig.h.in` using the upstream release defaults needed for this local static package build.
- OpenEXR is not included in this import.

## OpenEXR

- Upstream name: OpenEXR
- Upstream version inspected: 3.4.13
- Upstream authors: Contributors to the OpenEXR Project / Academy Software Foundation
- Source archive inspected: `https://github.com/AcademySoftwareFoundation/openexr/archive/refs/tags/v3.4.13.tar.gz`
- SHA-256: `1ED0CEE48AC8C77DA235C8CA8AB85D031D43CD790EDA36AF87FED4CF316CF2DF`
- License: BSD-3-Clause
- Date inspected: 2026-06-28
- Files modified after import: No imported upstream files modified in the preflight subset

Notes:

- This task does not add full OpenEXR support.
- A minimal public/header subset was imported into `openexr_src/upstream/` for compile-time probing only.
- A repository-owned generated config header `openexr_src/upstream/OpenEXRConfig.h` was created from upstream `cmake/OpenEXRConfig.h.in` using values derived from the inspected release.
- Full OpenEXR packaging will require additional libraries and generated config headers, especially for `Iex`, `IlmThread`, and `OpenEXRCore`.

## Iex

- Upstream name: Iex
- Upstream version inspected from OpenEXR release: 3.4.13 source tree
- Source origin: `openexr-3.4.13/src/lib/Iex`
- Files modified after import: Yes

Notes:

- `iex_src` packages the lower OpenEXR exception/error layer.
- Repository-owned generated headers `IexConfig.h` and `IexConfigInternal.h` were added from upstream templates / local static-package assumptions.
- Imported file `iex_src/upstream/IexMathFpu.cpp` was minimally adjusted to use quoted local config-header includes for the U++ package include model.

## IlmThread

- Upstream name: IlmThread
- Upstream version inspected from OpenEXR release: 3.4.13 source tree
- Source origin: `openexr-3.4.13/src/lib/IlmThread`
- Files modified after import: No imported upstream source files modified

Notes:

- `ilmthread_src` packages the lower OpenEXR threading layer.
- Repository-owned generated header `IlmThreadConfig.h` was added from upstream template / local Windows static-package assumptions.
