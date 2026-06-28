# zlib

This package contains upstream `zlib` 1.3.2 packaged for U++.

## Include style

Preferred public include:

```cpp
#include <zlib/zlib.h>
```

The package provides `zlib/zlib.h` as a thin wrapper over `zlib/upstream/zlib.h` so callers do not need to include headers from the `upstream/` subdirectory directly.

On Windows builds that also use `Core`, the wrapper intentionally resolves to U++'s `plugin/z` header because `Core` already brings in `plugin/z` on that platform. This avoids duplicate zlib symbols while preserving the imported upstream source tree in this repository.

## Upstream import

- upstream version: `1.3.2`
- source archive: `https://zlib.net/zlib-1.3.2.tar.gz`
- upstream license text: `zlib/upstream/LICENSE`

## Local modifications

No upstream source files were modified for the initial import.
Only U++ package metadata and a wrapper header were added around the imported source.

## Windows note

For the current `umk` + `CLANGx64` proof using `Core`, U++ on Windows links `plugin/z` through `Core`. Because of that, the package currently reuses that linkage on Windows instead of compiling a second static zlib copy into the same executable.
