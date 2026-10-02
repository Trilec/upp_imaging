# libpng_src

`libpng_src` is the strict upstream-source libpng package in this repository.

Meaning:

- it builds imported official libpng 1.6.59 source directly
- it depends on `zlib_src`
- it does not use U++ `plugin/png`
- it does not use U++ `plugin/z`
- it is the package to use when validating vendored-source libpng linkage

## Include style

Preferred strict include:

```cpp
#include <libpng_src/png.h>
```

## Upstream import

- upstream version: `1.6.59`
- official Git tag: `v1.6.59`, commit `cd952f49f95bb27154ae77dbb103032d95f6e580`
- source: `https://github.com/pnggroup/libpng/tree/v1.6.59`
- refreshed changed compiled files/public headers from v1.6.58 on 2026-10-02
- `pnglibconf.h`: copied from the official release file `scripts/pnglibconf.h.prebuilt`
- license text: `third_party/codecs/libpng_src/upstream/LICENSE`

## Local modifications

No upstream libpng source files were modified.
Only U++ package metadata and wrapper headers were added around the imported source.
This includes a local `upstream/zlib.h` bridge header so the imported libpng sources can resolve zlib through `zlib_src` without editing upstream libpng files.
