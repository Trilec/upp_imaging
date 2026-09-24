# openjph_src

`openjph_src` is the strict imported-source OpenJPH package in this repository.

Meaning:

- it builds the official standalone OpenJPH 0.27.1 core library source directly
- it does not use system OpenJPH
- it does not depend on OpenEXRCore
- it supplies the linked `openexr_core_src` JPEG 2000 decoder dependency

## Include style

Preferred strict include:

```cpp
#include <openjph_src/openjph.h>
```

## Packaging notes

- this first pass imports only the `src/core` library tree
- command-line executables are not packaged
- TIFF support is not packaged
- SIMD-specific source files are intentionally excluded for the initial portable package proof
