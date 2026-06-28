# Toolchain Matrix

## windows-clang-x64

- status: partial
- U++ build method: `CLANGx64`
- compiler: `clang++ 21.1.1`, target `x86_64-w64-windows-gnu`
- zlib: `Core + zlib` package proof passes, but runtime linkage currently resolves to U++ `plugin/z` (`1.3.1`) on this target

Notes:

- `Core` on Windows already links U++ `plugin/z`.
- The `zlib` package therefore reuses `plugin/z` linkage on this target to avoid duplicate static symbols in `Core + zlib` builds.

## Future targets

- `linux-clang-x64`
- `macos-clang-arm64`
- `macos-clang-x64`
