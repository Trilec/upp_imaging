# openexr

User-facing OpenEXR package with a stable `openexr/...` include path.
Applications depend on `openexr` and include `<openexr/Imf.h>`.
They do not add `openexr_src` include directories.
The wrapper delegates to `openexr_src` internally and keeps the strict package available only for source-boundary validation.

Status:

- `openexr_src` = strict imported-source package
- `openexr` = stable application-facing package
- `openexr_src_test` = strict high-level round-trip
- `openexr_test` = stable high-level round-trip
- `openexr_io` = separate narrow U++ helper
