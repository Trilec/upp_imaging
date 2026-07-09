# OpenImageIO Preflight Plan

Target version: `v3.1.15.0`

Official source:
- `https://github.com/AcademySoftwareFoundation/OpenImageIO/releases/tag/v3.1.15.0`
- upstream project: `https://github.com/AcademySoftwareFoundation/OpenImageIO`

Major required dependencies:
- `Imath`
- full `OpenEXR` support, not just `openexr_core`
- `zlib`
- `libpng`
- `libjpeg-turbo`
- `libtiff`
- `fmt`
- `robin-map`
- CMake and a Windows C++ toolchain

Existing packages that should be reusable:
- `imath`
- `zlib`
- `libpng`
- `libjpeg_turbo`
- `libtiff`
- `fmt`
- `robinmap`
- likely `iex_src` and `ilmthread_src` once a full OpenEXR package is added

Still missing for a first OIIO pass:
- a full OpenEXR package boundary for OIIO to bind against
- an OIIO package wrapper and test harness
- any build glue for OIIO plugin discovery
- any OIIO-specific dependency shims beyond the imported prerequisite packages

First target:
- a thin OIIO package that proves `ImageInput` and `ImageOutput` for EXR, PNG, JPEG, and TIFF
- keep optional plugin families out of the first pass unless they are required by the four baseline codecs

First tests:
- header/version/package-compile proof
- a tiny read/write smoke test if the dependency graph stays clean
- otherwise start with read-only proof and add writes after the package boundary is stable

Out of scope:
- the OCIO viewer
- bgfx or GPU rendering
- the LumaPix adapter
