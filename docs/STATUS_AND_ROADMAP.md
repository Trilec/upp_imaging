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
| OpenColorIO | 2.5.2 | `opencolorio_src` | `OpenColorIO` | — | Green | Green | Packaged and validated |
| OpenImageIO headers | 3.1.15.0 | `openimageio_headers` | `openimageio_src`, `openimageio_util_src` | — | Green | Green | Internal strict header-routing package owning the upstream `OpenImageIO/` tree |
| OpenImageIO | 3.1.15.0 | `openimageio_src` | `OpenImageIO` | `oiio` compatibility forwarder | Green (EXR+PNG) | Green | Canonical public application package; source/application boundary complete; EXR and PNG integration validated; JPEG/TIFF/dynamic loading not validated |
| Architecture and package naming | — | — | — | — | — | — | **Architecture/documentation pivot complete**; `OpenImageIO` and `OpenColorIO` public renames complete; `oiio` remains an OIIO compatibility forwarder; Windows cannot host a case-only OCIO compatibility package; framework migration is in progress |
| `ImagingCore` | — | — | `ImagingCore` | — | Green | — | Backend-neutral image data model and result contracts implemented; Core-only boundary validated |
| `ImagingIO` | — | — | planned | — | — | — | Planned initial backend: OpenImageIO; design target documented; not implemented |
| `ImagingColor` | — | — | planned | — | — | — | Planned initial backend: OpenColorIO; design target documented; not implemented |
| `ImagingAnalysis` | — | — | planned | — | — | — | Initial scope: histograms, channel statistics, source probes, finite and non-finite value handling; later scope includes waveform and vectorscope; design target documented; not implemented |
| `ImagingDiagnostics` | — | — | planned | — | — | — | Will depend on `ImagingCore`; GUI-independent; design target documented; not implemented |
| `Imaging` (umbrella) | — | — | planned | — | — | — | Convenience umbrella for standard stack; design target documented; not implemented |
| `plugin/exr` | — | — | planned | — | — | — | Opt-in `Upp::Image` integration; design target documented; not implemented |
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

## Milestone status

- **Architecture and documentation pivot: complete.** The three-layer model, `Upp::Imaging` framework design, `plugin/exr` scope, LumaPix disposition, and format roadmap are defined and documented. The public renames are complete.
- **Package renames:** the public `oiio` → `OpenImageIO` and `opencolorio` → `OpenColorIO` renames are complete. `oiio` remains a compatibility forwarder; no case-only OCIO compatibility package is possible on Windows. The strict packages remain `openimageio_headers` and `opencolorio_src`.
- **Framework migration:** `ImagingCore` is implemented and validated. `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics` and the `Imaging` umbrella remain planned.

## Next implementation order

1. **Complete the architecture and documentation pivot** — **done** (including the `OpenImageIO` and `OpenColorIO` public renames)
2. **Framework migration** — **in progress**; `ImagingCore` is implemented, followed by `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics` and the `Imaging` umbrella using the proven LumaPix contracts
3. `plugin/exr` — opt-in raster integration into ordinary U++ `Upp::Image` workflows
4. JPEG XL support
5. HDR/RGBE support
6. DPX/Cineon support
7. RAW image support
8. WebP support
9. HEIF/AVIF support
10. Additional TIFF/OIIO coverage — expand TIFF handling where useful and route additional formats through the OIIO boundary
11. FFmpeg as a separate major milestone

JPEG XL, HDR, DPX/Cineon, RAW, WebP, HEIF and AVIF come before duplicating existing ordinary PNG, JPEG or TIFF U++ raster support.

For each new format the sequence is:

1. package and validate required upstream dependencies
2. compile and register the OpenImageIO format plugin where applicable
3. validate direct OpenImageIO loading and saving
4. validate the ImagingIO path
5. add a format-specific `plugin/*` package only where useful for `Upp::Image` workflows

## ImagingAnalysis scope

**Initial scope:**

- histograms
- channel statistics
- source probes
- finite and non-finite value handling

**Later scope:**

- waveform analysis
- vectorscope analysis
- other reusable scopes

The initial framework-establishment task does not require waveform or vectorscope extraction.

## Planned framework dependency direction

When the framework packages are implemented:

- `ImagingCore` depends on U++ Core
- `ImagingIO` depends on `ImagingCore` and `OpenImageIO`
- `ImagingColor` depends on `ImagingCore` and `OpenColorIO`
- `ImagingAnalysis` depends on `ImagingCore`
- `ImagingDiagnostics` depends on `ImagingCore`
- `Imaging` umbrella depends on all five framework packages
- `plugin/exr` remains opt-in and is not included automatically by `Imaging`

## Planned public include convention

When the framework packages are implemented, public headers will live at the package root:

- `<ImagingCore/ImagingCore.h>`
- `<ImagingIO/ImagingIO.h>`
- `<ImagingColor/ImagingColor.h>`
- `<ImagingAnalysis/ImagingAnalysis.h>`
- `<ImagingDiagnostics/ImagingDiagnostics.h>`
- `<Imaging/Imaging.h>`

All public types remain in the namespace `Upp::Imaging`.

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
