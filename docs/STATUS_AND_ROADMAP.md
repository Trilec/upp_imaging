# Status And Roadmap

## Current framework status

- Architecture and documentation pivot: complete.
- Public OpenImageIO and OpenColorIO package renames: complete.
- ImagingCore: implemented and validated.
- ImagingIO: EXR/PNG contract is Windows-accepted; JPEG XL support is implemented code-side and awaiting focused Windows acceptance.
- ImagingColor: implemented and Windows-accepted with OpenColorIO as the private backend.
- ImagingAnalysis: implemented and Windows-accepted as a Core-only statistics, histogram and source-probe layer.
- ImagingDiagnostics: implemented and Windows-accepted as a Core-only deterministic comparison and structured reporting layer.
- Imaging umbrella: implemented and Windows-accepted as the standard forwarding package over all five framework packages.
- plugin/exr: implemented code-side as an opt-in display-oriented `StreamRaster` bridge; focused Windows acceptance is tracked independently.
- JPEG XL: pinned libjxl 0.12.0 backend, static OpenImageIO registration and strict ImagingIO slice are implemented code-side; Windows acceptance is pending.
- LumaPix: paused; retained as reference material only.

## ImagingIO supported subset

- one image and one mip level only
- non-deep, two-dimensional images only
- EXR Float16 and Float32
- EXR Gray, GrayAlpha, RGB, RGBA, alpha-only and named MultiChannel
- EXR non-zero and negative data-window origins
- PNG UInt8 and UInt16
- PNG Gray, GrayAlpha, RGB and RGBA at zero origin
- JPEG XL UInt8, UInt16, Float16 and Float32
- JPEG XL Gray, RGB and RGBA at zero origin
- JPEG XL output is explicitly lossless
- transactional loads and same-directory transactional saves
- completed save candidates are reopened, specification-checked and fully decoded before destination replacement
- typed scalar and homogeneous numeric-array metadata
- explicit backend-managed metadata omission policy

Unsupported structures and formats fail closed with stable diagnostics: multipart, mipmapped, deep, volume, mixed-channel-format, integer EXR, floating PNG, arbitrary PNG/JPEG XL multichannel files, JPEG XL GrayAlpha, and non-zero-origin PNG/JPEG XL.

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

## ImagingDiagnostics supported subset

- Core-only dependency boundary with no OpenImageIO, OpenColorIO or GUI types
- absolute/relative scalar numerical comparisons with deterministic NaN/infinity handling
- numerical-array comparison summaries with mismatch count, first mismatch and maximum errors
- stable names for ImagingCore sample types, channel layouts, result codes and diagnostic severities
- deterministic structured reports for image specifications, metadata, results and diagnostics
- operation reports combining operation name, elapsed milliseconds, result and ordered diagnostics
- sorted metadata keys and deterministic text rendering for tests, logs and future ImagingWorkbench presentation
- transactional failures with stable `IMGDIAG_*` diagnostics

`ImagingCore::Diagnostics` remains the authoritative diagnostics container. ImagingDiagnostics formats and compares existing Core contracts rather than duplicating backend or GUI state.

## plugin/exr preview subset

- opt-in `StreamRaster` integration; it is not pulled in by the `Imaging` umbrella
- reads encoded EXR data from the supplied U++ `Stream` through OpenImageIO `IOProxy`, with no temporary files
- ordinary single-image, single-mip, non-deep 2D EXR only
- named RGB/RGBA, Gray/GrayAlpha, one-channel masks, and named MultiChannel images with an unambiguous RGB triplet
- straight RGBA8 preview output with finite source values clamped to `[0,1]` and rounded to 8-bit
- no implicit colour transform, exposure adjustment or tone mapping
- no claim to preserve floating-point samples, arbitrary channels, metadata, source data-window coordinates, multipart/deep/mip structure or HDR values outside the display clamp

Full-fidelity EXR work remains the responsibility of ImagingIO or the direct OpenImageIO/OpenEXR APIs.

## JPEG XL implementation

- pins upstream libjxl 0.12.0 at commit `a7a9c787341cf703dede03c2009fa460cae5e5df`
- uses libjxl's recursively pinned Brotli 1.2.0, Highway 1.2.0 and skcms source dependencies
- builds the codec directly as a U++ source package through `import.ext`; no system libjxl or CMake build step
- enables JPEG XL container boxes and disables lossless JPEG reconstruction/transcoding
- uses repository-owned static export/version headers in place of CMake-generated headers
- registers OpenImageIO 3.1.15.0's JPEG XL reader/writer statically through the stable `OpenImageIO` package
- focused direct tests cover backend versioning, thread runner, lossless encode/decode, OIIO registration, RGB/RGBA alpha preservation and malformed-input refusal
- focused ImagingIO tests cover the accepted sample/layout matrix, exact lossless pixels, stable refusal diagnostics and transactional replacement

JPEG XL GrayAlpha and arbitrary extra-channel data remain deferred until the backend adapter can expose their semantics without ambiguity.

## Next implementation order

1. Windows acceptance of the completed JPEG XL slice may proceed in parallel with further code-side work.
2. HDR/RGBE.
3. DPX/Cineon.
4. RAW image support.
5. WebP.
6. HEIF/AVIF.
7. Additional TIFF/OpenImageIO coverage.
8. FFmpeg as a separate major milestone.

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
- ImagingDiagnostics depends on ImagingCore only and remains GUI-independent.
- Imaging depends on all five framework packages.
- format backend source packages do not depend on the framework layer.
- plugin/exr remains opt-in and is not included automatically by Imaging.
