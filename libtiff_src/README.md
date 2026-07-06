# libtiff_src

Strict upstream libtiff 4.7.2 source package.

## Provenance

- version: `4.7.2`
- source archive: `tiff-4.7.2.tar.gz`
- SHA-256: `672BD7D10AEE4606171AFB864F3570B83340F6A33E2C186DC0512F7145FFDF6A`
- upstream license: see `upstream/LICENSE.md`
- import date: `2026-07-06`

## Local configuration

```text
ENABLE_STATIC = ON
ENABLE_SHARED = OFF
USE_WIN32_FILEIO = ON
CCITT_SUPPORT = ON
LZW_SUPPORT = ON
PACKBITS_SUPPORT = ON
LOGLUV_SUPPORT = ON
ZIP_SUPPORT = ON
LIBDEFLATE_SUPPORT = ON
JPEG_SUPPORT = OFF
OJPEG_SUPPORT = OFF
JBIG_SUPPORT = OFF
LERC_SUPPORT = OFF
LZMA_SUPPORT = OFF
ZSTD_SUPPORT = OFF
WEBP_SUPPORT = OFF
CXX_SUPPORT = OFF
tiff-tools = OFF
tiff-tests = OFF
tiff-contrib = OFF
tiff-docs = OFF
```

## Generated headers

The checked-in generated headers are kept in `generated/`:

- `generated/tif_config.h`
- `generated/tiffconf.h`
- `generated/tiffvers.h`

These headers are derived from the official 4.7.2 templates for the fixed
Windows CLANGx64 scalar configuration.
