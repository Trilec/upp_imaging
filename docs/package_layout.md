# Package Layout

This repository is one U++ nest containing independently usable packages.

## Core layout rules

- one repository/nest, many packages;
- each package depends only on what it needs;
- third-party source stays close to the package that owns it;
- application code uses stable public packages, not strict `_src`, generated-header or static-registration packages;
- public package boundaries hide strict-source filesystem layout;
- source manifests are explicit; recursive source globs must not be used to hide missing dependency/source ownership;
- do not link strict and stable implementations of the same underlying library into one ordinary executable;
- generated executables/images belong under ignored output directories;
- machine-specific U++ nest configuration is not committed.

## Dependency direction

```text
pinned / strict upstream implementation
        ↓
stable direct application package
        ↓
backend-neutral Upp::Imaging framework
        ↓
application / diagnostic integration
```

FFmpeg is a parallel stable-direct media stack and does not depend upward into `Upp::Imaging`.

## Compression and image foundations

- `zlib_src` owns pinned upstream zlib source proof.
- `zlib` is the stable compatibility layer; on Windows/Core it resolves through U++ `plugin/z` to avoid duplicate symbols.
- `libpng_src` owns pinned libpng source and strict proof; `libpng` is the stable application route.
- `imath_src` owns pinned Imath source; `imath` is the stable route.
- `libdeflate_src` / `libdeflate` and `openjph_src` / `openjph` follow the same strict/stable split.
- `libjpeg_turbo_src` / `libjpeg_turbo` and `libtiff_src` / `libtiff` keep codec implementation details below their stable application packages.
- `fmt_src` / `fmt` and `robinmap_src` / `robinmap` retain the strict/stable split.

## OpenEXR stack

- `iex_src` owns the exception/error layer.
- `ilmthread_src` owns the threading layer and depends on `iex_src`.
- `openexr_core_src` owns lower-level OpenEXRCore source.
- `openexr_core` is the stable lower-level application package.
- `openexr_src` owns the pinned high-level OpenEXR 3.4.13 implementation and depends on the strict lower layers.
- `openexr` is the stable high-level application package.
- `openexr_io` is the separate narrow U++ scanline-RGBA helper over `openexr_core`.

Strict and stable OpenEXR tests/probes remain separate so one target does not pull duplicate implementations.

## OpenColorIO stack

- `opencolorio_src` owns pinned OpenColorIO 2.5.2 implementation source.
- It depends on its explicit stable prerequisite packages including `expat`, `yaml_cpp`, `pystring`, `minizip_ng`, `imath` and `zlib`.
- `OpenColorIO` is the sole canonical public application package.
- The old lowercase public package name is intentionally not provided on Windows because package paths cannot differ only by case.
- Strict validation belongs to `opencolorio_src_test`; public-boundary validation belongs to `opencolorio_test`; GUI link coverage remains separate.

## OpenImageIO stack

- `openimageio_headers` owns the strict upstream `OpenImageIO/` public-header tree and compiles no implementation source.
- `openimageio_src` and `openimageio_util_src` compile the pinned OpenImageIO 3.1.15.0 implementation.
- static format-registration packages own the selected OpenImageIO plugins and declare every third-party header/dependency they directly consume;
- `OpenImageIO` is the canonical public application package;
- `oiio` is a compatibility forwarder, not a second implementation.

The original Windows-proven OpenEXR/PNG route remains the baseline. Code-side static plugin support now extends through the current still-image format line: JPEG XL, HDR/RGBE, DPX/Cineon, RAW, WebP, decode-only HEIF/AVIF and TIFF. The shared dependency closure repair is `5ca436c3ba6265f6431deaf7348332940051686d`; current accumulated Windows validation is still pending.

Do not describe a format as Windows-accepted through OpenImageIO merely because its plugin/source is present.

## `Upp::Imaging` framework

All framework packages are implemented.

- `ImagingCore` depends only on U++ Core.
- `ImagingIO` depends on `ImagingCore` and `OpenImageIO`; OIIO types remain private.
- `ImagingColor` depends on `ImagingCore` and `OpenColorIO`; OCIO types remain private.
- `ImagingAnalysis` depends only on `ImagingCore` and stays GUI-independent.
- `ImagingDiagnostics` depends only on `ImagingCore` and stays GUI-independent.
- `Imaging` is the convenience umbrella over all five framework packages.
- `Imaging` does not automatically include `plugin/exr` or FFmpeg.

Established Windows framework baseline:

- `ImagingCore` 48/0;
- `ImagingIO` 79/0 baseline;
- `ImagingColor` 66/0 plus independent OCIO 15/0;
- `ImagingAnalysis` 41/0;
- `ImagingDiagnostics` 33/0;
- `Imaging` 6/0.

Later-format `ImagingIO` accumulation remains pending after the static OpenImageIO dependency repair.

## Raster integration

`plugin/exr` is implemented as an opt-in display-oriented `StreamRaster` / `Upp::Image` bridge.

It is deliberately separate from full-fidelity `ImagingIO`, OpenImageIO and OpenEXR APIs. It must remain opt-in and must not silently alter ordinary U++ raster behaviour.

The current focused contract is 22 checks and remains Windows Debug/Release pending.

## FFmpeg stack

FFmpeg is a separate media subsystem.

```text
application
    ↓
FFmpeg
    ├── ffmpeg_avutil_src
    ├── ffmpeg_avcodec_src
    ├── ffmpeg_avformat_src
    └── ffmpeg_swscale_src
            ↓
      ffmpeg_headers
```

Rules:

- `ffmpeg_headers` owns repository-generated equivalent config/public headers and compiles no implementation source;
- implementation packages compile explicit source manifests reconstructed from the exact pinned upstream Makefiles/configure selections;
- `FFmpeg` is the stable direct application package and forwards standard FFmpeg API types;
- generated registries remain checked in but are not compiled as standalone header-package translation units;
- required upstream materializer/duplicate objects are owned explicitly by the implementation package that needs them;
- no recursive source globs;
- first-slice feature policy remains scalar LGPL decode only: no threads/network/external codecs/filters/devices/audio-resample/CLI/encoding, no external/inline assembly, no hardware acceleration.

Exact pin: FFmpeg `n9.0.1`, commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.

The current avformat manifest explicitly includes `libavformat/to_upper4.c` and `libavformat/mpegaudiotabs.c` because pinned FFmpeg materializes those library symbols separately.

## Conflict examples

Good:

- a strict source test links only its strict implementation package;
- an ordinary application links only the corresponding stable public package;
- `ImagingCore` can be used without OpenImageIO/OpenColorIO;
- `plugin/exr` is added only by applications that explicitly want raster preview integration.

Bad:

- one target links both strict and stable implementations of the same library;
- a public framework header exposes `OIIO::*`, `OCIO::*` or strict-source paths;
- a missing source/dependency is hidden with a recursive source glob;
- FFmpeg is added to `ImagingIO` or the `Imaging` umbrella without a separate architecture decision;
- a code-complete format is described as Windows-accepted before its accumulated platform gate is green.

## Engineering rhythm

Implement coherent dependency/format/source slices, review the full touched dependency boundary, publish recoverable checkpoints, and validate accumulated matrices at meaningful boundaries. Do not turn each compiler error or one-line correction into its own milestone.
