# ImagingIO

`ImagingIO` is the backend-neutral file I/O package for `Upp::Imaging`.
Its public API exposes only `ImagingCore` types; OpenImageIO is private to
`ImagingIO.cpp`.

## Public API

```cpp
Result LoadImageFile(const String& path, ImageData& output,
                     Diagnostics* diagnostics = nullptr);
Result SaveImageFile(const String& path, const ImageData& image,
                     Diagnostics* diagnostics = nullptr);
```

Loads are transactional: `output` is changed only after structure, sample,
channel, pixel, metadata, close, and Core validation succeed. Saves write a
same-directory temporary file, close it completely, then promote it. Existing
regular files are staged through a unique backup and restored if promotion
fails. Temporary and backup cleanup failures are reported without replacing
the primary error.

## Supported files

- single-image, single-mip, non-deep EXR
- EXR `Float16` and `Float32`
- EXR Gray, GrayAlpha, RGB, RGBA, alpha-only and named MultiChannel layouts
- non-zero and negative EXR data-window origins
- ordinary zero-origin PNG
- PNG `UInt8` and `UInt16`
- PNG Gray, GrayAlpha, RGB and RGBA layouts

The adapter rejects multipart, mipmapped, deep, volume, mixed-channel-format,
integer EXR, floating PNG, and arbitrary PNG multichannel files with stable
structured diagnostic codes.

## Channel policy

Backend alpha indices are authoritative where present. Alpha-only one-channel
images remain named `MultiChannel` data rather than being reclassified as
Gray. Canonical output names are `Y`, `Y/A`, `R/G/B`, and `R/G/B/A`.
Compatible explicit canonical names are retained; contradictory names are
replaced with an `IMGIO_CHANNELS` warning. Named EXR MultiChannel order and
alpha index are preserved exactly.

## Metadata policy

String, signed integer, floating scalar, and homogeneous integer/floating
arrays are translated into Core metadata without converting integer evidence
to floating point. Unsigned values are accepted only when representable by
Core `int64`. Portable EXR/PNG save output emits strings, 32-bit-range integers,
doubles, and homogeneous numeric arrays.

Structural and plugin-managed attributes are readable as backend information
where useful but are not blindly re-emitted. Dimensions, channel structure,
alpha ownership, bit depth, compression, line order, tile/chunk and internal
OIIO state are derived from the authoritative `ImageSpec`. Omitted, unsupported,
out-of-range, heterogeneous, boolean, and read-only values always produce an
`IMGIO_METADATA` warning rather than disappearing silently.

## Validation

`imaging_io_test` owns the public contract matrix, exact Float16 bit evidence,
source immutability, diagnostics and transactional replacement tests.
`imaging_io_oiio_test` creates independent OpenImageIO fixtures, cross-checks
adapter output, and proves rejection of multipart, mipmapped, deep and mixed
channel-format EXR files.
