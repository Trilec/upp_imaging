# openimageio_plugin_tiff

Static OpenImageIO 3.1.15.0 TIFF reader/writer registration backed by the repository's existing strict libtiff 4.7.2 Windows CLANGx64 package.

The OIIO TIFF sources are compiled unchanged from `openimageio_plugins_src`. Direct OIIO registration exposes its ordinary `tif`, `tiff`, `tx`, `env`, `sm` and `vsm` extensions and native TIFF multi-image/mipmap/tiling facilities. The higher-level ImagingIO policy intentionally claims only `.tif/.tiff` in its first strict slice.

The existing `libtiff_src` backend supplies uncompressed, PackBits, LZW and zlib/libdeflate-backed Deflate support. JPEG/OJPEG, JBIG, LERC, LZMA, Zstd and WebP-in-TIFF remain disabled by that package's generated configuration.
