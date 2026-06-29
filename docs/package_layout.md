# Package Layout

This repository is one U++ nest that can host many independent packages.

Rules for package layout:

- one repo/nest, many packages
- each package should depend only on what it needs
- third-party source should stay close to the package that owns it
- future packages should be importable independently when practical

Expected dependency direction:

- `zlib_src` is the strict upstream-source base package
- `zlib` is the user-facing compatibility layer
- `libpng_src` is the strict upstream-source libpng package and depends on `zlib_src`
- `libpng` is the user-facing libpng layer and currently depends on `zlib`
- `imath_src` is the strict upstream-source Imath package
- `imath` is the user-facing Imath layer and currently depends on `imath_src`
- `iex_src` is the strict upstream-source Iex lower layer
- `ilmthread_src` is the strict upstream-source IlmThread lower layer and depends on `iex_src`
- `openexr_src` is currently only a preflight skeleton, not a full strict OpenEXR package yet
- future OpenEXR packages will likely depend on Imath and may also depend on zlib
- OpenImageIO may remain external or become an adapter later, depending on complexity

Current zlib package policy:

- `zlib_src` builds imported upstream zlib 1.3.2 directly
- `zlib` provides a stable include path for normal apps
- on Windows/Core, `zlib` may resolve through U++ `plugin/z` to avoid duplicate symbols
- strict proof of vendored upstream linkage belongs to `zlib_src_test`

Current libpng package policy:

- `libpng_src` builds imported upstream libpng 1.6.58 directly
- `libpng_src` depends on `zlib_src`
- `libpng` provides a stable include path for normal apps
- `libpng` currently compiles imported libpng source against `zlib`
- strict proof of vendored upstream linkage belongs to `libpng_src_test`

Current Imath package policy:

- `imath_src` builds imported upstream Imath 3.2.2 directly
- `imath` provides a stable include path for normal apps
- `imath` currently delegates to `imath_src`
- strict proof of vendored upstream linkage belongs to `imath_src_test`

Current OpenEXR lower-layer policy:

- `iex_src` is packaged and tested as the exception/error layer
- `ilmthread_src` is packaged and tested as the threading layer
- `openexr_core_src` is intentionally deferred until deflate/OpenJPH/config decisions are made cleanly
- `openexr_src` remains preflight only

Conflict rule:

- do not mix strict and user-facing implementations in one executable unless the goal is to test symbol behavior explicitly
- good: `libpng_src_roundtrip_test` uses `libpng_src` only
- good: `libpng_roundtrip_test` uses `Core + libpng`
- bad: one target uses both `libpng_src` and `libpng`
- reason: both layers may compile libpng objects and can cause duplicate symbols
