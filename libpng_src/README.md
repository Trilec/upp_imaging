# libpng_src

`libpng_src` is the strict upstream-source libpng package in this repository.

Meaning:

- it builds imported official libpng 1.6.58 source directly
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

- upstream version: `1.6.58`
- source archive: `https://download.sourceforge.net/libpng/libpng-1.6.58.tar.xz`
- `pnglibconf.h`: copied from the official release file `scripts/pnglibconf.h.prebuilt`
- license text: `libpng_src/upstream/LICENSE`

## Local modifications

No upstream libpng source files were modified.
Only U++ package metadata and wrapper headers were added around the imported source.
This includes a local `upstream/zlib.h` bridge header so the imported libpng sources can resolve zlib through `zlib_src` without editing upstream libpng files.
