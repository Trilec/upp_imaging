# upp_imaging

`upp_imaging` is a reusable U++ imaging nest providing:

- pinned third-party packages
- strict imported-source validation
- stable user-facing packages
- small format-specific IO helpers
- automated numerical round-trip tests
- a shared diagnostic viewer
- foundations for future OpenColorIO and OpenImageIO integration

## Current state

- `OpenEXRCore` 3.4.13 is packaged and working
- EXR scanline RGBA `HALF` and `FLOAT` load/save works
- EXR `NONE` and `ZIP` compression paths are tested
- `libpng` 1.6.58 is packaged and symbol-prefixed
- PNG RGBA8 load/save works
- `libjpeg-turbo` 3.2.0 is packaged and working
- JPEG RGB8 load/save works through `jpeg_io`
- `libtiff` 4.7.2 is packaged and working
- TIFF RGBA8, RGBA16, and Float32 load/save works
- TIFF NONE, LZW, and Deflate paths are tested
- EXR, PNG, and JPEG are exercised in the shared viewer
- TIFF profiles are exercised in the shared viewer
- `CLANGx64` is the currently validated toolchain

## Current limits

EXR does not yet support:

- tiled files
- multipart files
- deep files
- arbitrary layers/channels
- metadata preservation
- non-zero data-window origins

PNG does not yet preserve:

- metadata
- ICC profiles
- gamma policy
- source colour type
- source bit depth above normalized RGBA8

TIFF support is intentionally limited to:

- classic TIFF only
- one directory
- scanline layout
- contiguous RGBA
- top-left orientation
- unassociated alpha
- no tiled input
- no planar-separate input
- no palette, grayscale, or CMYK input
- no metadata or ICC preservation
- no JPEG-compressed TIFF
- no BigTIFF claim

## Package selection

- use `openexr_io` for ordinary supported EXR RGBA load/save
- use `png_io` for ordinary PNG RGBA8 load/save
- use `jpeg_io` for ordinary JPEG RGB8 load/save
- use `tiff_io` for the supported typed TIFF subset
- use `openexr_core` and `libpng` only when direct lower-level upstream APIs are required
- use `libtiff` only when direct upstream TIFF APIs are required
- do not consume `_src` packages from ordinary application code

## Docs

- `docs/ARCHITECTURE.md`
- `docs/USAGE.md`
- `docs/STATUS_AND_ROADMAP.md`

## Notes

- build output belongs under ignored output directories such as `out/`
- machine-specific U++ nest configuration is local-only
- `GitHubOut.var.example` documents the symbolic nest requirement
