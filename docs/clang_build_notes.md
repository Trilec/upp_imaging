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
