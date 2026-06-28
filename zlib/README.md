# zlib

`zlib` is the normal user-facing compatibility package for U++ applications.

## Include style

Preferred public include:

```cpp
#include <zlib/zlib.h>
```

The package provides a stable user-facing include path.

## Package meaning

- use `zlib_src` when you need strict imported-source compilation and linkage
- use `zlib` when you need a convenient package for normal U++ applications

## Windows behavior

On Windows builds that also use `Core`, U++ already links `plugin/z`.
To avoid duplicate zlib symbols, this package resolves to `plugin/z` on that target.

That means `zlib` is a compatibility package on Windows/Core, not proof that the imported upstream zlib 1.3.2 objects are linked.

On targets without that conflict, `zlib` can delegate to `zlib_src`.

Strict upstream import details live in `zlib_src/README.md`.
