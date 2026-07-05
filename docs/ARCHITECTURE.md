# Architecture

`upp_imaging` is layered to keep upstream validation, application-facing APIs, testing, and diagnostics separate.

## Layering

```text
strict imported-source package
        ↓
stable user-facing package
        ↓
small format IO helper
        ↓
automated round-trip test
        ↓
shared visual viewer
        ↓
future OpenImageIO / OpenColorIO / LumaPix integration
```

## Strict source packages

Examples:

- `zlib_src`
- `libpng_src`
- `imath_src`
- `libdeflate_src`
- `openjph_src`
- `iex_src`
- `ilmthread_src`
- `openexr_core_src`

Purpose:

- compile genuine pinned upstream sources
- expose dependency and linker problems honestly
- prove imported-source linkage
- never contain fake replacements for missing upstream APIs

## Stable user-facing packages

Examples:

- `zlib`
- `libpng`
- `imath`
- `libdeflate`
- `openjph`
- `openexr_core`

Purpose:

- stable U++ package dependency
- stable public include path
- hide source-package layout from application code
- permit compatibility handling such as libpng symbol prefixing

## Format IO helpers

Current examples:

- `openexr_io`
- `png_io`

Purpose:

- narrow U++-friendly load/save API
- no requirement for callers to configure codec pipelines
- honest supported subset
- useful errors and clean failure handling

## Test infrastructure

`imaging_roundtrip_test_support` is format-neutral and must remain dependent only on `Core`.

It provides:

- deterministic colour/alpha/HDR test charts
- RGBA8 quantisation
- exact comparison
- tolerance-capable floating comparison
- numerical diagnostics

## Viewer

`imaging_roundtrip_viewer` is a diagnostic application, not the authority for correctness.

Numerical comparison controls PASS/FAIL.

The visual panes provide:

- `Generated`
- `Reloaded`
- `Difference`

## Architectural rules

1. Do not create fake implementations of upstream APIs.
2. Ordinary applications do not depend directly on `_src`.
3. Format helpers stay narrow.
4. Automated comparison is authoritative.
5. Viewer inspection is supplementary.
6. Generated images and executables belong under ignored output directories.
7. Machine-specific U++ nest configuration is not committed.
8. Format-neutral test code must not depend on a codec.
9. Add one dependency or format slice at a time.
10. Do not claim support that has not been tested.

## Continuation checklist

- inspect the status document first
- verify package names and APIs against code
- preserve package layering
- add automated round-trip coverage
- extend the existing viewer when appropriate
- run the relevant regression matrix
- update status only after acceptance
- keep future work marked as planned
