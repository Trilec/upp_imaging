# yaml_cpp

Stable user-facing yaml-cpp include path.

Use this package from normal application code. It delegates to `yaml_cpp_src`
for the compiled implementation.

## Include path

```cpp
#include <yaml_cpp/yaml.h>
```

## Consumer

- normal application code

## Dependency

- `yaml_cpp_src`

## Validated scope

- in-memory YAML parse
- emit and re-parse
- malformed YAML rejection
