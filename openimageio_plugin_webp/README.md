# openimageio_plugin_webp

Static OpenImageIO 3.1.15.0 WebP registration backed by repository-pinned libwebp 1.6.0.

The reader is compiled unchanged from `openimageio_plugins_src`. The writer remains aligned with the OIIO 3.1.15.0 implementation except that lossless output sets `WebPConfig::exact=1`, ensuring hidden RGB values beneath fully transparent pixels are preserved rather than rewritten for compression.

The direct backend supports RGB/RGBA, static and animated WebP reading, lossy or lossless static output, EXIF/ICC handling exposed by OIIO, and straight-alpha preservation through `oiio:UnassociatedAlpha`. `ImagingIO` deliberately applies a narrower static, lossless policy.
