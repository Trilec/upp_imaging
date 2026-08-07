# Status And Roadmap

## Current framework status

- Architecture and documentation pivot: complete.
- Public OpenImageIO and OpenColorIO package renames: complete.
- ImagingCore: implemented and validated.
- ImagingIO: implemented and hardened for the supported EXR/PNG slice; final Windows closure matrix remains pending.
- ImagingColor: implemented code-side with OpenColorIO as the private backend; Windows acceptance pending.
- ImagingAnalysis: next framework implementation package after ImagingColor acceptance.
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

## ImagingColor supported subset

- built-in OpenColorIO configurations and explicit OCIO config files
- backend-neutral config inspection for colour spaces, displays, looks and defaults
- named source-to-destination colour-space transforms
- source/display/view display transforms
- RGB and RGBA layouts
- named MultiChannel images with one unambiguous `R`, `G`, `B` triplet
- UInt8, UInt16, Float16 and Float32 storage
- all depth slices processed independently
- exact preservation of alpha and non-RGB channels
- preservation of image specification, data window, channel names, sample type and metadata
- transactional failures: output is not changed unless processing completes successfully

Gray and GrayAlpha transforms are deliberately unsupported in the initial slice because converting an RGB colour result back to one luminance channel would require an application policy. Ambiguous MultiChannel RGB mappings are rejected with stable diagnostics.

## Next implementation order

1. Final Windows acceptance of ImagingIO and ImagingColor.
2. ImagingAnalysis.
3. ImagingDiagnostics.
4. Imaging umbrella.
5. plugin/exr.
6. JPEG XL.
7. HDR/RGBE.
8. DPX/Cineon.
9. RAW image support.
10. WebP.
11. HEIF/AVIF.
12. Additional TIFF/OpenImageIO coverage.
13. FFmpeg as a separate major milestone.

For every additional format:

1. package and validate required upstream dependencies;
2. compile and register the OpenImageIO plugin where applicable;
3. validate direct OpenImageIO loading and saving;
4. validate the ImagingIO path;
5. add a format-specific plugin only where ordinary Upp::Image workflows benefit.

## Dependency direction

- ImagingCore depends on U++ Core only.
- ImagingIO depends on ImagingCore and OpenImageIO; OIIO types remain private.
- ImagingColor depends on ImagingCore and OpenColorIO; OCIO types remain private.
- ImagingAnalysis depends on ImagingCore.
- ImagingDiagnostics depends on ImagingCore and remains GUI-independent.
- Imaging depends on all five framework packages.
- plugin/exr remains opt-in and is not included automatically by Imaging.
