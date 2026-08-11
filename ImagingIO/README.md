# ImagingIO

`ImagingIO` is the backend-neutral file I/O package for `Upp::Imaging`.
Its public API exposes only `ImagingCore` types; OpenImageIO and format backends
remain private to `ImagingIO.cpp`.

## Public API

```cpp
Result LoadImageFile(const String& path, ImageData& output,
                     Diagnostics* diagnostics = nullptr);
Result SaveImageFile(const String& path, const ImageData& image,
                     Diagnostics* diagnostics = nullptr);
```

Loads are transactional: `output` is changed only after structure, sample,
channel, pixel, metadata, close, and Core validation succeed. Saves write a
same-directory temporary file, close it completely, reopen it, verify its
specification, and decode the full pixel payload before promotion. Existing
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
- static zero-origin JPEG XL (`.jxl`)
- JPEG XL `UInt8`, `UInt16`, `Float16` and `Float32`
- JPEG XL Gray, RGB and RGBA layouts
- JPEG XL output is explicitly lossless (`jpegxl:100`)

JPEG XL GrayAlpha and arbitrary extra-channel/MultiChannel images remain
fail-closed in this slice because OpenImageIO 3.1.15's JPEG XL adapter does not
represent those channel semantics reliably enough for the framework contract.

The adapter rejects multipart, mipmapped, deep, volume, mixed-channel-format,
integer EXR, floating PNG, arbitrary PNG multichannel, non-zero-origin PNG/JXL,
and unsupported JPEG XL channel layouts with stable structured diagnostic codes.

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
Core `int64`. Portable save output emits strings, 32-bit-range integers,
doubles, and homogeneous numeric arrays where the selected backend supports
them.

Structural and plugin-managed attributes are readable as backend information
where useful but are not blindly re-emitted. Dimensions, channel structure,
alpha ownership, bit depth, compression, line order, tile/chunk and internal
OIIO state are derived from the authoritative `ImageSpec`. Omitted, unsupported,
out-of-range, heterogeneous, boolean, and read-only values always produce an
`IMGIO_METADATA` warning rather than disappearing silently.

## Validation

`imaging_io_test` owns the established EXR/PNG public contract matrix, exact
Float16 bit evidence, source immutability, diagnostics and transactional
replacement tests. `imaging_io_oiio_test` owns independent OpenImageIO structure
fixtures. `jpegxl_imagingio_test` adds the focused JPEG XL framework contract,
including all supported sample types, RGB/RGBA alpha evidence, fail-closed
channel/origin cases, and candidate verification before replacement.
