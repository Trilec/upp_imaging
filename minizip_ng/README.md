# minizip_ng

Stable user-facing minizip-ng include path.

Use this package from normal application code. It delegates to `minizip_ng_src`
for the compiled implementation.

## Include path

```cpp
#include <minizip_ng/mz.h>
#include <minizip_ng/mz_zip.h>
#include <minizip_ng/mz_zip_rw.h>
```

## Consumer

- normal application code

## Dependency

- `minizip_ng_src`

## Validated scope

- ZIP write/read round-trip
- malformed ZIP rejection
