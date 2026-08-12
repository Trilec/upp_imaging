# openimageio_plugin_heif

Static OpenImageIO 3.1.15.0 HEIF-family input registration backed by `libheif_src` 1.23.1.

Supported registry extensions are `.heic`, `.heif`, `.heics`, `.hif` and `.avif`. The exact upstream OIIO `heifinput.cpp` is compiled from `openimageio_plugins_src`; no OIIO HEIF writer is compiled or registered.

The underlying libheif package contains only dav1d AV1 and libde265 HEVC decoders. Therefore AVIF and HEIC/HEIF are input-only in this slice. The OIIO reader can expose 8-bit data as UInt8 and 10/12-bit data as UInt16, preserves straight alpha when `oiio:UnassociatedAlpha=1`, and exposes additional top-level images as subimages for the framework layer to reject or handle explicitly.
