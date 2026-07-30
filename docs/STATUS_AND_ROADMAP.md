# Status And Roadmap

| Area | Version | Strict package | User package | Helper | Automated coverage | Viewer | Status |
| --- | ---: | --- | --- | --- | --- | --- | --- |
| zlib | 1.3.2 strict / 1.3.1 Windows provider | `zlib_src` | `zlib` | — | Green | — | Foundation complete |
| libpng | 1.6.58 | `libpng_src` | `libpng` | `png_io` | Green | Green | RGBA8 subset complete |
| Imath | 3.2.2 | `imath_src` | `imath` | — | Green | — | Foundation complete |
| libdeflate | 1.25 | `libdeflate_src` | `libdeflate` | — | Green | — | Foundation complete |
| OpenJPH | 0.26.3 | `openjph_src` | `openjph` | — | Green | — | Foundation complete |
| Iex | OpenEXR 3.4.13 stack | `iex_src` | — | — | Green | — | Lower layer complete |
| IlmThread | OpenEXR 3.4.13 stack | `ilmthread_src` | — | — | Green | — | Lower layer complete |
| OpenEXRCore | 3.4.13 | `openexr_core_src` | `openexr_core` | `openexr_io` | Green | Green | Scanline RGBA subset complete |
| OpenEXR high-level | 3.4.13 | `openexr_src` | `openexr` | — | Strict and stable exact RGBA HALF ZIP round-trip | — | Stable package boundary complete |
| JPEG | 3.2.0 | `libjpeg_turbo_src` | `libjpeg_turbo` | `jpeg_io` | Green | Green | RGB8 lossy subset complete |
| TIFF | 4.7.2 | `libtiff_src` | `libtiff` | `tiff_io` | Green | Green | Typed RGBA subset complete |
| OpenColorIO | 2.5.2 | `opencolorio_src` | `opencolorio` | — | Green | Green | Packaged and validated |
| OpenImageIO | 3.1.15.0 | `openimageio_src` | `oiio` | — | Green (EXR+PNG) | Green | Source/application boundary complete; EXR and PNG integration validated; JPEG/TIFF/dynamic loading not validated |
| `ImagingCore` | — | — | planned | — | — | — | Backend-neutral image data model; design target documented |
| `ImagingIO` | — | — | planned | — | — | — | OpenImageIO-backed full-fidelity I/O; design target documented |
| `ImagingColor` | — | — | planned | — | — | — | OpenColorIO-backed colour processing; design target documented |
| `ImagingAnalysis` | — | — | planned | — | — | — | Histograms, statistics, probes; design target documented |
| `ImagingDiagnostics` | — | — | planned | — | — | — | Shared structured reporting; design target documented |
| `Imaging` (umbrella) | — | — | planned | — | — | — | Convenience umbrella for standard stack; design target documented |
| `plugin/exr` | — | — | planned | — | — | — | Opt-in `Upp::Image` integration; design target documented |
| LumaPix | — | — | paused | — | — | — | Reader proof completed; paused; reference material for `ImagingCore` and `ImagingIO` |

## Current completed format subsets

- exact EXR, PNG, and TIFF validation
- metric-based JPEG validation
- EXR scanline RGBA `HALF` and `FLOAT`
- EXR `NONE` and `ZIP` compression
- PNG straight RGBA8 read/write
- JPEG RGB8 load/save
- JPEG Q95 4:4:4 baseline validation
- JPEG MAE/RMSE/PSNR acceptance
- JPEG progressive validation
- TIFF RGBA8, RGBA16, and Float32 load/save
- TIFF `NONE`, `LZW`, and `Deflate`
- TIFF exact comparisons
- classic single-directory scanline TIFF subset
- shared viewer with `Generated`, `Reloaded`, and `Difference` panes
- OpenColorIO 2.5.2 core packaged
- OpenColorIO CPU processing validated
- OpenColorIO GPU GLSL and HLSL extraction validated
- OpenColorIO built-in CG and Studio configs validated
- OpenColorIO CPU preview integration added to the diagnostic viewer
- OpenColorIO preview error recovery validated
- OpenColorIO raw round-trip metrics remain unchanged
- OpenColorIO actual GPU rendering not implemented
- OpenColorIO export-through-OCIO not implemented
- OpenColorIO system-monitor enumeration currently headless
- OpenImageIO 3.1.15.0 source/application boundary complete
- static OpenEXR and PNG OpenImageIO plugins integrated
- `openimageio_io_test` validates the EXR/PNG integration path
- `ImagingWorkbench` is the diagnostic integration application for the complete stack

## Currently validated OpenImageIO formats

- **OpenEXR**
- **PNG**

JPEG, TIFF and other formats are not validated through the OIIO boundary.

## Next implementation order

1. Complete the framework documentation and public naming — **in progress**
2. Perform controlled `oiio` to `OpenImageIO` and `opencolorio` to `OpenColorIO` package renaming
3. Establish `ImagingCore` and `ImagingIO` using the proven LumaPix contracts
4. Establish `ImagingColor`, `ImagingAnalysis` and `ImagingDiagnostics` boundaries
5. Add `plugin/exr`
6. Add JPEG XL support
7. Add HDR/RGBE support
8. Add DPX support
9. Add RAW image support
10. Add WebP support
11. Add HEIF and AVIF support
12. Expand TIFF coverage where useful
13. Treat FFmpeg and moving-image support as a separate larger milestone

## Format priority

Prioritise formats not already adequately supported by standard U++ plugins:

- JPEG XL, HDR, DPX, RAW, WebP, HEIF and AVIF come before duplicating existing ordinary PNG, JPEG or TIFF U++ raster support.

For each new format the sequence is:

1. package and validate required upstream dependencies
2. compile and register the OpenImageIO format plugin where applicable
3. validate direct OpenImageIO loading and saving
4. validate the ImagingIO path
5. add a format-specific `plugin/*` package only where useful for `Upp::Image` workflows

## LumaPix disposition

`upp_lumapix` is paused after completing its OpenImageIO reader proof. That work demonstrated:

- backend-neutral image specifications
- typed image buffers
- metadata translation
- channel-order handling
- data-window preservation
- OIIO isolation behind a private implementation
- deterministic generated-fixture testing

It is reference material for the future `ImagingCore` and `ImagingIO` migration. `upp_imaging` does not depend on `upp_lumapix`. The LumaPix name is reserved for a possible future image-processing application built on `upp_imaging`.
