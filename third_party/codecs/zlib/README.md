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
- on Windows, `zlib` resolves through the repository-owned `plugin/z` overlay,
  which delegates to the pinned `zlib_src` 1.3.2 implementation
- on non-Windows targets, `zlib` delegates to `zlib_src`
- the Windows provider is selected to coexist with U++ GUI/plugin linkage and still satisfies the OpenColorIO minimum version

## Package meaning

- use `zlib_src` when you need strict imported-source compilation and linkage
- use `zlib` when you need a convenient package for normal U++ applications

## Windows behavior

On Windows builds that also use `Core`, U++ already depends on `plugin/z`.
The repository assembly resolves that package name to
`third_party/codecs/plugin/z`, which delegates to `zlib_src` rather than
compiling another zlib copy.

The runtime provider still requires verification for each assembly and
toolchain; the repository manifest alone does not prove linkage.

On targets without that conflict, `zlib` can delegate to `zlib_src`.

Strict upstream import details live in `third_party/codecs/zlib_src/README.md`.
