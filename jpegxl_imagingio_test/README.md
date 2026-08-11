# jpegxl_imagingio_test

Focused GUI-free framework acceptance for JPEG XL through `ImagingIO`.

The test exercises lossless Gray/RGB/RGBA files over UInt8, UInt16, Float16 and
Float32 storage, exact source/pixel preservation, fail-closed GrayAlpha,
MultiChannel and non-zero-origin output, transactional replacement, stable
diagnostics, and temporary/backup cleanup.
