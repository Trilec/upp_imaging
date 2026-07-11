# Architecture

`upp_imaging` keeps upstream validation, public APIs, helpers, testing, and diagnostics separate.

## Layering

```text
Pinned strict source packages
        ↓
Stable public packages
        ├── direct codec APIs
        └── OpenColorIO
        ↓
Narrow format helpers
        ↓
Future OpenImageIO abstraction
        ↓
Future LumaPix/application adapters

Automated tests validate each layer.
The viewer remains supplementary.
```

## Pinned strict source foundations

Examples:

- `zlib_src`
- `libpng_src`
- `imath_src`
- `libdeflate_src`
- `openjph_src`
- `iex_src`
- `ilmthread_src`
- `openexr_core_src`
- `openexr_src`
- `libjpeg_turbo_src`
- `libtiff_src`
- `opencolorio_src`
- `fmt_src`
- `robinmap_src`

Purpose:

- compile genuine pinned upstream sources
- expose dependency and linker problems honestly
- prove imported-source linkage
- never contain fake replacements for missing upstream APIs

## Stable public packages

Examples:

- `zlib`
- `libpng`
- `imath`
- `libdeflate`
- `openjph`
- `openexr_core`
- `openexr`
- `libjpeg_turbo`
- `libtiff`
- `opencolorio`
- `fmt`
- `robinmap`

Purpose:

- stable U++ package dependency
- stable public include path
- hide strict source-package filesystem layout from application code
- permit compatibility handling such as libpng symbol prefixing and OpenEXR boundary shims

## Narrow format helpers

Examples:

- `openexr_io`
- `png_io`
- `jpeg_io`
- `tiff_io`

Purpose:

- narrow U++-friendly load/save API
- no requirement for callers to configure codec pipelines
- honest supported subset
- useful errors and clean failure handling

## Tests and probes

- `probe` means a fast package/header/object boundary check
- `test` means behavioral or numerical validation
- `viewer` means supplementary visual diagnostics

Numerical comparison controls PASS/FAIL.

The visual panes provide generated, reloaded, and difference views, but they never outrank the numerical result.

## Architectural rules

1. Do not create fake implementations of upstream APIs.
2. Ordinary applications do not depend directly on `_src`.
3. Public packages must hide strict-source filesystem layout from consumers.
4. Format helpers stay narrow.
5. Automated comparison is authoritative.
6. Viewer inspection is supplementary.
7. Generated images and executables belong under ignored output directories.
8. Machine-specific U++ nest configuration is not committed.
9. Format-neutral test code must not depend on a codec.
10. Add one dependency or format slice at a time.
11. Do not claim support that has not been tested.

## Repository strategy

`upp_imaging` remains one U++ nest containing many independently usable packages.

Separate `upp_openimageio` or `upp_opencolorio` repositories are not being created yet.

A later split may be considered only if release cadence, ownership, distribution, or application integration needs justify it.

Stable package boundaries provide the useful separation now without duplicating vendored source or coordinating multiple repositories.
