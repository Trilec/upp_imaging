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

### EXR
- single-image, single-mip, non-deep
- Float16 and Float32
- Gray, GrayAlpha, RGB, RGBA, alpha-only and named MultiChannel
- non-zero and negative data-window origins

### PNG
- ordinary zero-origin images
- UInt8 and UInt16
- Gray, GrayAlpha, RGB and RGBA

### JPEG XL
- static zero-origin `.jxl`
- UInt8, UInt16, Float16 and Float32
- Gray, RGB and RGBA
- output explicitly lossless (`jpegxl:100`)
- GrayAlpha and arbitrary extra-channel/MultiChannel remain fail-closed

### Radiance HDR/RGBE
- `.hdr` and `.rgbe`
- zero-origin Float32 RGB only
- no implicit tone mapping, alpha synthesis or integer facade
- RGBE is a quantized HDR representation; arbitrary Float32 bit identity is not promised

### DPX
- initial framework slice is single-image UInt8/UInt16 RGB
- non-negative data-window origins are preserved
- 10/12-bit DPX may load through OIIO as UInt16; `oiio:BitsPerSample` is treated as backend-managed evidence and is not silently re-emitted on save
- negative origins, additional layouts and floating DPX remain fail-closed in this slice

### Cineon
- input only, matching OpenImageIO 3.1.15.0
- initial slice accepts UInt8/UInt16 Gray and RGB images
- save attempts fail explicitly with `IMGIO_FORMAT`

Unsupported structures fail closed with stable diagnostics: multipart, mipmapped,
deep, volume, mixed-channel-format, integer EXR, floating PNG, arbitrary PNG/JPEG XL
multichannel files, unsupported JPEG XL channel layouts, unsupported HDR layouts,
and DPX/Cineon structures outside the initial policy.

## Channel policy

Backend alpha indices are authoritative where present. Alpha-only one-channel
images remain named `MultiChannel` data rather than being reclassified as Gray.
Canonical output names are `Y`, `Y/A`, `R/G/B`, and `R/G/B/A`. Compatible
explicit canonical names are retained; contradictory names are replaced with an
`IMGIO_CHANNELS` warning. Cineon intensity channel name `I` is accepted as Gray.

## Metadata policy

String, signed integer, floating scalar, and homogeneous integer/floating arrays
are translated into Core metadata without converting integer evidence to floating
point. Unsigned values are accepted only when representable by Core `int64`.
Portable save output emits strings, 32-bit-range integers, doubles, and homogeneous
numeric arrays where the selected backend supports them.

Structural and plugin-managed attributes are readable as backend information where
useful but are not blindly re-emitted. Dimensions, channel structure, alpha
ownership, bit depth, compression, line order, tile/chunk and internal OIIO state
are derived from the authoritative `ImageSpec`. Omitted, unsupported, out-of-range,
heterogeneous, boolean, and read-only values produce `IMGIO_METADATA` warnings.

## Validation

`imaging_io_test` owns the established EXR/PNG public contract matrix.
`jpegxl_imagingio_test` owns the JPEG XL framework contract.
`hdr_dpx_imagingio_test` owns the focused HDR/RGBE and DPX/Cineon policy,
transaction and refusal contract.
