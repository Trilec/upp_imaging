# openimageio_plugin_tiff

Static OpenImageIO 3.1.17.0 TIFF reader/writer registration backed by the repository's existing strict libtiff 4.7.2 Windows CLANGx64 package.

The pinned OIIO TIFF source files remain unchanged in `openimageio_plugins_src`.
The reader compiles through TiffInput.cpp, a repository overlay that applies
libtiff open options to both file and IOProxy input. Direct registration exposes
ordinary `tif`, `tiff`, `tx`, `env`, `sm` and `vsm` extensions and native TIFF
multi-image/mipmap/tiling facilities. ImagingIO claims only `.tif/.tiff`.

Native allocations owned by each libtiff read handle are limited to 256 MiB per
allocation and 512 MiB cumulatively. OIIO's existing limits:imagesize_MB can
lower the native single-allocation budget (clamped to 1..256 MiB); the total is
twice that budget, so zero cannot disable them. OIIO/C++ scratch and output buffers are separate;
this does not establish process isolation or hostile-input memory safety.

The existing `libtiff_src` backend supplies uncompressed, PackBits, LZW and third_party/codecs/zlib/libdeflate-backed Deflate support. JPEG/OJPEG, JBIG, LERC, LZMA, Zstd and WebP-in-TIFF remain disabled by that package's generated configuration.
