# imath

`imath` is the normal user-facing Imath package for U++ applications.

## Include style

```cpp
#include <imath/half.h>
#include <imath/ImathVec.h>
```

## Package meaning

- use `imath_src` when you need strict imported-source compilation and linkage
- use `imath` when you need a stable public include path for applications and future packages

## Current behavior

For this first pass, `imath` delegates directly to `imath_src`.
