# openimageio_util_src

Strict upstream OpenImageIO utility package for U++.

## Upstream identity

- version: `v3.1.15.0`
- commit: `cbe57bc005678ca310835473568121719861734c`
- upstream target: `OpenImageIO_Util`

## Commit Note

- the implementation landed in commit `5d4f906`
- no history rewrite was needed; the code was already in place, just mildly mislabelled

## Scope

- imports the audited 18-source utility target
- exposes the minimum strict utility umbrella at `#include <openimageio_util_src/OIIOUtil.h>`
- keeps the main `OpenImageIO` library, image plugins, and image I/O out of scope
- utility marker owner: `third_party/openimageio/openimageio_util_src/OIIOUtil.h`
- static macro owner for consumers: the umbrellas define `OIIO_STATIC_DEFINE`

## Source count

- exact utility source count: `18`
- sources:
  - `argparse.cpp`
  - `benchmark.cpp`
  - `errorhandler.cpp`
  - `farmhash.cpp`
  - `filesystem.cpp`
  - `fmath.cpp`
  - `filter.cpp`
  - `hashes.cpp`
  - `paramlist.cpp`
  - `plugin.cpp`
  - `SHA1.cpp`
  - `strutil.cpp`
  - `sysutil.cpp`
  - `thread.cpp`
  - `timer.cpp`
  - `typedesc.cpp`
  - `ustring.cpp`
  - `xxhash.cpp`

## Dependencies

- `OpenImageIO`
- `imath`
- `fmt`
- `robinmap`
- platform threading support
- `psapi` on Windows if required by the imported utility implementation
- `robinmap` is required by `upstream/libutil/thread.cpp`, which includes `<tsl/robin_map.h>`

## Generated version header

- `third_party/openimageio/OpenImageIO/oiioversion.h` is repository-owned and derived from upstream `src/include/OpenImageIO/oiioversion.h.in`
- pinned values: `3.1.15.0`
- namespace: `OpenImageIO::v3_1`

## Forwarding Shims

- `third_party/openimageio/OpenImageIO/detail/fmt/format.h` -> `third_party/support/fmt/format.h`
- `third_party/openimageio/OpenImageIO/detail/fmt/ostream.h` -> `third_party/support/fmt/upstream/ostream.h`
- `third_party/openimageio/OpenImageIO/detail/fmt/printf.h` -> `third_party/support/fmt/upstream/printf.h`
- `third_party/openimageio/OpenImageIO/detail/fmt/std.h` -> `third_party/support/fmt/upstream/std.h`
- `upstream/tsl/robin_map.h` -> `third_party/support/robinmap/robin_map.h`
- all of these are thin forwarding headers; no implementation code, no macro games

## Header Policy

- kept utility-required public headers only
- kept utility-required internal headers and generated utility headers only
- removed main-library-only headers such as `imageio`, `imagebuf`, `imagecache`, `color`, `deepdata`, `texture`, `tiffutils`, `Imath`, and `detail/pugixml`
- the strict umbrella stays utility-only and does not expose `imageio.h`, `imagebuf.h`, `imagebufalgo.h`, `imagecache.h`, `color.h`, or `texture.h`

## Probe Policy

- no consumer probe defines package export macros
- no consumer probe needs a private include path
- the umbrella headers supply `OIIO_STATIC_DEFINE`

## Config Fixes

- `FMT_HEADER_ONLY` is owned by the stable `fmt` package through `third_party/support/fmt_src/format.h`
- `third_party/openimageio/OpenImageIO/detail/fmt.h` no longer defines `FMT_HEADER_ONLY`, so the warning is gone
- `third_party/openimageio/OpenImageIO/half.h` now includes the canonical lowercase `third_party/imath/imath/Imath/half.h` path, matching the stable `imath` package and removing the casing warning

## Validation

- current build target: `CLANGx64`

## Deferred work

- `openimageio_src`
- image plugins
- image file I/O
- OpenColorIO integration at the main-library layer

Current validation targets and results are listed in `tests/acceptance.txt` and `docs/WINDOWS_ACCEPTANCE.md` at the repository root.
