# libheif_src

Pinned libheif 1.23.5 container/colour-conversion backend for the decode-only HEIF/AVIF slice.

- libheif release: 1.23.5
- exact commit: `413e2a87e6a70b3eccc3a3adc5801179dd2d9e00`
- official release: `https://github.com/strukturag/libheif/releases/tag/v1.23.5`
- source archive SHA-256: `3BE49950C75D3FD9318BA775E8253248DD9E08C61EA955FCA81B6B89C02D6B2B`
- updated: 2026-09-24
- license: LGPL-3.0-or-later
- built-in AV1 decoder: dav1d 1.5.4 (`dav1d_src`)
- built-in HEVC decoder: libde265 1.1.1 (`libde265_src`)
- encoders: none
- dynamic plugin loading: disabled

The package compiles the ordinary libheif core recursively, removes all optional/external plugin implementations and experimental/uncompressed-codec sources, then adds back only the dav1d and libde265 decoder adapters plus the two plugin helper sources that upstream always links into the core.

libheif requires C++20. U++ CLANG/GCC build methods currently supply `COMMON_CPP_OPTIONS` with `-std=c++17`, and the builder appends those method options after package-wide compiler options. The C++20 requirement is therefore attached to the `import.ext` file entry instead of the package-wide `options` section. Imported libheif `.cc` sources inherit that per-file option after the method defaults, so the final effective language mode for those sources is C++20 while the rest of the application remains on the established U++ standard.

No AOM, x265, ffmpeg, JPEG/JPEG2000 codec backend, VVC backend or runtime plugin DLL is part of this slice. Consequently AVIF/HEIC/HEIF are decode-only here. AVIF encoding is a separate future backend milestone rather than being hidden behind an unvalidated generated AOM build.

libheif and libde265 are LGPL-3.0. Static redistribution therefore carries LGPL combined-work/relinking/source obligations. The repository keeps exact source pins and dependency boundaries explicit so downstream distributors can meet those obligations.

The repository compiles plugin_registry_lifetime.cc in place of upstream's
plugin_registry.cc. It includes that unmodified pinned file and adds a destructor
in the same translation unit, after the registry's automatic registration.
This ordering matters: libheif allocates built-in decoder state even when an
application only links it and never calls an image API. The destructor empties
any remaining plugin registries before their containers and plugin-init mutexes
are destroyed. A normal heif_deinit already empties them, so the fallback does
not repeat plugin cleanup or add an unmatched heif_deinit. Keep this adaptation
paired with the exact pin and reassess it on upgrade. imaging_test is the
link-without-initialization regression; HEIF/ImagingIO gates exercise the normal
paired path. All native objects and worker threads must finish before teardown.
