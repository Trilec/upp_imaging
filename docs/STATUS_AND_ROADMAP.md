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
| JPEG | 3.2.0 | `libjpeg_turbo_src` | `libjpeg_turbo` | `jpeg_io` | Green | Green | RGB8 lossy subset complete |
| TIFF | 4.7.2 | `libtiff_src` | `libtiff` | `tiff_io` | Green | Green | Typed RGBA subset complete |
| OpenColorIO | 2.5.2 | `opencolorio_src` | `opencolorio` | `opencolorio_src_test`, `opencolorio_test`, `opencolorio_gui_link_test` | Green | Green | Core packaged |
| OpenImageIO | planned | — | — | — | — | — | Format abstraction |
| LumaPix adapter | planned | — | — | — | — | — | Final integration |

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
- OpenColorIO actual GPU rendering not implemented
- OpenColorIO system-monitor enumeration currently headless

## Next implementation order

1. OCIO integration in the diagnostic image viewer
2. OpenImageIO focused integration proof
3. initial OIIO plugins: EXR, PNG, JPEG, TIFF
4. LumaPix adapter

OpenColorIO 2.5.2 is now packaged and validated.

We are not manually implementing every format supported by OpenImageIO before packaging OpenImageIO.

The direct format work establishes:

- proven dependencies
- known-working codec packages
- test images
- comparison policies
- diagnostic tooling
- fallback helpers
