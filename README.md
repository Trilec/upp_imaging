# upp_imaging

## Purpose

`upp_imaging` is a U++ imaging nest with pinned third-party packages, strict source validation, stable public APIs, narrow format helpers, tests, and diagnostics.

## What applications should use

Direct upstream-style APIs:

- `openexr`
- `opencolorio`
- `libpng`
- `libjpeg_turbo`
- `libtiff`

Narrow U++ IO helpers:

- `openexr_io`
- `png_io`
- `jpeg_io`
- `tiff_io`

## Current major capabilities

- strict and stable OpenEXR 3.4.13 high-level packages exist
- strict and stable 4 x 3 RGBA HALF ZIP round-trips pass exactly
- `openexr_io` remains the narrow HALF/FLOAT, NONE/ZIP scanline helper
- OpenColorIO 2.5.2 is packaged and validated
- OpenImageIO 3.1.15.0 source/application boundary exists with a stable `oiio` package
- static OpenEXR and PNG plugins are integrated into the OpenImageIO build
- `openimageio_io_test` validates the EXR/PNG integration path
- `ImagingWorkbench` is a diagnostic and integration application; it is not the core correctness authority
- PNG, JPEG, and TIFF application packages are present with their narrow helpers
- `CLANGx64` is the currently validated toolchain

## Repository package categories

- strict imported-source packages: `_src`
- stable application-facing packages: `openexr`, `openexr_core`, `opencolorio`, `oiio`, `imath`, `zlib`, `libpng`, `libjpeg_turbo`, `libtiff`, `libdeflate`, `openjph`, `fmt`, `robinmap`
- narrow format helpers: `openexr_io`, `png_io`, `jpeg_io`, `tiff_io`
- tests and probes: package-specific validation targets
- shared infrastructure: `imaging_roundtrip_test_support`, `ImagingWorkbench` diagnostic viewer

## Current limitations

- OpenEXR high-level support is validated for scanline RGBA HALF ZIP only
- OpenEXRCore remains a narrower subset
- OpenColorIO actual GPU rendering is not implemented
- OpenColorIO export-through-OCIO is not implemented
- OpenColorIO system-monitor enumeration is headless
- OpenImageIO 3.1.15.0 covers the validated EXR/PNG integration path only; other formats and dynamic plugin loading are not validated
- `ImagingWorkbench` is a diagnostic integration viewer; its tests are not the core correctness authority

## Next milestone

LumaPix adapter.

## Documentation links

- `docs/PACKAGE_CATALOGUE.md`
- `docs/ARCHITECTURE.md`
- `docs/STATUS_AND_ROADMAP.md`
- `docs/package_layout.md`
