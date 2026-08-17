# Architecture

`upp_imaging` separates strict upstream source ownership, stable direct APIs, backend-neutral U++ imaging contracts, optional raster integration, and the bounded FFmpeg media stack.

The architecture is designed so that implementation backends can evolve without leaking their types or build layout into application code.

## Layer model

```text
Pinned / strict upstream source packages
        ↓
Stable direct public packages
        ├── OpenImageIO
        ├── OpenColorIO
        ├── openexr / openexr_core
        ├── libpng / libjpeg_turbo / libtiff / ...
        └── FFmpeg (separate media stack)
        ↓
Backend-neutral Upp::Imaging framework
        ├── ImagingCore
        ├── ImagingIO
        ├── ImagingColor
        ├── ImagingAnalysis
        ├── ImagingDiagnostics
        └── Imaging umbrella
        ↓
Application / diagnostic integration
        ├── opt-in plugin/exr
        └── ImagingWorkbench
```

Strict `_src`, generated-header and static-registration packages are implementation boundaries. Ordinary applications depend on stable public packages or the `Upp::Imaging` framework instead.

## Direct upstream-style packages

Direct packages expose established native APIs without inventing substitute implementations.

### OpenImageIO

- `openimageio_headers` owns the strict upstream public-header tree and compiles no implementation source.
- `openimageio_src` / `openimageio_util_src` compile the pinned OpenImageIO implementation.
- static registration packages own the configured format plugins.
- `OpenImageIO` is the canonical application-facing package.
- `oiio` is a temporary compatibility forwarder.

The repository began with the Windows-accepted OpenEXR/PNG OIIO path and has since added the code-side still-image expansion for JPEG XL, HDR/RGBE, DPX/Cineon, RAW, WebP, decode-only HEIF/AVIF and TIFF. The post-repair accumulated Windows pass is tracked separately in `docs/ACTIVE_WORK.md`; implementation must not be confused with platform acceptance.

### OpenColorIO

`OpenColorIO` is the canonical public package over the strict `opencolorio_src` implementation. Applications may use native OCIO configurations, processors and transforms directly, while `ImagingColor` keeps those types behind a backend-neutral public boundary.

### Other direct packages

`openexr`, `openexr_core`, `imath`, `libpng`, `libjpeg_turbo`, `libtiff`, `libdeflate`, `openjph`, `fmt`, `robinmap` and their dependencies remain independently usable stable direct packages. Existing package names are retained unless a separate migration explicitly changes them.

## Upp::Imaging framework

All framework public types live in `Upp::Imaging`. Public framework headers must not expose `OIIO::*`, `OCIO::*`, strict-source filesystem paths or application GUI types.

### ImagingCore

- backend-neutral image data model and operation/result contracts;
- public concepts include `ImageSpec`, `ImageBuffer`, `ImageData`, `Metadata`, `DataWindow`, `SampleType`, `ChannelLayout`, `Result` and `Diagnostics`;
- depends only on U++ Core;
- no OIIO, OCIO, CtrlLib, Workbench or raster-plugin dependency.

### ImagingIO

- full-fidelity typed image loading/saving through `ImagingCore` types;
- OpenImageIO remains a private backend;
- supports the accepted EXR/PNG baseline and the implemented still-image expansion according to each format's documented subset;
- preserves transactional load/save semantics and stable backend-neutral diagnostics;
- public headers expose no OIIO types.

### ImagingColor

- implemented backend-neutral colour-processing layer;
- depends on `ImagingCore` and privately on `OpenColorIO`;
- supports named colour-space transforms and display transforms over the documented RGB/RGBA and unambiguous named multichannel subset;
- preserves alpha and non-RGB channels and keeps OCIO objects out of public headers.

### ImagingAnalysis

- implemented reusable numerical analysis layer;
- depends only on `ImagingCore`;
- provides channel statistics, normalized histograms, finite/non-finite accounting and source-coordinate probes;
- remains GUI-independent so tests and applications share the same numerical authority;
- waveform and vectorscope algorithms are next-scope work, not part of the current closure milestone.

### ImagingDiagnostics

- implemented Core-only deterministic comparison/reporting layer;
- formats and compares existing `ImagingCore` contracts rather than duplicating diagnostic state;
- remains GUI-independent;
- supports deterministic numerical summaries and stable text reports for tests, logs and Workbench presentation.

### Imaging umbrella

`Imaging` is the implemented convenience package over `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis` and `ImagingDiagnostics`.

Applications that need a smaller dependency set include individual packages directly. The umbrella does **not** automatically include `plugin/exr` or FFmpeg.

## U++ raster integration

Raster plugins under `plugin/*` are opt-in and display-oriented.

### plugin/exr

The implemented EXR bridge integrates ordinary single-image EXR previews with U++ `StreamRaster` / `Upp::Image` workflows.

Its contract is intentionally narrower than ImagingIO/OpenEXR/OpenImageIO:

- reads encoded EXR through the supplied U++ `Stream`;
- supports the documented RGB/RGBA, Gray/GrayAlpha, mask and unambiguous named multichannel preview cases;
- emits straight RGBA8 preview pixels;
- finite values are clamped to `[0,1]` and rounded to 8-bit;
- non-finite preview samples map deterministically to zero;
- no implicit colour transform, exposure or tone map;
- no claim to preserve arbitrary channels, source floating-point samples, complete metadata, source-window semantics, multipart/deep/mip structure or unclamped HDR values.

The plugin remains opt-in so ordinary U++ raster behaviour is not silently changed. Its expanded 22-check focused contract is implemented and awaits current Windows Debug/Release acceptance.

## FFmpeg media subsystem

FFmpeg is a separate subsystem and is not an ImagingIO backend in the current architecture.

The current first slice pins signed FFmpeg `n9.0.1` at exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa` and exposes a deliberately bounded direct stack:

```text
application
    ↓
FFmpeg
    ├── ffmpeg_avutil_src
    ├── ffmpeg_avcodec_src   (native H.264 decoder only)
    ├── ffmpeg_avformat_src  (MOV/MP4 demux + local file only)
    └── ffmpeg_swscale_src   (scalar YUV → RGBA path)
            ↓
      ffmpeg_headers
```

The generated-equivalent configuration is repository-owned and static for Windows x86_64/U++ CLANGx64. The first slice deliberately disables threads, network, external codecs, filters, devices, audio resampling, CLI/encoding, external/inline assembly and hardware acceleration.

Generated configuration must be complete for every identifier referenced by the compiled source slice. `ffmpeg_headers_test` therefore performs a source/config parity preflight over the production manifests and recursively reached headers before implementation-package validation.

Source manifests are explicit. Required upstream duplicate/materializer objects are owned by the package that needs them; they are not added by recursive globs. The selected avformat closure includes `to_upper4.c` and `mpegaudiotabs.c` because pinned FFmpeg materializes those symbols separately for libavformat.

Current implementation/source ownership is closed; Windows Debug/Release and first-frame repeatability acceptance remains the final platform gate. `docs/FFMPEG_PLAN.md` defines the first-slice contract and `docs/ACTIVE_WORK.md` records its exact validation state.

## ImagingWorkbench

`ImagingWorkbench` is the full-stack diagnostic application. It may inspect image specifications, metadata, channels, samples, colour transforms, probes, histograms, timings and failures.

It is not a reusable core package and is never the formal correctness authority. Automated package tests remain authoritative; Workbench inspection is supplementary.

## Validation state is not architecture

Architecture documents what packages own and how dependencies flow. Validation evidence is recorded separately so a source-complete subsystem is not accidentally described as Windows-accepted.

The currently established Windows framework baseline is:

- ImagingCore 48/0
- ImagingIO 79/0
- ImagingColor 66/0 plus OCIO 15/0
- ImagingAnalysis 41/0
- ImagingDiagnostics 33/0
- Imaging umbrella 6/0

JPEG XL prerequisite/backend acceptance is also recorded as 9/0 Debug and 9/0 Release after its skcms repair. Current still-image accumulation, expanded `plugin/exr`, and FFmpeg acceptance boundaries remain in `docs/ACTIVE_WORK.md` until green.

## Architectural rules

1. Do not create fake implementations of upstream APIs.
2. Ordinary applications do not depend directly on `_src`, generated-source or static-registration packages.
3. Public packages hide strict-source filesystem layout from consumers.
4. `Upp::Imaging` public headers do not expose OIIO, OCIO or application GUI types.
5. ImagingCore stays Core-only.
6. ImagingAnalysis and ImagingDiagnostics stay numerical/GUI-independent.
7. Format helpers stay narrow and claims do not exceed tested contracts.
8. Automated tests are the formal pass/fail authority; diagnostic viewers are supplementary.
9. Generated images/executables belong under ignored output directories; machine-specific U++ nest configuration is not committed.
10. Source/package manifests are explicit and reviewed against upstream ownership; do not use source globs to hide missing dependency closure.
11. Implement coherent dependency/format slices, then validate accumulated boundaries rather than creating a milestone for every operation.
12. Never describe implementation as platform-accepted until the corresponding gate is recorded green.
13. Raster plugins remain opt-in.
14. FFmpeg remains separate from ImagingIO and the Imaging umbrella until a future explicit media-abstraction decision changes that boundary.

## Repository strategy

`upp_imaging` remains one U++ nest containing independently usable packages. Stable package boundaries provide separation without duplicating vendored source or coordinating multiple repositories.

A future repository split or backend-neutral media wrapper is a separate architectural decision and is outside the current closure milestone.
