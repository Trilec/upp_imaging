# Package catalogue

Package names are stable across nests. See [layout](package_layout.md) for assembly configuration. Tests listed below are selected by [acceptance.txt](../tests/acceptance.txt).

| Package | Physical directory | Current purpose |
| --- | --- | --- |
| `ImagingWorkbench` | `apps/ImagingWorkbench` | Interactive imaging workbench MVP |
| `Imaging` | `imaging/Imaging` | Standard complete U++ Imaging framework umbrella |
| `ImagingAnalysis` | `imaging/ImagingAnalysis` | Backend-neutral image analysis |
| `ImagingColor` | `imaging/ImagingColor` | Backend-neutral colour processing through OpenColorIO |
| `ImagingCore` | `imaging/ImagingCore` | Backend-neutral U++ Imaging core contracts |
| `ImagingDiagnostics` | `imaging/ImagingDiagnostics` | Core-only structured imaging diagnostics and reporting |
| `ImagingIO` | `imaging/ImagingIO` | Backend-neutral EXR, PNG, JPEG XL, HDR, DPX, Cineon, RAW, WebP, HEIF/AVIF and TIFF image I/O |
| `imaging_histogram` | `integrations/imaging_histogram` | Source-referred histogram analysis data model |
| `imaging_preview_coalescing` | `integrations/imaging_preview_coalescing` | Pure preview render coalescing helper for ImagingWorkbench |
| `imaging_roundtrip_test_support` | `integrations/imaging_roundtrip_test_support` | Reusable synthetic image test support |
| `imaging_roundtrip_viewer_ocio` | `integrations/imaging_roundtrip_viewer_ocio` | Workbench preview and diagnostic support |
| `imaging_tone_conversion` | `integrations/imaging_tone_conversion` | Shared tone conversion helpers for ImagingWorkbench |
| `imaging_view_transform` | `integrations/imaging_view_transform` | Pure source/view transform helpers for ImagingWorkbench |
| `plugin/exr` | `integrations/plugin/exr` | Opt-in OpenEXR StreamRaster preview bridge |
| `dpx_cineon_oiio_test` | `tests/dpx_cineon_oiio_test` | Focused OpenImageIO DPX and Cineon contract test |
| `expat_test` | `tests/expat_test` | Stable Expat user-facing probe |
| `ffmpeg_avcodec_test` | `tests/ffmpeg_avcodec_test` | Current public-contract regression |
| `ffmpeg_avformat_test` | `tests/ffmpeg_avformat_test` | Current public-contract regression |
| `ffmpeg_avutil_test` | `tests/ffmpeg_avutil_test` | Current public-contract regression |
| `ffmpeg_first_frame_test` | `tests/ffmpeg_first_frame_test` | Current public-contract regression |
| `ffmpeg_headers_test` | `tests/ffmpeg_headers_test` | Current public-contract regression |
| `ffmpeg_swscale_test` | `tests/ffmpeg_swscale_test` | Current public-contract regression |
| `hdr_dpx_imagingio_test` | `tests/hdr_dpx_imagingio_test` | Focused ImagingIO HDR, DPX and Cineon contract test |
| `hdr_oiio_test` | `tests/hdr_oiio_test` | Focused OpenImageIO Radiance HDR/RGBE registration test |
| `heif_imagingio_test` | `tests/heif_imagingio_test` | Focused ImagingIO HEIF/AVIF input-only policy test |
| `heif_oiio_test` | `tests/heif_oiio_test` | Focused OpenImageIO HEIF/AVIF decode-only registration test |
| `imaging_analysis_test` | `tests/imaging_analysis_test` | ImagingAnalysis public contract test |
| `imaging_color_ocio_test` | `tests/imaging_color_ocio_test` | Independent OpenColorIO acceptance test for ImagingColor |
| `imaging_color_test` | `tests/imaging_color_test` | ImagingColor public contract test |
| `imaging_core_test` | `tests/imaging_core_test` | ImagingCore contract test |
| `imaging_diagnostics_test` | `tests/imaging_diagnostics_test` | ImagingDiagnostics public contract test |
| `imaging_histogram_test` | `tests/imaging_histogram_test` | Tests for source-referred histogram analysis |
| `imaging_io_oiio_test` | `tests/imaging_io_oiio_test` | Independent OpenImageIO cross-check for ImagingIO |
| `imaging_io_test` | `tests/imaging_io_test` | ImagingIO public EXR and PNG contract test |
| `imaging_preview_coalescing_test` | `tests/imaging_preview_coalescing_test` | Deterministic tests for preview render coalescing |
| `imaging_roundtrip_viewer_ocio_smoke_test` | `tests/imaging_roundtrip_viewer_ocio_smoke_test` | Current public-contract regression |
| `imaging_test` | `tests/imaging_test` | Imaging umbrella contract test |
| `imaging_tone_conversion_test` | `tests/imaging_tone_conversion_test` | Deterministic tone conversion test |
| `imaging_view_transform_test` | `tests/imaging_view_transform_test` | Deterministic source/view transform test |
| `imaging_workbench_ocio_test` | `tests/imaging_workbench_ocio_test` | Current public-contract regression |
| `imath_test` | `tests/imath_test` | User-facing Imath package test |
| `jpegxl_imagingio_test` | `tests/jpegxl_imagingio_test` | Focused ImagingIO JPEG XL contract test |
| `jpegxl_oiio_test` | `tests/jpegxl_oiio_test` | Focused OpenImageIO JPEG XL registration and roundtrip test |
| `libdeflate_test` | `tests/libdeflate_test` | User-facing libdeflate package test |
| `libjpeg_turbo_test` | `tests/libjpeg_turbo_test` | User-facing libjpeg-turbo probe |
| `libpng_roundtrip_test` | `tests/libpng_roundtrip_test` | User-facing libpng memory round-trip test |
| `libtiff_test` | `tests/libtiff_test` | User-facing libtiff probe |
| `minizip_ng_test` | `tests/minizip_ng_test` | Stable minizip-ng user-facing probe |
| `opencolorio_test` | `tests/opencolorio_test` | Stable OpenColorIO user-facing probe |
| `openexr_core_rgba_zip_test` | `tests/openexr_core_rgba_zip_test` | OpenEXRCore RGBA ZIP round-trip |
| `openexr_test` | `tests/openexr_test` | Stable OpenEXR 3.4.13 high-level file round-trip test |
| `openimageio_io_test` | `tests/openimageio_io_test` | Deterministic OpenImageIO IO test |
| `plugin_exr_test` | `tests/plugin_exr_test` | plugin/exr StreamRaster preview contract |
| `pystring_test` | `tests/pystring_test` | Stable pystring user-facing probe |
| `raw_imagingio_test` | `tests/raw_imagingio_test` | Focused ImagingIO camera RAW policy contract test |
| `raw_oiio_test` | `tests/raw_oiio_test` | Focused OpenImageIO RAW registration contract test |
| `tiff_imagingio_test` | `tests/tiff_imagingio_test` | Focused ImagingIO TIFF policy and transaction test |
| `tiff_oiio_test` | `tests/tiff_oiio_test` | Focused OpenImageIO TIFF registration and fidelity test |
| `webp_imagingio_test` | `tests/webp_imagingio_test` | Focused ImagingIO WebP policy and transaction test |
| `webp_oiio_test` | `tests/webp_oiio_test` | Focused OpenImageIO WebP registration and fidelity test |
| `yaml_cpp_test` | `tests/yaml_cpp_test` | Stable yaml-cpp user-facing probe |
| `zlib_test` | `tests/zlib_test` | Minimal zlib packaging test |
| `dav1d_src` | `third_party/codecs/dav1d_src` | Pinned dav1d 1.5.4 scalar AV1 decoder backend for U++ |
| `jpegxl_src` | `third_party/codecs/jpegxl_src` | Pinned libjxl 0.12.0 source backend for U++ |
| `libde265_src` | `third_party/codecs/libde265_src` | Pinned libde265 1.1.1 scalar HEVC decoder backend for U++ |
| `libdeflate` | `third_party/codecs/libdeflate` | libdeflate package with stable public include path |
| `libdeflate_src` | `third_party/codecs/libdeflate_src` | Strict standalone libdeflate 1.25 source package for U++ |
| `libheif_src` | `third_party/codecs/libheif_src` | Pinned libheif 1.23.5 decode-only HEIF/AVIF container backend |
| `libjpeg_turbo` | `third_party/codecs/libjpeg_turbo` | Stable user-facing libjpeg-turbo package for U++ |
| `libjpeg_turbo_src` | `third_party/codecs/libjpeg_turbo_src` | Strict upstream libjpeg-turbo 3.2.0 source package for U++ |
| `libpng` | `third_party/codecs/libpng` | libpng package with stable public include path |
| `libpng_src` | `third_party/codecs/libpng_src` | Strict upstream libpng 1.6.58 source package for U++ |
| `libraw_src` | `third_party/codecs/libraw_src` | Pinned LibRaw 0.22.2 source backend for U++ |
| `libtiff` | `third_party/codecs/libtiff` | Stable user-facing libtiff package for U++ |
| `libtiff_src` | `third_party/codecs/libtiff_src` | Strict upstream libtiff 4.7.2 source package for U++ |
| `openjph` | `third_party/codecs/openjph` | OpenJPH package with stable public include path |
| `openjph_src` | `third_party/codecs/openjph_src` | Strict standalone OpenJPH 0.26.3 source package for U++ |
| `webp_src` | `third_party/codecs/webp_src` | Pinned libwebp 1.6.0 static source backend for U++ |
| `zlib` | `third_party/codecs/zlib` | zlib compression library packaged for U++ |
| `zlib_src` | `third_party/codecs/zlib_src` | Strict upstream zlib 1.3.2 source package for U++ |
| `FFmpeg` | `third_party/ffmpeg/FFmpeg` | Stable application-facing FFmpeg 9.0.1 native decode package |
| `ffmpeg_avcodec_src` | `third_party/ffmpeg/ffmpeg_avcodec_src` | Pinned FFmpeg 9.0.1 native H.264 libavcodec package |
| `ffmpeg_avformat_src` | `third_party/ffmpeg/ffmpeg_avformat_src` | Pinned FFmpeg 9.0.1 MOV/file libavformat package |
| `ffmpeg_avutil_src` | `third_party/ffmpeg/ffmpeg_avutil_src` | Pinned FFmpeg 9.0.1 libavutil scalar source package |
| `ffmpeg_headers` | `third_party/ffmpeg/ffmpeg_headers` | Strict FFmpeg 9.0.1 public-header and generated-config boundary |
| `ffmpeg_swscale_src` | `third_party/ffmpeg/ffmpeg_swscale_src` | Pinned FFmpeg 9.0.1 scalar libswscale package |
| `imath` | `third_party/imath/imath` | Imath package with stable public include path |
| `imath_src` | `third_party/imath/imath_src` | Strict upstream Imath 3.2.2 source package for U++ |
| `OpenColorIO` | `third_party/opencolorio/OpenColorIO` | Stable OpenColorIO user-facing package |
| `opencolorio_src` | `third_party/opencolorio/opencolorio_src` | OpenColorIO 2.5.2 core source package |
| `iex_src` | `third_party/openexr/iex_src` | Strict imported-source Iex package for U++ |
| `ilmthread_src` | `third_party/openexr/ilmthread_src` | Strict imported-source IlmThread package for U++ |
| `openexr` | `third_party/openexr/openexr` | User-facing OpenEXR package with stable include path |
| `openexr_core` | `third_party/openexr/openexr_core` | User-facing OpenEXRCore package with stable include path |
| `openexr_core_src` | `third_party/openexr/openexr_core_src` | Pinned OpenEXRCore source ownership for U++ |
| `openexr_src` | `third_party/openexr/openexr_src` | Strict upstream OpenEXR 3.4.13 high-level source package for U++ |
| `OpenImageIO` | `third_party/openimageio/OpenImageIO` | Stable application-facing OpenImageIO package |
| `openimageio_headers` | `third_party/openimageio/openimageio_headers` | Strict internal OpenImageIO 3.1.17.0 public-header package |
| `openimageio_plugin_dpxcineon` | `third_party/openimageio/openimageio_plugin_dpxcineon` | Pinned OpenImageIO 3.1.17 DPX and Cineon plugins |
| `openimageio_plugin_hdr` | `third_party/openimageio/openimageio_plugin_hdr` | OpenImageIO Radiance HDR/RGBE plugin registration package |
| `openimageio_plugin_heif` | `third_party/openimageio/openimageio_plugin_heif` | OpenImageIO HEIF/AVIF input plugin backed by decode-only libheif 1.23.5 |
| `openimageio_plugin_jpegxl` | `third_party/openimageio/openimageio_plugin_jpegxl` | OpenImageIO JPEG XL plugin registration package |
| `openimageio_plugin_openexr` | `third_party/openimageio/openimageio_plugin_openexr` | OpenImageIO OpenEXR plugin registration package |
| `openimageio_plugin_png` | `third_party/openimageio/openimageio_plugin_png` | OpenImageIO PNG plugin registration package |
| `openimageio_plugin_raw` | `third_party/openimageio/openimageio_plugin_raw` | OpenImageIO RAW input plugin backed by pinned LibRaw 0.22.2 |
| `openimageio_plugin_tiff` | `third_party/openimageio/openimageio_plugin_tiff` | OpenImageIO TIFF plugin backed by bundled libtiff 4.7.2 |
| `openimageio_plugin_webp` | `third_party/openimageio/openimageio_plugin_webp` | OpenImageIO WebP plugin backed by pinned libwebp 1.6.0 |
| `openimageio_plugins_src` | `third_party/openimageio/openimageio_plugins_src` | Pinned OpenImageIO 3.1.17.0 plugin source tree |
| `openimageio_src` | `third_party/openimageio/openimageio_src` | Strict upstream OpenImageIO 3.1.17.0 main source package |
| `openimageio_util_src` | `third_party/openimageio/openimageio_util_src` | Strict upstream OpenImageIO 3.1.17.0 utility source package |
| `expat` | `third_party/support/expat` | Stable Expat user-facing package |
| `expat_src` | `third_party/support/expat_src` | Strict upstream Expat 2.8.5 source package |
| `fmt` | `third_party/support/fmt` | fmt package with stable public include path |
| `fmt_src` | `third_party/support/fmt_src` | Strict upstream fmt 12.2.0 source package |
| `minizip_ng` | `third_party/support/minizip_ng` | Stable minizip-ng user-facing package |
| `minizip_ng_src` | `third_party/support/minizip_ng_src` | Strict upstream minizip-ng 4.0.10 source package |
| `pystring` | `third_party/support/pystring` | Stable pystring user-facing package |
| `pystring_src` | `third_party/support/pystring_src` | Strict upstream pystring 1.1.4 source package |
| `robinmap` | `third_party/support/robinmap` | robin-map package with stable public include path |
| `robinmap_src` | `third_party/support/robinmap_src` | Strict upstream robin-map 1.4.1 source package |
| `yaml_cpp` | `third_party/support/yaml_cpp` | Stable yaml-cpp user-facing package |
| `yaml_cpp_src` | `third_party/support/yaml_cpp_src` | Strict upstream yaml-cpp 0.8.0 source package |
| `imaging_workbench_bench` | `tools/imaging_workbench_bench` | ImagingWorkbench timing and coordinate bench |
| `libpng_src_roundtrip_test` | `tests/libpng_src_roundtrip_test` | Standalone no-Core PNG/zlib provider regression. |
