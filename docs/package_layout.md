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
- `libpng` is the user-facing libpng layer and currently compiles imported libpng source against `zlib`
- `minizip_ng_src` is the strict upstream-source minizip-ng package and depends on `zlib`
- `minizip_ng` is the user-facing minizip-ng layer and depends on `minizip_ng_src` and `zlib`
- `imath_src` is the strict upstream-source Imath package
- `imath` is the user-facing Imath layer and currently delegates to `imath_src`
- `libdeflate_src` is the strict standalone libdeflate package
- `libdeflate` is the user-facing libdeflate layer and currently delegates to `libdeflate_src`
- `openjph_src` is the strict standalone OpenJPH package
- `openjph` is the user-facing OpenJPH layer and currently delegates to `openjph_src`
- `iex_src` is the strict upstream-source Iex lower layer
- `ilmthread_src` is the strict upstream-source IlmThread lower layer and depends on `iex_src`
- `openexr_core_src` is the lower-level OpenEXRCore source package
- `openexr_core` is the stable lower-level OpenEXRCore package
- `openexr_src` is the full strict imported-source OpenEXR 3.4.13 high-level 96-source package and depends on `imath_src`, `iex_src`, `ilmthread_src`, and `openexr_core_src`
- `openexr` is the stable application-facing high-level package and depends on `openexr_src`
- `openexr_io` is the narrow U++ scanline RGBA helper layered on `openexr_core`
- `fmt_src` is the strict upstream fmt header-only package
- `fmt` is the user-facing fmt layer and currently depends on `fmt_src`
- `robinmap_src` is the strict upstream robin-map header-only package
- `robinmap` is the user-facing robin-map layer and currently depends on `robinmap_src`
- `opencolorio_src` builds imported OpenColorIO 2.5.2 directly and depends on `expat`, `yaml_cpp`, `pystring`, `minizip_ng`, `imath`, and `zlib`
- `opencolorio_src/OCIO.h` is the strict source-boundary wrapper used by source probes
- `opencolorio` is the stable user-facing wrapper package and keeps the public include boundary separate
- `OpenImageIO` may remain external or become an adapter later, depending on complexity

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
- `libpng` compiles imported libpng source against `zlib`
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

Current OpenEXR package policy:

- `iex_src` is packaged and tested as the exception/error layer
- `ilmthread_src` is packaged and tested as the threading layer
- `openexr_core_src` is the lower-level OpenEXRCore source package
- `openexr_core` is the stable lower-level OpenEXRCore package
- `openexr_src` is the strict high-level OpenEXR 3.4.13 package
- `openexr` is the stable application-facing high-level OpenEXR package
- `openexr_io` is the narrow scanline RGBA helper
- `openexr_src_probe` checks the strict object/header boundary
- `openexr_src_test` checks the strict high-level file round-trip
- `openexr_test` checks the stable high-level file round-trip

Current OpenColorIO package policy:

- `opencolorio_src` builds imported OpenColorIO 2.5.2 directly
- `opencolorio` is the stable application-facing wrapper
- `opencolorio_src` depends on `expat`, `yaml_cpp`, `pystring`, `minizip_ng`, `imath`, and `zlib`
- strict proof of vendored upstream linkage belongs to `opencolorio_src_test`
- stable proof of the public boundary belongs to `opencolorio_test`
- `opencolorio_gui_link_test` covers the GUI linking surface

Planned OpenImageIO package names:

- `openimageio_src` - not implemented
- `openimageio` - not implemented

Current planned prerequisite direction:

- `openexr`
- `imath`
- `zlib`
- `libpng`
- `libjpeg_turbo`
- `libtiff`
- `opencolorio`
- `fmt`
- `robinmap`

Current prerequisite coexistence check:

- `openimageio_prereq_test` passes against the stable prerequisite stack under CLANGx64

Conflict rule:

- do not link strict and stable implementations of the same library into one normal application executable

Examples:

- good: `libpng_src_roundtrip_test` uses `libpng_src` only
- good: `libpng_roundtrip_test` uses `Core + libpng`
- bad: one target uses both `libpng_src` and `libpng`
- reason: both layers may compile library objects and can cause duplicate symbols
