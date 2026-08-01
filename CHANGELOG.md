# Changelog

## Unreleased

* Defined the U++ Imaging framework pivot and the agreed implementation roadmap.
* Documented the three use cases: direct upstream-style packages, backend-neutral `Upp::Imaging` framework, and opt-in `plugin/*` raster integration.
* `README.md`, `docs/ARCHITECTURE.md`, `docs/PACKAGE_CATALOGUE.md`, `docs/STATUS_AND_ROADMAP.md` and `docs/package_layout.md` updated.
* Recorded the agreed package rename mapping: `oiio` → `OpenImageIO`, `opencolorio` → `OpenColorIO`. Rename is deferred to a later implementation task.
* Moved the strict OpenImageIO header package to `openimageio_headers` to remove the package-name collision blocking the later public wrapper rename and clarified its internal-only ownership.
* Recorded the planned framework packages under `Upp::Imaging`: `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics` and the `Imaging` umbrella.
* Recorded the planned `plugin/exr` display-oriented raster integration.
* Recorded the LumaPix disposition: `upp_lumapix` is paused after completing its OpenImageIO reader proof; `upp_imaging` does not depend on `upp_lumapix`; the LumaPix name is reserved for a possible future application built on `upp_imaging`.
* Recorded the format priority: JPEG XL, HDR, DPX/Cineon, RAW, WebP, HEIF and AVIF come before duplicating existing ordinary PNG, JPEG or TIFF U++ raster support.
* Added initial zlib package experiment.
* Split zlib into strict `zlib_src` and compatibility `zlib` package layers.
* Added `zlib_src_test` to prove imported upstream zlib 1.3.2 linkage.
* Added strict `libpng_src` and user-facing `libpng` packages.
* Added `libpng_src_test` and `libpng_test`.
* Added automated libpng and U++ plugin/png round-trip tests.
* Added strict `imath_src` and user-facing `imath` packages.
* Added `imath_src_test` and `imath_test`.
* Added OpenEXR preflight audit documentation and a minimal compile probe skeleton.
* Added strict `iex_src` and `ilmthread_src` lower-layer OpenEXR preflight packages and tests.
* Added strict `libdeflate_src` and user-facing `libdeflate` packages and tests.
* Added strict `openjph_src` and user-facing `openjph` packages and tests.
* Added the first `openexr_core_src` package attempt and probe.
