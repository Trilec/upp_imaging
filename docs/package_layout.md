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
- on Windows, `zlib` resolves to U++ `plugin/z` to coexist with GUI/plugin packages
- on non-Windows targets, `zlib` delegates to `zlib_src`
- `libpng_src` is the strict upstream-source libpng package and depends on `zlib_src`
- `libpng` is the user-facing libpng layer and currently depends on `zlib`
- `minizip_ng_src` is the strict upstream-source minizip-ng package and depends on `zlib`
- `minizip_ng` is the user-facing minizip-ng layer and depends on `minizip_ng_src` and `zlib`
- `imath_src` is the strict upstream-source Imath package
- `imath` is the user-facing Imath layer and currently depends on `imath_src`
- `libdeflate_src` is the strict standalone libdeflate package
- `libdeflate` is the user-facing libdeflate layer and currently depends on `libdeflate_src`
- `openjph_src` is the strict standalone OpenJPH package
- `openjph` is the user-facing OpenJPH layer and currently depends on `openjph_src`
- `iex_src` is the strict upstream-source Iex lower layer
- `ilmthread_src` is the strict upstream-source IlmThread lower layer and depends on `iex_src`
- `openexr_core_src` is the first OpenEXRCore package attempt with a modest real implementation slice
- `openexr_src` is currently only a preflight skeleton, not a full strict OpenEXR package yet
- future OpenEXR packages will likely depend on Imath and may also depend on zlib
- `opencolorio_src` builds imported OpenColorIO 2.5.2 directly, with generated builtin config blobs and a headless monitor path on Windows
- `opencolorio_src/OCIO.h` is the strict source-boundary wrapper used by source probes
- `opencolorio` is the stable user-facing wrapper package and keeps the public include boundary separate
- OpenImageIO may remain external or become an adapter later, depending on complexity

Current zlib package policy:

- `zlib_src` builds imported upstream zlib 1.3.2 directly
- `zlib` provides a stable include path for normal apps
- on Windows/Core, `zlib` resolves through U++ `plugin/z` to avoid duplicate symbols
- this Windows provider also satisfies the minimum zlib version used by the OCIO dependency foundation
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

Current libdeflate package policy:

- `libdeflate_src` builds official standalone libdeflate 1.25 directly
- `libdeflate` provides a stable include path for normal apps
- `libdeflate` currently delegates to `libdeflate_src`
- strict proof of vendored upstream linkage belongs to `libdeflate_src_test`

Current OpenJPH package policy:

- `openjph_src` builds official standalone OpenJPH 0.26.3 directly
- `openjph` provides a stable include path for normal apps
- `openjph` currently delegates to `openjph_src`
- strict proof of vendored upstream linkage belongs to `openjph_src_test`

Current OpenEXR lower-layer policy:

- `iex_src` is packaged and tested as the exception/error layer
- `ilmthread_src` is packaged and tested as the threading layer
- `openexr_core_src` now exists as a modest first package attempt and probe layer
- full OpenEXRCore coverage is still deferred
- `openexr_src` remains preflight only

Conflict rule:

- do not mix strict and user-facing implementations in one executable unless the goal is to test symbol behavior explicitly
- good: `libpng_src_roundtrip_test` uses `libpng_src` only
- good: `libpng_roundtrip_test` uses `Core + libpng`
- bad: one target uses both `libpng_src` and `libpng`
- reason: both layers may compile libpng objects and can cause duplicate symbols
