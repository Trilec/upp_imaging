# Status And Roadmap

| Area | Version | Strict package | User package | Helper | Automated coverage | Viewer | Status |
| --- | ---: | --- | --- | --- | --- | --- | --- |
| zlib | 1.3.2 | `zlib_src` | `zlib` | — | Green | — | Foundation complete |
| libpng | 1.6.58 | `libpng_src` | `libpng` | `png_io` | Green | Green | RGBA8 subset complete |
| Imath | 3.2.2 | `imath_src` | `imath` | — | Green | — | Foundation complete |
| libdeflate | 1.25 | `libdeflate_src` | `libdeflate` | — | Green | — | Foundation complete |
| OpenJPH | 0.26.3 | `openjph_src` | `openjph` | — | Green | — | Foundation complete |
| Iex | OpenEXR 3.4.13 stack | `iex_src` | — | — | Green | — | Lower layer complete |
| IlmThread | OpenEXR 3.4.13 stack | `ilmthread_src` | — | — | Green | — | Lower layer complete |
| OpenEXRCore | 3.4.13 | `openexr_core_src` | `openexr_core` | `openexr_io` | Green | Green | Scanline RGBA subset complete |
| JPEG | planned | — | — | — | — | — | Next |
| TIFF | planned | — | — | — | — | — | After JPEG |
| OpenColorIO | planned | — | — | — | — | — | Later foundation |
| OpenImageIO | planned | — | — | — | — | — | Format abstraction |
| LumaPix adapter | planned | — | — | — | — | — | Final integration |

## Current completed format subsets

- EXR scanline RGBA `HALF` and `FLOAT`
- EXR `NONE` and `ZIP` compression
- PNG straight RGBA8 read/write
- exact numerical round-trip tests for both supported helper paths
- shared viewer with `Generated`, `Reloaded`, and `Difference` panes

## Next implementation order

1. `libjpeg-turbo` source and user-facing packages
2. reusable JPEG IO
3. lossy comparison rules: tolerance, RMSE, PSNR
4. JPEG viewer profile
5. `libtiff` source and user-facing packages
6. reusable TIFF IO
7. TIFF 8-bit, 16-bit, and float validation where practical
8. OpenColorIO CPU foundation
9. OpenImageIO reduced build
10. initial OIIO plugins: EXR, PNG, JPEG, TIFF
11. OCIO integration with the higher-level image stack
12. LumaPix adapter

We are not manually implementing every format supported by OpenImageIO before packaging OpenImageIO.

The direct format work establishes:

- proven dependencies
- known-working codec packages
- test images
- comparison policies
- diagnostic tooling
- fallback helpers
