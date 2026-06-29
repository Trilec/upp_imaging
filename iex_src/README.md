# iex_src

`iex_src` is the strict imported-source package for the OpenEXR Iex layer.

Current scope:

- builds the upstream Iex source subset directly
- does not use system Iex
- does not provide OpenEXR image APIs
- depends on repository-owned generated config metadata derived from OpenEXR 3.4.13

Local imported-source adjustment:

- `upstream/IexMathFpu.cpp` was adjusted to include `IexConfig.h` and `IexConfigInternal.h` with quoted local includes so the imported source works under the U++ package include model
