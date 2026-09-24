# openimageio_plugin_webp

Static OpenImageIO 3.1.17.0 WebP registration backed by repository-pinned libwebp 1.6.0.

The reader is compiled unchanged from `openimageio_plugins_src`. The writer remains aligned with the OIIO 3.1.17.0 implementation except that lossless output sets `WebPConfig::exact=1`, ensuring hidden RGB values beneath fully transparent pixels are preserved rather than rewritten for compression.


Current validation targets and results are listed in `tests/acceptance.txt` and `docs/WINDOWS_ACCEPTANCE.md` at the repository root.
