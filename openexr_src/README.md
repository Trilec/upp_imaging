# openexr_src

`openexr_src` is a preflight package skeleton for OpenEXR.

Current meaning:

- it is not a full OpenEXR implementation package yet
- it captures the minimum imported public/header surface needed for compile-time probing
- it depends on `imath_src`
- it does not claim EXR read/write support in this task

## Upstream preflight target

- inspected version: `3.4.13`
- source archive: `https://github.com/AcademySoftwareFoundation/openexr/archive/refs/tags/v3.4.13.tar.gz`
- this preflight imports only a small public/header subset plus repository-owned generated config metadata

## Current scope

- compile-time namespace/version/header probe only
- no OpenEXR library `.cpp` files are packaged yet
- no EXR file round-trip is implemented yet
- broader public header use still needs a clean decision for OpenEXR's direct `<half.h>` include pattern under U++ package include paths
