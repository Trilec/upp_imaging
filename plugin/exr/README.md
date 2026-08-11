# plugin/exr

Opt-in display-oriented OpenEXR integration for ordinary U++ `StreamRaster` and `Upp::Image` workflows.

The plugin reads EXR bytes from the supplied U++ `Stream` through OpenImageIO's in-memory `IOProxy`; it does not create temporary files and does not expose OpenImageIO types in its public header.

Initial preview policy:

- ordinary single-image, single-mip, non-deep 2D EXR only;
- named `R`, `G`, `B` channels, optional alpha, named Gray/GrayAlpha, or one-channel mask previews;
- named extra channels are ignored when an unambiguous RGB triplet is present;
- source samples are converted to straight RGBA8 by clamping finite values to `[0,1]` and rounding to the nearest 8-bit value;
- non-finite preview samples map to zero;
- no colour transform, exposure adjustment or tone mapping is applied.

This package is intentionally not a full-fidelity EXR API. It does not preserve floating-point samples, arbitrary channels, metadata, source data-window coordinates, multipart/deep/mip structures, or HDR values outside the display clamp. Use `ImagingIO`, `OpenImageIO`, or `openexr` when those semantics matter.
