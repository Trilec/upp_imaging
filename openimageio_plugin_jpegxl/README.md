# openimageio_plugin_jpegxl

Static OpenImageIO 3.1.15.0 JPEG XL plugin registration for the U++ imaging nest.

The codec implementation is the upstream `jpegxl.imageio` reader/writer paired with
the repository-pinned `jpegxl_src` libjxl 0.12.0 backend. The package is internal
to the stable `OpenImageIO` wrapper and does not expose libjxl types through the
framework APIs.

Initial framework acceptance is intentionally stricter than the raw backend: the
ImagingIO layer will admit only structures and channel layouts it can represent
without semantic loss, and will verify completed temporary files before replacing
a destination.
