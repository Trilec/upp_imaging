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

`libpng` compiles the imported libpng 1.6.59 sources against `zlib`.
It does not use U++ `plugin/png`.

The GitHubOut assembly resolves U++ plugin/z to the repository provider, so
both routes link zlib 1.3.2. This package preserves its upp_png_ symbol prefix;
U++ Draw still has its own plugin/png reader. See THIRD_PARTY_SECURITY.md for
provider-specific applicability. The matching changed source/header slices were
refreshed from official v1.6.59 (cd952f49f95bb27154ae77dbb103032d95f6e580).
