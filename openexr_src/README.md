# openexr_src

`openexr_src` is the strict imported-source OpenEXR 3.4.13 high-level package.

Current meaning:

- it imports the upstream `src/lib/OpenEXR` implementation set
- it depends on `imath_src`, `iex_src`, `ilmthread_src`, and `openexr_core_src`
- it keeps repository-owned generated config headers and include bridges alongside the imported sources
- `openexr_src_probe` verifies the strict source boundary and package version
- `openexr_src_test` now exercises a deterministic 4 x 3 scanline RGBA HALF round-trip and malformed-file rejection
- `openexr` is the stable application-facing wrapper above this package

## Upstream preflight target

- inspected version: `3.4.13`
- source archive: `https://github.com/AcademySoftwareFoundation/openexr/archive/refs/tags/v3.4.13.tar.gz`
- this package now carries the full high-level OpenEXR source boundary plus repository-owned generated config metadata

## Current scope

- strict source-boundary package for the high-level OpenEXR API
- package-level compile/run probe passes
- this does not broaden the `openexr_io` helper boundary
