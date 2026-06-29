# imath_src

`imath_src` is the strict upstream-source Imath package in this repository.

Meaning:

- it builds imported official Imath 3.2.2 source directly
- it does not use system Imath
- it does not use OpenEXR
- it is the package to use when validating vendored-source Imath linkage

## Include style

Preferred strict includes:

```cpp
#include <imath_src/half.h>
#include <imath_src/ImathVec.h>
```

## Upstream import

- upstream version: `3.2.2`
- source archive: `https://github.com/AcademySoftwareFoundation/Imath/archive/refs/tags/v3.2.2.tar.gz`
- generated config header: `imath_src/upstream/ImathConfig.h` was created from upstream `config/ImathConfig.h.in` using upstream release defaults
- license text: `imath_src/upstream/LICENSE.md`

## Local modifications

No upstream Imath source files were modified.
Only U++ package metadata, wrapper headers, and a repository-owned generated `ImathConfig.h` were added.
