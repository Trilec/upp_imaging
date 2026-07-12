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
- defines `UPP_IMAGING_LOCAL_OPENIMAGEIO_UTIL_SOURCE_INCLUDE`
- keeps the main `OpenImageIO` library, image plugins, and image I/O out of scope

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

- `imath`
- `fmt`
- `robinmap`
- platform threading support
- `psapi` on Windows if required by the imported utility implementation
- `robinmap` is required by `upstream/libutil/thread.cpp`, which includes `<tsl/robin_map.h>`

## Generated version header

- `upstream/OpenImageIO/oiioversion.h` is repository-owned and derived from upstream `src/include/OpenImageIO/oiioversion.h.in`
- pinned values: `3.1.15.0`
- namespace: `OpenImageIO::v3_1`

## Forwarding Shims

- `upstream/OpenImageIO/detail/fmt/format.h` -> `fmt_src/format.h`
- `upstream/OpenImageIO/detail/fmt/ostream.h` -> `fmt_src/upstream/ostream.h`
- `upstream/OpenImageIO/detail/fmt/printf.h` -> `fmt_src/upstream/printf.h`
- `upstream/OpenImageIO/detail/fmt/std.h` -> `fmt_src/upstream/std.h`
- `upstream/tsl/robin_map.h` -> `robinmap/robin_map.h`
- all of these are thin forwarding headers; no implementation code, no macro games

## Header Policy

- kept utility-required public headers only
- kept utility-required internal headers and generated utility headers only
- removed main-library-only headers such as `imageio`, `imagebuf`, `imagecache`, `color`, `deepdata`, `texture`, `tiffutils`, `Imath`, and `detail/pugixml`
- the strict umbrella stays utility-only and does not expose `imageio.h`, `imagebuf.h`, `imagebufalgo.h`, `imagecache.h`, `color.h`, or `texture.h`

## Config Fixes

- `FMT_HEADER_ONLY` is owned by the stable `fmt` package through `fmt_src/format.h`
- `OpenImageIO/detail/fmt.h` no longer defines `FMT_HEADER_ONLY`, so the warning is gone
- `OpenImageIO/half.h` now includes the canonical lowercase `imath/half.h` path, matching the stable `imath` package and removing the casing warning

## Validation

- the strict probe lives in `openimageio_util_src_probe`
- current build target: `CLANGx64`
- probe status: pass
- probe output: `source_count=18`, `version=3.1.15.0`, `namespace=v3_1`, `SUMMARY passed=10 failed=0`
- clean probe rebuild: pass
- remaining utility warnings: genuine upstream deprecation noise from `benchmark.cpp` and `argparse.cpp` (`time_trial`, `callback_t`)
- dependent test matrix: `openimageio_prereq_test`, `imath_test`, `fmt_test`, and `robinmap_test` all pass
- dependency test noise that remains outside this package: OpenColorIO/OpenEXR casing warnings in `openimageio_prereq_test`

## Deferred work

- `openimageio_src`
- image plugins
- image file I/O
- OpenColorIO integration at the main-library layer
