# openimageio_plugin_openexr

Statically registers the OpenEXR format with OpenImageIO. The current U++
adapter uses the repository's existing `openexr_io` implementation and exposes
the OIIO creator/extension registration without dynamic plugin lookup.
