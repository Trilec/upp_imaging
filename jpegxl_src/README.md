# jpegxl_src

Strict source package for the JPEG XL reference implementation (`libjxl`) 0.12.0.

The upstream tree is pinned as the Git submodule `jpegxl_src/upstream` at commit
`a7a9c787341cf703dede03c2009fa460cae5e5df` (tag `v0.12.0`). Its recursively
pinned Brotli, Highway and skcms submodules provide the only third-party source
components compiled by this package.

The package deliberately mirrors a static libjxl build with:

- JPEG XL boxes enabled, for container metadata used by the later OpenImageIO integration;
- lossless JPEG reconstruction/transcoding disabled, avoiding libjpeg-turbo;
- skcms as the colour-management backend;
- baseline skcms transform code only; libjxl/Highway retains runtime SIMD dispatch;
- repository-owned generated static export headers and `version.h` instead of a CMake configure step;
- no tools, tests, examples, command-line programs, PNG/JPEG helpers or testdata.

`import.ext` is the U++ source-selection authority. It compiles the codec,
thread runner, Brotli, Highway and skcms sources directly from the pinned tree.
No system-installed libjxl is used.

After cloning or pulling a revision that first introduces this package, initialize
its source recursively:

```text
git submodule sync --recursive
git submodule update --init --recursive
```
