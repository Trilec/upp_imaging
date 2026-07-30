# upp_imaging

## Purpose

`upp_imaging` is a U++ imaging nest that provides pinned third-party packages, strict source validation, and a layered backend-neutral U++ Imaging framework on top of those packages.

## Three supported use cases

### 1. Direct upstream-style API access

Programmers who want the established native APIs directly use the direct public packages:

- `openexr` — OpenEXR 3.4.13 high-level API
- `openexr_core` — OpenEXRCore 3.4.13 C API
- `oiio` — OpenImageIO 3.1.15.0 API (target public name: `OpenImageIO`)
- `opencolorio` — OpenColorIO 2.5.2 API (target public name: `OpenColorIO`)
- `imath`, `libpng`, `libjpeg_turbo`, `libtiff`, `libdeflate`, `openjph`, `fmt`, `robinmap`

Use these when you want the established native types and APIs directly in your application code.

### 2. Backend-neutral U++ Imaging framework

Programmers who want a U++-native, backend-neutral image API use the `Upp::Imaging` framework:

- `ImagingCore` — the backend-neutral image data model and result contracts. Depends only on U++ Core.
- `ImagingIO` — typed image loading and saving. Currently OIIO-backed. Depends on `ImagingCore` and `OpenImageIO`.
- `ImagingColor` — backend-neutral colour-processing operations. Currently OCIO-backed. Depends on `ImagingCore` and `OpenColorIO`.
- `ImagingAnalysis` — reusable image-analysis algorithms. Depends only on `ImagingCore`.
- `ImagingDiagnostics` — shared structured validation and reporting. Depends only on U++ Core.
- `Imaging` — convenience umbrella that pulls in Core, IO, Color, Analysis and Diagnostics.

Public headers in the framework expose only `Upp::Imaging` types. No `OIIO::*` or `OCIO::*` types appear in `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics` or `Imaging` public headers.

### 3. Optional U++ raster integration

Programmers who want display-oriented access into ordinary U++ `StreamRaster` and `Upp::Image` workflows use the opt-in format plugins:

- `plugin/exr` — load an EXR preview into `Upp::Image`.

These plugins are display-oriented only. They do not preserve full source data, arbitrary channels, deep or multipart layers. Use `ImagingIO` or the direct packages for full-fidelity access.

## Current validated formats

- OpenImageIO validated path: **OpenEXR** and **PNG** only.
- Direct upstream packages validated independently of OpenImageIO: OpenEXR (scanline RGBA HALF ZIP), PNG (RGBA8), JPEG (RGB8 baseline), TIFF (typed RGBA scanline).
- JPEG, TIFF and other formats are not yet routed through the OpenImageIO boundary.

## Current major capabilities

- strict and stable third-party source packages for all listed direct APIs
- `openexr`, `openexr_core`, `opencolorio`, `oiio`, `imath`, `libpng`, `libjpeg_turbo`, `libtiff`, `libdeflate`, `openjph`, `fmt`, `robinmap` stable user-facing packages
- static OpenEXR and PNG plugins integrated into the OpenImageIO build
- `openimageio_io_test` validates the EXR/PNG integration path
- narrow format helpers: `openexr_io`, `png_io`, `jpeg_io`, `tiff_io`
- deterministic package tests as the formal pass/fail authority
- `ImagingWorkbench` as a full-stack diagnostic and integration application
- `CLANGx64` as the currently validated toolchain

## Current limitations

- OpenImageIO 3.1.15.0 covers the validated EXR/PNG integration path only; JPEG, TIFF and dynamic plugin loading are not validated.
- `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics` and `Imaging` are not yet implemented; the contract definitions in `docs/ARCHITECTURE.md` are the design target.
- `plugin/exr` is not yet implemented.

## LumaPix disposition

`upp_lumapix` is paused after completing its OpenImageIO reader proof. The completed LumaPix work demonstrated:

- backend-neutral image specifications
- typed image buffers
- metadata translation
- channel-order handling
- data-window preservation
- OIIO isolation behind a private implementation
- deterministic generated-fixture testing

That work is reference material for the future `ImagingCore` and `ImagingIO` migration. `upp_imaging` does not depend on `upp_lumapix`. The LumaPix name is reserved for a possible future image-processing application built on `upp_imaging`.

## Next implementation order

1. Complete the framework documentation and public naming.
2. Perform controlled `oiio` to `OpenImageIO` and `opencolorio` to `OpenColorIO` package renaming.
3. Establish `ImagingCore` and `ImagingIO` using the proven LumaPix contracts.
4. Establish `ImagingColor`, `ImagingAnalysis` and `ImagingDiagnostics` boundaries.
5. Add `plugin/exr`.
6. Add JPEG XL support.
7. Add HDR/RGBE support.
8. Add DPX support.
9. Add RAW image support.
10. Add WebP support.
11. Add HEIF and AVIF support.
12. Expand TIFF coverage where useful.
13. Treat FFmpeg and moving-image support as a separate larger milestone.

JPEG XL, HDR, DPX, RAW, WebP, HEIF and AVIF come before duplicating existing ordinary PNG, JPEG or TIFF U++ raster support.

For each new format the sequence is:

1. package and validate required upstream dependencies
2. compile and register the OpenImageIO format plugin where applicable
3. validate direct OpenImageIO loading and saving
4. validate the ImagingIO path
5. add a format-specific `plugin/*` package only where useful for `Upp::Image` workflows

## Choosing a package

| Need | Use |
| --- | --- |
| Direct native EXR, PNG, JPEG, TIFF, OCIO, OIIO access | `openexr`, `opencolorio`, `oiio`, `libpng`, `libjpeg_turbo`, `libtiff`, `imath`, `fmt`, `robinmap`, `libdeflate`, `openjph` |
| Narrow RGBA-only scanline load/save helpers | `openexr_io`, `png_io`, `jpeg_io`, `tiff_io` |
| Backend-neutral U++ image API | `ImagingCore` + `ImagingIO` (+ `ImagingColor` for colour) |
| Standard full U++ imaging stack | `Imaging` |
| Image analysis (histograms, statistics, probes) | `ImagingAnalysis` |
| Structured diagnostics and reporting | `ImagingDiagnostics` |
| Load an EXR into `Upp::Image` | `plugin/exr` |
| Full-stack visual and diagnostic application | `ImagingWorkbench` (application only, not a reusable core package) |

## Documentation links

- `docs/ARCHITECTURE.md`
- `docs/PACKAGE_CATALOGUE.md`
- `docs/STATUS_AND_ROADMAP.md`
- `docs/package_layout.md`
