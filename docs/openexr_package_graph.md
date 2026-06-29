# OpenEXR Package Graph

Current intended strict-source layering for the OpenEXR branch:

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
  - depends on generated OpenEXR config metadata
  - may depend on deflate and OpenJPH-related compression configuration immediately
  - currently deferred until the compression/config surface is reduced or explicitly packaged

- `openexr_src`
  - later higher-level C++ OpenEXR image API layer
  - likely depends on `iex_src`, `ilmthread_src`, `openexr_core_src`, and `imath_src`
  - currently remains only a preflight skeleton

Current uncertainty:

- whether `openexr_core_src` should vendor or externally package deflate/OpenJPH support in the first pass
- whether `Iex`, `IlmThread`, and `OpenEXRCore` should remain top-level packages or become subpackages beneath a single `openexr_src` source tree layout
- how best to handle OpenEXR's direct include style like `<half.h>` under U++ package include paths without over-broad global shims
