# openexr

User-facing OpenEXR package with a stable `openexr/...` include path.
Delegates to `openexr_src` and does not compile the OpenEXR implementation again.

Status:

- `openexr_src` = strict imported-source package
- `openexr` = stable application-facing package
- `openexr_src_test` = strict high-level round-trip
- `openexr_test` = stable high-level round-trip
- `openexr_io` = separate narrow U++ helper
