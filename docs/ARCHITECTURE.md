# Architecture

`upp_imaging` keeps upstream validation, public APIs, narrow helpers, the backend-neutral U++ Imaging framework, testing, and diagnostics separate.

## Three layers

```text
Pinned strict source packages (_src)
        ↓
Direct upstream-style public packages
        ├── openexr, openexr_core
        ├── opencolorio
        ├── oiio (OpenImageIO)
        ├── imath, libpng, libjpeg_turbo, libtiff, libdeflate, openjph
        └── fmt, robinmap
        ↓
Narrow format helpers
        └── openexr_io, png_io, jpeg_io, tiff_io
        ↓
Upp::Imaging framework (backend-neutral)
        ├── ImagingCore
        ├── ImagingIO           (planned; depends on ImagingCore + current oiio, later OpenImageIO)
        ├── ImagingColor        (planned; depends on ImagingCore + current opencolorio, later OpenColorIO)
        ├── ImagingAnalysis     (planned; depends on ImagingCore)
        ├── ImagingDiagnostics  (planned; depends on ImagingCore)
        └── Imaging (umbrella; depends on all five framework packages)
        ↓
plugin/* (opt-in raster integration)
        └── plugin/exr
```

`ImagingWorkbench` sits on top of the framework as the full-stack diagnostic application. It is not a reusable core package and applications should not depend on it.

Automated tests validate each layer. `ImagingWorkbench` is the visual diagnostic. The two never outrank each other; tests are the formal pass/fail authority, and `ImagingWorkbench` is the full-stack diagnostic authority.

## Direct upstream-style packages

These packages expose the established native APIs directly.

### OpenImageIO (target public name: `OpenImageIO`, current: `oiio`)

- Purpose: expose the native OpenImageIO API for U++ projects.
- Users work directly with `OIIO::ImageBuf`, `OIIO::ImageSpec`, `OIIO::ImageCache`, `ImageBufAlgo` and native OIIO metadata and format APIs.
- Responsible for linking the selected statically compiled OIIO format plugins.
- Currently validated formats: **OpenEXR** and **PNG**.
- JPEG, TIFF and other formats must not be described as available through OIIO until their OIIO plugins are compiled, registered and tested.

### OpenColorIO (target public name: `OpenColorIO`, current: `opencolorio`)

- Purpose: expose the native OpenColorIO API for U++ projects.
- Users work directly with OCIO configurations, processors, transforms and GPU shader extraction.

Existing direct packages such as `openexr`, `imath`, `libpng`, `libjpeg_turbo`, `libtiff`, `libdeflate` and `openjph` remain direct upstream-style packages. Renaming them is out of scope for this task.

## Upp::Imaging framework

All framework public types belong under `Upp::Imaging`. No `OIIO::*` or `OCIO::*` types may appear in public headers of the framework.

### `ImagingCore`

- Purpose: the backend-neutral image data model and result contracts.
- Public concepts: `ImageSpec`, `ImageBuffer`, `ImageData`, `Metadata`, `DataWindow`, `SampleType`, `ChannelLayout`, `Result`, `Diagnostics`.
- `ImageSpec`: dimensions, depth, channel count, channel layout, sample type, source data window.
- `ImageBuffer`: typed pixel storage, not limited to `Upp::Image` display pixels.
- `ImageData`: convenience aggregate containing `ImageSpec`, `ImageBuffer` and `Metadata`.
- `Metadata`: backend-neutral metadata container.
- `DataWindow`: preserves original source-image coordinate window including non-zero origins.
- `SampleType`: at minimum `UInt8`, `UInt16`, `Float16`, `Float32`.
- `ChannelLayout`: `Gray`, `GrayAlpha`, canonical `RGB`, canonical `RGBA` and `MultiChannel`. Arbitrary channel orders are not treated as canonical.
- `Result`: stable operation outcome and error category independent of OIIO or OCIO.
- `Diagnostics`: structured information suitable for automated tests, human-readable console reporting and `ImagingWorkbench` display.
- Depends only on U++ Core.
- Must not depend on OpenImageIO, OpenColorIO, CtrlLib, ImagingWorkbench or `plugin/exr`.

### `ImagingIO`

- Purpose: full-fidelity U++ image loading and saving using `ImagingCore` types.
- Planned with OpenImageIO as its initial backend (current package name: `oiio`, target public name: `OpenImageIO`).
- Planned dependency set: `ImagingCore`, current `oiio` (later `OpenImageIO`).
- Public headers expose only `Upp::Imaging` types; no `OIIO::ImageBuf` or `OIIO::ImageSpec`.
- Use case: typed image pixels, HDR and floating-point values, arbitrary source channels, metadata, source data-window origins, stable U++ errors, backend-independent application code.

### `ImagingColor`

- Purpose: backend-neutral colour-processing operations.
- Planned with OpenColorIO as its initial backend (current package name: `opencolorio`, target public name: `OpenColorIO`).
- Planned dependency set: `ImagingCore`, current `opencolorio` (later `OpenColorIO`).
- Public headers must not expose OCIO processor, config or transform types.
- Responsible for colour transforms and display transforms; not responsible for file loading.

### `ImagingAnalysis`

- Purpose: reusable image-analysis algorithms.
- Initial scope: histograms, channel statistics, source probes, finite and non-finite value handling.
- Later scope: waveform analysis, vectorscope analysis, other reusable scopes.
- Prefer numerical and non-GUI code so the same algorithms are used by tests and `ImagingWorkbench`.

### `ImagingDiagnostics`

- Purpose: shared structured validation and reporting.
- GUI-independent.
- Depends on `ImagingCore`.
- Supports: deterministic package tests, readable console reports, numerical comparisons, image specification reports, metadata reports, channel and sample-type reports, timing and operation diagnostics, `ImagingWorkbench` presentation.
- Package tests remain the automated correctness gates.

### `Imaging` (umbrella)

- Convenience umbrella for applications wanting the standard complete U++ Imaging framework.
- Planned dependency set: `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics`.
- Because `ImagingIO` and `ImagingColor` planned initial backends are OpenImageIO and OpenColorIO, including `Imaging` brings those standard backends.
- Applications needing a lighter dependency set may include only `ImagingCore`, `ImagingIO`, `ImagingColor` or `ImagingAnalysis` individually.
- Must not automatically include `plugin/exr`.

## U++ raster integration (opt-in)

Format plugins live under `plugin/*`. They are opt-in and display-oriented.

### `plugin/exr`

- Purpose: integrate EXR files into ordinary U++ `StreamRaster` and `Upp::Image` workflows.
- Eventually allows display-oriented use such as loading an EXR preview into `Upp::Image`.
- Not the full-fidelity EXR API.
- May convert floating-point pixels to display pixels, selected RGB or RGBA channels to `Upp::Image`, HDR values through a defined display policy.
- Must not claim to preserve arbitrary EXR channels, full source metadata, unmodified half or float pixels, source data-window semantics, or deep or multipart EXR data.
- Users requiring those features use `ImagingIO` or the direct `openexr` / `OpenImageIO` APIs.
- Must remain opt-in so it does not silently change normal U++ raster-loading behaviour.
- Reserved future pattern: `plugin/exr`, `plugin/jxl`, `plugin/hdr`.
- Only create format-specific U++ plugins where U++ does not already provide suitable native raster support.
- Do not create a broad `plugin/imaging` package at this stage.

## ImagingWorkbench

`ImagingWorkbench` is the diagnostic integration application for the complete stack. Its role:

- open real and generated images
- inspect `ImageSpec`
- inspect metadata
- inspect channel order and sample type
- display source and processed pixels
- apply colour transforms
- probe numerical values
- display histograms and statistics
- report timings and failures
- print clear human-readable console output
- help programmers and AI agents determine whether integration behaviour is correct

`ImagingWorkbench` is not a reusable core package; applications should not depend on it. Package tests remain the formal pass/fail authority.

## Internal implementation packages

These stay internal. Ordinary applications must not depend directly on them:

- all `_src` packages
- `openimageio_plugin_openexr`
- `openimageio_plugin_png`
- later OpenImageIO format-registration packages
- source include-routing shims
- build probes

## Package name policy

- `oiio` and `opencolorio` are the current names. Target public names are `OpenImageIO` and `OpenColorIO`.
- Rename happens in a later implementation task, not in this documentation pivot.
- Other direct packages keep their current names. Renaming `openexr`, `imath`, `libpng`, `libjpeg_turbo`, `libtiff`, `libdeflate`, `openjph`, `fmt`, `robinmap` is not in scope.

## LumaPix disposition

`upp_lumapix` is paused after completing its OpenImageIO reader proof. It demonstrated the contracts that `ImagingCore` and `ImagingIO` should adopt. `upp_imaging` does not depend on `upp_lumapix`. The LumaPix name is reserved for a possible future image-processing application built on `upp_imaging`.

## Architectural rules

1. Do not create fake implementations of upstream APIs.
2. Ordinary applications do not depend directly on `_src`.
3. Public packages must hide strict-source filesystem layout from consumers.
4. `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics` and `Imaging` public headers must not expose OIIO or OCIO types.
5. Format helpers stay narrow.
6. Automated comparison is authoritative.
7. `ImagingWorkbench` is supplementary diagnostic, never the correctness authority.
8. Generated images and executables belong under ignored output directories.
9. Machine-specific U++ nest configuration is not committed.
10. Format-neutral test code must not depend on a codec.
11. Add one dependency or format slice at a time.
12. Do not claim support that has not been tested.
13. `plugin/exr` and other raster plugins must be opt-in.

## Repository strategy

`upp_imaging` remains one U++ nest containing many independently usable packages.

Separate `upp_openimageio` or `upp_opencolorio` repositories are not being created yet.

A later split may be considered only if release cadence, ownership, distribution, or application integration needs justify it.

Stable package boundaries provide the useful separation now without duplicating vendored source or coordinating multiple repositories.
