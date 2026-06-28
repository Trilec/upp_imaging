# CLANG Build Notes

This repository is being validated first with the local U++ `umk` + CLANG toolchain on Windows.

The first expected build method is `CLANGx64`.
Observed local compiler:

- `clang++ 21.1.1`
- target: `x86_64-w64-windows-gnu`

Observed package behavior on Windows:

- `Core` already links `plugin/z`
- a second static zlib package causes duplicate linker symbols
- the current `zlib` package reuses `plugin/z` linkage on this target so `Core + zlib` builds succeed
- `zlib_src` exists specifically to prove the imported upstream source can be compiled and linked without that compatibility layer

Current validation result:

- `zlib_src_test` builds without `Core` and reports zlib `1.3.2`
- `zlib_test` builds with `Core + zlib` and reports zlib `1.3.1` on Windows/Core
