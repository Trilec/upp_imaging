# openexr_src

`openexr_src` is the strict imported-source OpenEXR 3.4.14 high-level package.

Current meaning:

- it imports the upstream `src/lib/OpenEXR` implementation set
- it depends on `imath_src`, `iex_src`, `ilmthread_src`, and `openexr_core_src`
- it keeps repository-owned generated config headers and include bridges alongside the imported sources
- `openexr` is the stable application-facing wrapper above this package

## Upstream preflight target

- inspected version: `3.4.14`
- source tag: `https://github.com/AcademySoftwareFoundation/openexr/releases/tag/v3.4.14`
- this package now carries the full high-level OpenEXR source boundary plus repository-owned generated config metadata

## Current scope

- strict source-boundary package for the high-level OpenEXR API
- package-level compile/run probe passes

Current validation targets and results are listed in `tests/acceptance.txt` and `docs/WINDOWS_ACCEPTANCE.md` at the repository root.
