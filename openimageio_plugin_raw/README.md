# openimageio_plugin_raw

Static OpenImageIO 3.1.15.0 RAW input registration backed by repository-pinned LibRaw 0.22.2.

The plugin compiles the exact upstream `src/raw.imageio/rawinput.cpp` from the OIIO source pin already owned by `openimageio_plugin_dpxcineon`; it does not carry another copy of OpenImageIO source.

## Policy

- input only; there is no RAW writer
- LibRaw's default OIIO path produces processed/demosaiced UInt16 image data
- camera metadata exposed by OIIO is retained for the framework layer to translate where representable
- `.hdr` is deliberately removed from the RAW extension registration because `OpenImageIO` already registers Radiance HDR/RGBE as the authoritative `.hdr` handler
- no RawSpeed, Adobe DNG SDK, LCMS or external JPEG dependency is enabled by this package

The remaining upstream RAW extensions are registered statically through `OpenImageIO::InitializeOpenImageIO()`.
