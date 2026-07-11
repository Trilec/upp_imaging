# openimageio_util_src

Strict upstream OpenImageIO utility package for U++.

## Upstream identity

- version: `v3.1.15.0`
- commit: `cbe57bc005678ca310835473568121719861734c`
- upstream target: `OpenImageIO_Util`

## Scope

- imports the audited 18-source utility target
- exposes the minimum strict utility umbrella at `#include <openimageio_util_src/OIIOUtil.h>`
- defines `UPP_IMAGING_LOCAL_OPENIMAGEIO_UTIL_SOURCE_INCLUDE`
- keeps the main `OpenImageIO` library and all image plugins out of scope

## Source count

- exact utility source count: `18`

## Dependencies

- `imath`
- `fmt`
- platform threading support
- `psapi` on Windows if required by the imported utility implementation

## Generated version header

- `upstream/OpenImageIO/oiioversion.h` is repository-owned and derived from upstream `src/include/OpenImageIO/oiioversion.h.in`
- pinned values: `3.1.15.0`
- namespace: `OpenImageIO::v3_1`

## Validation

- the strict probe lives in `openimageio_util_src_probe`
- current build target: `CLANGx64`
- known upstream warnings: none yet recorded for this package

## Deferred work

- `openimageio_src`
- image plugins
- image file I/O
- OpenColorIO integration at the main-library layer
