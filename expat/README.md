# expat

Stable user-facing Expat include path.

Use this package from normal application code. It delegates to `expat_src` for
the compiled implementation.

## Include path

```cpp
#include <expat/expat.h>
```

## Consumer

- normal application code

## Dependency

- `expat_src`

## Validated scope

- XML parsing
- namespace-capable parser creation
- malformed XML rejection
