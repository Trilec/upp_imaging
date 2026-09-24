# OpenImageIO

Strict upstream OpenImageIO 3.1.17.0 public-header package for U++.

## Upstream identity

- version: `v3.1.17.0`
- commit: `73bc189f7d8469a9760ce9c5099b686c77695074`
- target: `OpenImageIO`

## Scope

- public header namespace under `OpenImageIO/`
- includes the internal `detail/fmt` and `detail/pugixml` shims required by the strict source packages
- excludes source-local private headers such as `exif.h`, `kissfft.hh`, and `unittest.h`

## Ownership

- main marker owner: `third_party/openimageio/openimageio_src/OIIO.h`
- utility marker owner: `third_party/openimageio/openimageio_util_src/OIIOUtil.h`
- static macro owner for consumers: the umbrellas define `OIIO_STATIC_DEFINE`

## Dependencies

- `fmt`
- `imath`
- `libtiff`
