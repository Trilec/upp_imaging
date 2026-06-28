# zlib_src

`zlib_src` is the strict upstream-source package in this repository.

Meaning:

- it builds the imported official zlib 1.3.2 source directly
- it does not use U++ `plugin/z`
- it is the package to use when validating vendored-source compilation and linkage

## Include style

Preferred include for strict tests:

```cpp
#include <zlib_src/zlib.h>
```

## Upstream import

- upstream version: `1.3.2`
- source archive: `https://zlib.net/zlib-1.3.2.tar.gz`
- license text: `zlib_src/upstream/LICENSE`

## Local modifications

No upstream zlib source files were modified.
Only U++ package metadata and a package-level wrapper header were added.
