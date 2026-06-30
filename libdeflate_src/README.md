# libdeflate_src

`libdeflate_src` is the strict imported-source libdeflate package in this repository.

Meaning:

- it builds the official standalone libdeflate 1.25 source directly
- it does not use system libdeflate
- it does not depend on OpenEXR
- it is independently usable and intended to serve future OpenEXRCore work

## Include style

Preferred strict include:

```cpp
#include <libdeflate_src/libdeflate.h>
```

## Upstream import

- upstream version: `1.25`
- source archive: `https://github.com/ebiggers/libdeflate/archive/refs/tags/v1.25.tar.gz`
- license text: `libdeflate_src/upstream/COPYING`

## Local modifications

No upstream libdeflate source files were modified.
Only U++ package metadata and wrapper headers were added.
