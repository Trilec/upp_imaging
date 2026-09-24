# openexr

User-facing OpenEXR package with a stable `third_party/openexr/openexr/...` include path.
Applications depend on `openexr` and include `<openexr/Imf.h>`.
They do not add `openexr_src` include directories.
The wrapper delegates to `openexr_src` internally and keeps the strict package available only for source-boundary validation.

Status:

- `openexr_src` = strict imported-source package
- `openexr` = stable application-facing package
- `openexr_test` = stable high-level round-trip

Current validation targets and results are listed in `tests/acceptance.txt` and `docs/WINDOWS_ACCEPTANCE.md` at the repository root.
