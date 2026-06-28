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
