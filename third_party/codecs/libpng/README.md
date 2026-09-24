# libpng

`libpng` is the normal user-facing libpng package for U++ applications.

## Include style

```cpp
#include <libpng/png.h>
```

## Package meaning

- use `libpng_src` when you need strict imported-source compilation and linkage
- use `libpng` when you need a stable public include path for applications and future packages

## Current behavior

`libpng` compiles the imported libpng 1.6.58 sources against `zlib`.
It does not use U++ `plugin/png`.

This differs from `libpng_src` because Windows/Core builds cannot link `zlib_src` cleanly alongside U++ `plugin/z`.
Using `zlib` keeps the public package compatible with normal U++ applications while preserving strict vendored-source proof in `libpng_src`.
