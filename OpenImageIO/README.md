# OpenImageIO

Strict upstream OpenImageIO 3.1.15.0 public-header package for U++.

## Upstream identity

- version: `v3.1.15.0`
- commit: `cbe57bc005678ca310835473568121719861734c`
- target: `OpenImageIO`

## Scope

- public header namespace under `OpenImageIO/`
- includes the internal `detail/fmt` and `detail/pugixml` shims required by the strict source packages
- excludes source-local private headers such as `exif.h`, `kissfft.hh`, and `unittest.h`

## Ownership

- main marker owner: `openimageio_src/OIIO.h`
- utility marker owner: `openimageio_util_src/OIIOUtil.h`
- static macro owner for consumers: the umbrellas define `OIIO_STATIC_DEFINE`

## Dependencies

- `fmt`
- `imath`
- `libtiff`
