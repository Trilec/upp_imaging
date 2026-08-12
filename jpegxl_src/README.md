# jpegxl_src

Strict source package for the JPEG XL reference implementation (`libjxl`) 0.12.0.

The upstream tree is pinned as the Git submodule `jpegxl_src/upstream` at commit
`a7a9c787341cf703dede03c2009fa460cae5e5df` (tag `v0.12.0`). Its recursively
pinned Brotli, Highway and skcms submodules provide the only third-party source
components compiled by this package.

The package deliberately mirrors a static libjxl build with:

- JPEG XL boxes enabled, for container metadata used by the OpenImageIO integration;
- lossless JPEG reconstruction/transcoding disabled, avoiding libjpeg-turbo;
- skcms selected explicitly as the colour-management backend (`JPEGXL_ENABLE_SKCMS=1`);
- repository-owned generated static export headers and `version.h` instead of a CMake configure step;
- no tools, tests, gbench sources, test helpers, examples, command-line programs, codec extras or testdata.

`import.ext` is the U++ source-selection authority. For libjxl it mirrors the
pinned upstream `jxl_lists.cmake` production groups used by the CMake build:
`JPEGXL_INTERNAL_DEC_SOURCES`, the box decoder required by boxes,
`JPEGXL_INTERNAL_ENC_SOURCES`, `JPEGXL_INTERNAL_CMS_SOURCES`, and
`JPEGXL_INTERNAL_THREADS_SOURCES`. The JPEG reconstruction decoder group is not
compiled because `JPEGXL_ENABLE_TRANSCODE_JPEG=0`. The encoder's internal
`enc_jpeg_*` translation units remain because upstream includes them in the core
encoder group; they do not require libjpeg-turbo.

The Highway dependency is likewise limited to the seven translation units in
its `HWY_SOURCES` core library (`abort`, `aligned_allocator`, `nanobenchmark`,
`per_target`, `print`, `targets`, and `timer`). Highway contrib/test/example
sources are not compiled. Brotli is limited to its common/decoder/encoder C
source directories, and skcms is compiled from its single production
`skcms.cc` translation unit.

This explicit production manifest is intentional. U++ `import.ext` file
patterns recurse below the matched directory, so broad `lib/jxl/*.cc` or
`hwy/*.cc` patterns would also pull test/helper sources and are not used here.
No system-installed libjxl, lcms2, or googletest is required.

After cloning or pulling a revision that first introduces this package, initialize
its source recursively:

```text
git submodule sync --recursive
git submodule update --init --recursive
```
