# libraw_src

`libraw_src` is the repository-pinned LibRaw backend used by the OpenImageIO RAW plugin.

## Source boundary

- LibRaw release: 0.22.2
- upstream commit: `b93f6e45c194f5df9b02a43b1af9a54b4f41f33f`
- source lives in the `upstream` git submodule
- the package compiles the static source list published by LibRaw's own qmake/autotools build descriptions
- no system-installed LibRaw library or DLL is required

The first U++ slice deliberately leaves optional RawSpeed, Adobe DNG SDK, LCMS and external JPEG integrations disabled. LibRaw's built-in camera decoders, DNG decoding, demosaicing, metadata handling and internal lossless-JPEG path remain part of the compiled source set.

`LIBRAW_NODLL` makes the public headers match the static linkage model. `LIBRAW_BUILDLIB` identifies the package as the library build rather than a consumer.

On Windows, `_USE_MATH_DEFINES` is supplied at the package compile boundary so the CRT exposes constants such as `M_PI` and `M_SQRT1_2` before LibRaw's first `<math.h>` include. The pinned upstream source defines the macro later in `internal/defines.h`, after `internal/libraw_cxx_defs.h` has already included `<math.h>`, so the package-level definition preserves the upstream source unchanged while satisfying its math-constant assumption under U++ CLANGx64.

## Intended use

Application code should normally use `OpenImageIO` or `ImagingIO` rather than depend on this package directly. The direct `libraw_prereq_test` exists only to prove the pinned backend and static ABI contract independently of OpenImageIO.
