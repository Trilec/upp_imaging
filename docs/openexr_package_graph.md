# OpenEXR Package Graph

Current intended strict-source layering for the OpenEXR branch:

- `libdeflate_src`
  - independent compression package
  - likely future dependency of `openexr_core_src`

- `libdeflate`
  - user-facing wrapper package for normal app usage

- `openjph_src`
  - HTJ2K / OpenJPH package
  - likely future dependency of `openexr_core_src`

- `openjph`
  - user-facing wrapper package for normal app usage

- `iex_src`
  - OpenEXR exception and error support layer
  - should be independently usable
  - depends on generated OpenEXR config metadata

- `ilmthread_src`
  - OpenEXR threading primitives
  - depends on generated OpenEXR config metadata
  - depends on `iex_src`
  - may require platform-specific semaphore/thread implementation selection

- `openexr_core_src`
  - low-level OpenEXR core/C layer
  - depends on `imath_src`
  - likely depends on `libdeflate_src`
  - depends on generated OpenEXR config metadata
  - may depend on deflate and OpenJPH-related compression configuration immediately
  - currently deferred until the compression/config surface is reduced or explicitly packaged

- `openexr_src`
  - later higher-level C++ OpenEXR image API layer
  - likely depends on `iex_src`, `ilmthread_src`, `openexr_core_src`, and `imath_src`
  - currently remains only a preflight skeleton

Current tested lower layers:

- `iex_src` added and test-backed
- `ilmthread_src` added and test-backed
- `openexr_core_src` still deferred
- full OpenEXR read/write still deferred

Current uncertainty:

- whether `openexr_core_src` should vendor or externally package deflate/OpenJPH support in the first pass
- whether OpenEXRCore should depend directly on `libdeflate_src` or a more specialized package shim
- whether OpenJPH remains broadly useful enough as a standalone package or mostly exists here to keep the OpenEXRCore graph explicit
- whether `Iex`, `IlmThread`, and `OpenEXRCore` should remain top-level packages or become subpackages beneath a single `openexr_src` source tree layout
- how best to handle OpenEXR's direct include style like `<half.h>` under U++ package include paths without over-broad global shims
