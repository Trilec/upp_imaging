# openimageio_src

Strict upstream OpenImageIO 3.1.15.0 main-library package for U++.

## Upstream identity

- version: `v3.1.15.0`
- commit: `cbe57bc005678ca310835473568121719861734c`
- target: `OpenImageIO`

## Scope

- strict main library only
- no plugins
- no image-file I/O tests or tools
- main marker owner: `openimageio_src/OIIO.h`
- utility marker owner: `openimageio_util_src/OIIOUtil.h`
- static macro owner for consumers: the umbrellas define `OIIO_STATIC_DEFINE`

## Dependencies

- `OpenImageIO`
- `openimageio_util_src`
- `openexr`
- `imath`
- `zlib`
- `libtiff`
- `libjpeg_turbo`
- `opencolorio`
- `fmt`
- `robinmap`

## Generated config

- `buildopts.h` is repository-owned for this package
- `OIIO_DEFAULT_PLUGIN_SEARCHPATH = ""`
- `OIIO_ALL_BUILD_DEPS_FOUND = "OpenEXR;Imath;OpenColorIO;ZLIB;TIFF;JPEG;fmt;robinmap"`
- `EMBED_PLUGINS` is not enabled

## Validation

- probe target: `openimageio_src_probe`
- strict markers: main and utility markers are both required
- no-file-I/O probe only
- probe uses the static `OIIO` API path
- consumer probes do not define package export macros
