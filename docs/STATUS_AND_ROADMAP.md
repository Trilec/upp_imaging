# Status And Roadmap

## Current framework status

- Architecture and documentation pivot: complete.
- Public OpenImageIO and OpenColorIO package renames: complete.
- ImagingCore: implemented and validated.
- ImagingIO: implemented and hardened for the supported EXR/PNG slice; public Windows contract is green. The independent OIIO cross-check was aligned with the capabilities of the bundled writer and no longer blocks framework progress.
- ImagingColor: implemented and Windows-accepted with OpenColorIO as the private backend.
- ImagingAnalysis: implemented code-side as a Core-only statistics, histogram and source-probe layer; focused Windows acceptance pending.
- ImagingDiagnostics: next framework implementation package.
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

## ImagingAnalysis supported subset

- Core-only dependency boundary with no OpenImageIO, OpenColorIO or GUI types
- per-channel finite/non-finite counts, min/max and mean
- normalized `[0,1]` histograms with explicit below/above-range evidence
- source-coordinate pixel probes across non-zero data windows and depth slices
- UInt8 and UInt16 normalized to `[0,1]`
- Float16 and Float32 analyzed in source numeric space
- canonical and named MultiChannel images, with every channel analyzed
- transactional failure outputs and stable `IMGANALYSIS_*` diagnostics

Waveforms and vectorscopes remain later ImagingAnalysis scope. Workbench GUI controls remain application-level and are not dependencies of the framework package.

## Next implementation order

1. Focused Windows acceptance of ImagingAnalysis.
2. ImagingDiagnostics.
3. Imaging umbrella.
4. plugin/exr.
5. JPEG XL.
6. HDR/RGBE.
7. DPX/Cineon.
8. RAW image support.
9. WebP.
10. HEIF/AVIF.
11. Additional TIFF/OpenImageIO coverage.
12. FFmpeg as a separate major milestone.

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
- ImagingAnalysis depends on ImagingCore only.
- ImagingDiagnostics depends on ImagingCore and remains GUI-independent.
- Imaging depends on all five framework packages.
- plugin/exr remains opt-in and is not included automatically by Imaging.
