# openimageio_plugin_png

Statically registers the pinned OpenImageIO 3.1.15.0 PNG plugin sources with
OpenImageIO. The package exposes the OIIO creator/extension registration
without dynamic plugin lookup. The `libpng/libpng16/png.h` shim adapts the
stable libpng header layout without duplicating implementation files.
