# zlib

`zlib` is the normal user-facing compatibility package for U++ applications.

## Include style

Preferred public include:

```cpp
#include <zlib/zlib.h>
```

The package provides a stable user-facing include path.

## Provider split

- `zlib_src` is the pinned strict-source package for zlib 1.3.2
- `zlib` is the stable user-facing package
- on Windows, `zlib` resolves through U++ `plugin/z` (runtime/header 1.3.1)
- on non-Windows targets, `zlib` delegates to `zlib_src`
- the Windows provider is selected to coexist with U++ GUI/plugin linkage and still satisfies the OpenColorIO minimum version

## Package meaning

- use `zlib_src` when you need strict imported-source compilation and linkage
- use `zlib` when you need a convenient package for normal U++ applications

## Windows behavior

On Windows builds that also use `Core`, U++ already links `plugin/z`.
To avoid duplicate zlib symbols, this package resolves to `plugin/z` on that target.

That means `zlib` is a compatibility package on Windows/Core, not proof that the imported upstream zlib 1.3.2 objects are linked.

On targets without that conflict, `zlib` can delegate to `zlib_src`.

Strict upstream import details live in `zlib_src/README.md`.
