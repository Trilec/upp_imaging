# libheif_src

Pinned libheif 1.23.1 container/colour-conversion backend for the first HEIF/AVIF import slice.

- libheif release: 1.23.1
- exact commit: `2c4bbb54c2738d4a5efbbe3e5fa1d5d76bb88eb0`
- license: LGPL-3.0-or-later
- built-in AV1 decoder: dav1d 1.5.4 (`dav1d_src`)
- built-in HEVC decoder: libde265 1.1.1 (`libde265_src`)
- encoders: none
- dynamic plugin loading: disabled

The package compiles the ordinary libheif core recursively, removes all optional/external plugin implementations and experimental/uncompressed-codec sources, then adds back only the dav1d and libde265 decoder adapters plus the two plugin helper sources that upstream always links into the core.

No AOM, x265, ffmpeg, JPEG/JPEG2000 codec backend, VVC backend or runtime plugin DLL is part of this slice. Consequently AVIF/HEIC/HEIF are decode-only here. AVIF encoding is a separate future backend milestone rather than being hidden behind an unvalidated generated AOM build.

libheif and libde265 are LGPL-3.0. Static redistribution therefore carries LGPL combined-work/relinking/source obligations. The repository keeps exact source pins and dependency boundaries explicit so downstream distributors can meet those obligations.
