# Status And Roadmap

## Current framework status

- Architecture and documentation pivot: complete.
- Public OpenImageIO and OpenColorIO package renames: complete.
- ImagingCore: implemented and validated.
- ImagingIO: implemented and hardened for the supported EXR/PNG slice; final Windows acceptance pending.
- ImagingColor: next framework package.
- ImagingAnalysis: planned after ImagingColor.
- ImagingDiagnostics: planned after ImagingAnalysis.
- Imaging umbrella: planned after the five framework packages are complete.
- plugin/exr: planned as an opt-in display-oriented bridge after the framework is established.
- LumaPix: paused; retained as reference material only.

## ImagingIO supported subset

- one image and one mip level only
- non-deep, two-dimensional images only
- EXR Float16 and Float32
- EXR Gray, GrayAlpha, RGB, RGBA, alpha-only and named MultiChannel
- EXR non-zero and negative data-window origins
- PNG UInt8 and UInt16
- PNG Gray, GrayAlpha, RGB and RGBA at zero origin
- transactional loads and same-directory transactional saves
- typed scalar and homogeneous numeric-array metadata
- explicit backend-managed metadata omission policy

Unsupported structures and formats fail closed with stable diagnostics: multipart, mipmapped, deep, volume, mixed-channel-format, integer EXR, floating PNG and arbitrary PNG multichannel files.

## Next implementation order

1. Final Windows acceptance of ImagingIO.
2. ImagingColor through OpenColorIO.
3. ImagingAnalysis.
4. ImagingDiagnostics.
5. Imaging umbrella.
6. plugin/exr.
7. JPEG XL.
8. HDR/RGBE.
9. DPX/Cineon.
10. RAW image support.
11. WebP.
12. HEIF/AVIF.
13. Additional TIFF/OpenImageIO coverage.
14. FFmpeg as a separate major milestone.

For every additional format:

1. package and validate required upstream dependencies;
2. compile and register the OpenImageIO plugin where applicable;
3. validate direct OpenImageIO loading and saving;
4. validate the ImagingIO path;
5. add a format-specific plugin only where ordinary Upp::Image workflows benefit.

## Dependency direction

- ImagingCore depends on U++ Core only.
- ImagingIO depends on ImagingCore and OpenImageIO; OIIO types remain private.
- ImagingColor will depend on ImagingCore and OpenColorIO; OCIO types remain private.
- ImagingAnalysis depends on ImagingCore.
- ImagingDiagnostics depends on ImagingCore and remains GUI-independent.
- Imaging depends on all five framework packages.
- plugin/exr remains opt-in and is not included automatically by Imaging.
