# openimageio_prereq_test

OpenImageIO prerequisite coexistence test for stable application-facing packages.

It depends only on the stable packages:

- `openexr`
- `imath`
- `zlib`
- `libpng`
- `libjpeg_turbo`
- `libtiff`
- `opencolorio`
- `fmt`
- `robinmap`

What it checks:

- OpenEXR stable API and boundary markers
- Imath vector math and half conversion
- zlib compress/decompress on the Windows stable provider
- libpng create/destroy API
- libjpeg-turbo compressor creation/destruction
- libtiff version and callable API identity
- OpenColorIO stable config/processor usage
- fmt formatting
- robin-map insert/find/overwrite behavior
- one combined cross-package coexistence summary

Notes:

- Windows zlib provider split is intentional
- OpenImageIO itself is not included here
- validated under CLANGx64
