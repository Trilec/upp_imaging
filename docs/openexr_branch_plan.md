# OpenEXR Branch Plan

Imath is the first step in the future OpenEXR branch for this repository.

Current scope:

- `imath_src` provides strict imported-source validation for Imath 3.2.2
- `imath` provides the user-facing U++ package layer
- OpenEXR itself is not added as a full library package yet
- `openexr_src` is currently only a preflight header/probe skeleton
- `openexr_src_probe` is currently only a compile-time compatibility probe

Why Imath first:

- Imath is independently useful for half floats, vectors, matrices, boxes, colors, and math helpers
- OpenEXR depends on Imath
- validating Imath separately keeps the package graph cleaner and makes future OpenEXR issues easier to isolate

Future OpenEXR work should evaluate:

- how much of OpenEXR can remain strict upstream-source packages
- whether Windows/Core symbol interactions introduce any compatibility layer similar to `zlib` and `libpng`
- whether extra generated config headers or namespace/versioning rules need repository-owned packaging decisions
- how to package `Iex`, `IlmThread`, and `OpenEXRCore` cleanly before attempting a real EXR read/write test
