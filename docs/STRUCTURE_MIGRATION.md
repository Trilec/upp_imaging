# Repository structure migration audit

Starting commit: `db87e3787e1d01cb75c982ae7ee0c315161b431f`. Remote main fetched and matched before work.

## Scope and recoverability

The requested brief explicitly authorizes deleting obsolete packages, duplicate tests, compatibility forwarders, probes and superseded applications. The following is the exact reviewed scope. Removal uses `git rm` without force, limited to tracked files; it cannot remove local modifications or untracked data. All listed tracked files are recoverable from the starting commit. No upstream source or submodule is in the deletion set.

## Test decisions

| Package | Decision | Current purpose / replacement |
| --- | --- | --- |
| `dav1d_prereq_test` | REMOVE | Internal decoder allocation/version/malformed-header probe; HEIF registration and malformed AVIF remain in heif_oiio_test and heif_imagingio_test. |
| `dpx_cineon_oiio_test` | KEEP | Focused OpenImageIO DPX and Cineon contract test |
| `expat_src_test` | REMOVE | Duplicate implementation-package form of retained expat_test; keep the public API regression. |
| `expat_test` | KEEP | Stable Expat user-facing probe |
| `ffmpeg_avcodec_test` | KEEP | Current public API regression |
| `ffmpeg_avformat_test` | KEEP | Current public API regression |
| `ffmpeg_avutil_test` | KEEP | Current public API regression |
| `ffmpeg_first_frame_test` | KEEP | Current public API regression |
| `ffmpeg_headers_test` | KEEP | Current public API regression |
| `ffmpeg_swscale_test` | KEEP | Current public API regression |
| `fmt_src_test` | REMOVE | One formatting expression source-link probe; fmt is exercised by the retained OpenImageIO integration. |
| `fmt_test` | REMOVE | One formatting expression public-link probe; fmt is exercised by the retained OpenImageIO integration. |
| `hdr_dpx_imagingio_test` | KEEP | Focused ImagingIO HDR, DPX and Cineon contract test |
| `hdr_oiio_test` | KEEP | Focused OpenImageIO Radiance HDR/RGBE registration test |
| `heif_imagingio_test` | KEEP | Focused ImagingIO HEIF/AVIF input-only policy test |
| `heif_oiio_test` | KEEP | Focused OpenImageIO HEIF/AVIF decode-only registration test |
| `iex_src_test` | REMOVE | Exception throw/catch source-link probe; retained openexr_test exercises OpenEXR and its exception dependency. |
| `ilmthread_src_test` | REMOVE | Semaphore post/wait source-link probe; retained OpenEXR integration links the current implementation. |
| `imaging_analysis_test` | KEEP | ImagingAnalysis public contract test |
| `imaging_color_ocio_test` | KEEP | Independent OpenColorIO acceptance test for ImagingColor |
| `imaging_color_test` | KEEP | ImagingColor public contract test |
| `imaging_core_test` | KEEP | ImagingCore contract test |
| `imaging_diagnostics_test` | KEEP | ImagingDiagnostics public contract test |
| `imaging_histogram_test` | KEEP | Tests for source-referred histogram analysis |
| `imaging_io_oiio_test` | KEEP | Independent OpenImageIO cross-check for ImagingIO |
| `imaging_io_test` | KEEP | ImagingIO public EXR and PNG contract test |
| `imaging_preview_coalescing_test` | KEEP | Deterministic tests for preview render coalescing |
| `imaging_roundtrip_viewer_ocio_smoke_test` | KEEP | Workbench preview/geometry regression |
| `imaging_test` | KEEP | Imaging umbrella contract test |
| `imaging_tone_conversion_test` | KEEP | Deterministic tone conversion test |
| `imaging_view_transform_test` | KEEP | Deterministic source/view transform test |
| `imaging_workbench_bench` | KEEP | Manual timing/coordinate utility under tools; not a deterministic acceptance gate. |
| `imaging_workbench_ocio_test` | KEEP | Workbench preview/geometry regression |
| `imath_src_test` | REMOVE | Duplicate implementation-package form of retained imath_test; keep the public API regression. |
| `imath_test` | KEEP | User-facing Imath package test |
| `jpeg_io_test` | REMOVE | Tests only the retired JPEG helper. Direct JPEG codec roundtrip remains in libjpeg_turbo_test; JPEG is not claimed as an ImagingIO format. |
| `jpegxl_imagingio_test` | KEEP | Focused ImagingIO JPEG XL contract test |
| `jpegxl_oiio_test` | KEEP | Focused OpenImageIO JPEG XL registration and roundtrip test |
| `jpegxl_prereq_test` | REMOVE | Internal source version and RGB roundtrip probe; exact RGB/RGBA roundtrips and malformed rejection remain in jpegxl_oiio_test and jpegxl_imagingio_test. |
| `libde265_prereq_test` | REMOVE | Internal version/allocation/empty-decoder probe; retained HEIF integration establishes decoder registration and lifetime. |
| `libdeflate_src_test` | REMOVE | Duplicate implementation-package form of retained libdeflate_test; keep the public API regression. |
| `libdeflate_test` | KEEP | User-facing libdeflate package test |
| `libheif_prereq_test` | REMOVE | Internal registry/allocation/version probe; retained HEIF integration covers initialization, decode-only registration, malformed input and shutdown. |
| `libjpeg_turbo_gui_link_test` | REMOVE | GUI subsystem link-only recovery probe; ImagingWorkbench remains a GUI link acceptance target. |
| `libjpeg_turbo_src_test` | REMOVE | Duplicate implementation-package form of retained libjpeg_turbo_test; keep the public API regression. |
| `libjpeg_turbo_test` | KEEP | User-facing libjpeg-turbo probe |
| `libpng_roundtrip_test` | KEEP | User-facing libpng memory round-trip test |
| `libpng_src_roundtrip_test` | KEEP | Distinct no-Core PNG/zlib provider branch on Windows; public libpng instead reuses U++ plugin/z. |
| `libpng_src_test` | REMOVE | Version/header source probe; retained libpng_roundtrip_test performs actual encode/decode. |
| `libpng_test` | REMOVE | Version/header public probe; retained libpng_roundtrip_test performs actual encode/decode. |
| `libraw_prereq_test` | REMOVE | Internal version/camera-list/context probe; retained raw_oiio_test and raw_imagingio_test establish the supported RAW boundary. |
| `libtiff_gui_link_test` | REMOVE | GUI subsystem link-only recovery probe; ImagingWorkbench remains a GUI link acceptance target. |
| `libtiff_src_test` | REMOVE | Duplicate implementation-package form of retained libtiff_test; keep the public API regression. |
| `libtiff_test` | KEEP | User-facing libtiff probe |
| `minizip_ng_src_test` | REMOVE | Duplicate implementation-package form of retained minizip_ng_test; keep the public API regression. |
| `minizip_ng_test` | KEEP | Stable minizip-ng user-facing probe |
| `ocio_dependencies_gui_link_test` | REMOVE | Dependency coexistence GUI link probe; retained ImagingWorkbench and imaging_workbench_ocio_test exercise the same dependency stack. |
| `ocio_dependencies_test` | REMOVE | Dependency coexistence and basic parse/compress probes; retained expat, yaml_cpp, pystring, minizip_ng and opencolorio tests cover current public behavior. |
| `oiio_compat_test` | REMOVE | Tests only the removed oiio forwarding alias; openimageio_io_test covers the canonical OpenImageIO API. |
| `opencolorio_gui_link_test` | REMOVE | GUI link and processor probe; retained workbench OCIO integration tests cover actual preview processing. |
| `opencolorio_src_test` | REMOVE | Duplicate implementation-package form of retained opencolorio_test; keep the public API regression. |
| `opencolorio_test` | KEEP | Stable OpenColorIO user-facing probe |
| `openexr_core_rgba_zip_test` | KEEP | OpenEXRCore RGBA ZIP round-trip |
| `openexr_core_roundtrip_test` | REMOVE | Single red-channel uncompressed roundtrip; retained openexr_core_rgba_zip_test exercises full RGBA channel layout and ZIP roundtrip. |
| `openexr_core_write_probe` | REMOVE | One-off EXRCore write diagnostic, superseded by retained RGBA ZIP roundtrip. |
| `openexr_io_test` | REMOVE | Tests only the retired narrow EXR helper; openexr_test, openexr_core_rgba_zip_test and ImagingIO preserve direct/typed EXR capability. |
| `openexr_src_test` | REMOVE | Duplicate implementation-package form of retained openexr_test; keep the public API regression. |
| `openexr_test` | KEEP | Stable OpenEXR 3.4.13 high-level file round-trip test |
| `openimageio_io_test` | KEEP | Deterministic OpenImageIO IO test |
| `openimageio_prereq_test` | REMOVE | Combined source/header/prerequisite coexistence probe superseded by openimageio_io_test and full format integrations. |
| `openjph_src_test` | REMOVE | Version, point/size and HT encoder/decoder link probe; no image roundtrip; source owner remains built through OpenEXR. |
| `openjph_test` | REMOVE | Duplicate public HT link probe; source owner remains built through OpenEXR. |
| `plugin_exr_test` | KEEP | plugin/exr StreamRaster preview contract |
| `png_io_test` | REMOVE | Tests only the retired narrow PNG helper; libpng_roundtrip_test and imaging_io_test preserve PNG encode/decode. |
| `pystring_src_test` | REMOVE | Duplicate implementation-package form of retained pystring_test; keep the public API regression. |
| `pystring_test` | KEEP | Stable pystring user-facing probe |
| `raw_imagingio_test` | KEEP | Focused ImagingIO camera RAW policy contract test |
| `raw_oiio_test` | KEEP | Focused OpenImageIO RAW registration contract test |
| `robinmap_src_test` | REMOVE | Three-entry map source-link probe; retained OpenImageIO exercises its map dependency. |
| `robinmap_test` | REMOVE | Three-entry map public-link probe; retained OpenImageIO exercises its map dependency. |
| `tiff_imagingio_test` | KEEP | Focused ImagingIO TIFF policy and transaction test |
| `tiff_io_test` | REMOVE | Tests only the retired narrow TIFF helper and its deliberately restricted layout policies; direct libtiff and full-fidelity TIFF integration remain. |
| `tiff_oiio_test` | KEEP | Focused OpenImageIO TIFF registration and fidelity test |
| `upp_png_plugin_test` | REMOVE | Comparison experiment against external U++ plugin/png; that external package is not owned by this repository. |
| `webp_imagingio_test` | KEEP | Focused ImagingIO WebP policy and transaction test |
| `webp_oiio_test` | KEEP | Focused OpenImageIO WebP registration and fidelity test |
| `webp_prereq_test` | REMOVE | Internal codec version and lossless RGB/RGBA roundtrip probe; retained webp_oiio_test and webp_imagingio_test cover exact pixels, alpha, malformed data and transactions. |
| `yaml_cpp_src_test` | REMOVE | Duplicate implementation-package form of retained yaml_cpp_test; keep the public API regression. |
| `yaml_cpp_test` | KEEP | Stable yaml-cpp user-facing probe |
| `zlib_src_test` | REMOVE | Duplicate implementation-package form of retained zlib_test; keep the public API regression. |
| `zlib_test` | KEEP | Minimal zlib packaging test |

## Production removals

| Package | Reason |
| --- | --- |
| `imaging_roundtrip_viewer` | Earlier narrow-helper diagnostic app, superseded by ImagingWorkbench. Its OCIO library is retained because the current workbench still consumes it. |
| `jpeg_io` | Only production consumer is retired imaging_roundtrip_viewer; JPEG encoding options remain available through public libjpeg_turbo. |
| `oiio` | Compatibility-only forwarding package; no retained production consumer. |
| `openexr_io` | Only production consumer is retired imaging_roundtrip_viewer; EXR capability remains through ImagingIO and public OpenEXR. |
| `png_io` | Only production consumer is retired imaging_roundtrip_viewer; PNG capability remains through ImagingIO and public libpng. |
| `tiff_io` | Only production consumer is retired imaging_roundtrip_viewer; TIFF capability remains through ImagingIO and public libtiff. |
| lowercase OpenColorIO manifests | Identical case aliases of the canonical OpenColorIO manifest; remove index entries only and retain canonical file. |

No test bodies are merged: duplicate implementations are removed in favor of existing public regressions. Package/source ownership boundaries remain.

## Exact tracked removal inventory

- `dav1d_prereq_test/`: 3 tracked files, all present in starting commit.
- `expat_src_test/`: 3 tracked files, all present in starting commit.
- `fmt_src_test/`: 3 tracked files, all present in starting commit.
- `fmt_test/`: 3 tracked files, all present in starting commit.
- `iex_src_test/`: 3 tracked files, all present in starting commit.
- `ilmthread_src_test/`: 3 tracked files, all present in starting commit.
- `imaging_roundtrip_viewer/`: 5 tracked files, all present in starting commit.
- `imath_src_test/`: 3 tracked files, all present in starting commit.
- `jpeg_io/`: 4 tracked files, all present in starting commit.
- `jpeg_io_test/`: 3 tracked files, all present in starting commit.
- `jpegxl_prereq_test/`: 3 tracked files, all present in starting commit.
- `libde265_prereq_test/`: 3 tracked files, all present in starting commit.
- `libdeflate_src_test/`: 3 tracked files, all present in starting commit.
- `libheif_prereq_test/`: 3 tracked files, all present in starting commit.
- `libjpeg_turbo_gui_link_test/`: 3 tracked files, all present in starting commit.
- `libjpeg_turbo_src_test/`: 3 tracked files, all present in starting commit.
- `libpng_src_test/`: 2 tracked files, all present in starting commit.
- `libpng_test/`: 2 tracked files, all present in starting commit.
- `libraw_prereq_test/`: 3 tracked files, all present in starting commit.
- `libtiff_gui_link_test/`: 2 tracked files, all present in starting commit.
- `libtiff_src_test/`: 2 tracked files, all present in starting commit.
- `minizip_ng_src_test/`: 3 tracked files, all present in starting commit.
- `ocio_dependencies_gui_link_test/`: 3 tracked files, all present in starting commit.
- `ocio_dependencies_test/`: 3 tracked files, all present in starting commit.
- `oiio/`: 3 tracked files, all present in starting commit.
- `oiio_compat_test/`: 3 tracked files, all present in starting commit.
- `opencolorio_gui_link_test/`: 2 tracked files, all present in starting commit.
- `opencolorio_src_test/`: 2 tracked files, all present in starting commit.
- `openexr_core_roundtrip_test/`: 3 tracked files, all present in starting commit.
- `openexr_core_write_probe/`: 3 tracked files, all present in starting commit.
- `openexr_io/`: 4 tracked files, all present in starting commit.
- `openexr_io_test/`: 2 tracked files, all present in starting commit.
- `openexr_src_test/`: 3 tracked files, all present in starting commit.
- `openimageio_prereq_test/`: 3 tracked files, all present in starting commit.
- `openjph_src_test/`: 3 tracked files, all present in starting commit.
- `openjph_test/`: 3 tracked files, all present in starting commit.
- `png_io/`: 4 tracked files, all present in starting commit.
- `png_io_test/`: 3 tracked files, all present in starting commit.
- `pystring_src_test/`: 3 tracked files, all present in starting commit.
- `robinmap_src_test/`: 3 tracked files, all present in starting commit.
- `robinmap_test/`: 3 tracked files, all present in starting commit.
- `tiff_io/`: 4 tracked files, all present in starting commit.
- `tiff_io_test/`: 2 tracked files, all present in starting commit.
- `upp_png_plugin_test/`: 3 tracked files, all present in starting commit.
- `webp_prereq_test/`: 3 tracked files, all present in starting commit.
- `yaml_cpp_src_test/`: 3 tracked files, all present in starting commit.
- `zlib_src_test/`: 3 tracked files, all present in starting commit.

## Final structure and required repairs

- Source directories: 177 at the starting commit, 8 after migration. Ignored build output is excluded; legacy local logs were moved under out.
- Packages: 174 before, 124 retained, 50 removed, 0 newly merged packages.
- Test/probe/benchmark packages: 91 before; 49 deterministic tests and 1 manual benchmark retained; 41 removed. The standalone PNG/zlib provider test is retained because it uses a different Windows link route from the Core-based public package.
- Framework: 6 packages under imaging. Third-party: 60 packages in seven dependency-family nests. Integrations: 7 packages, including the unchanged plugin/exr package identity. Apps: ImagingWorkbench. Tools: the manual benchmark and validation/generation scripts. Examples: framework usage documentation, requiring no nest. Docs: current architecture plus guides and design assets.
- The generated designer prototype executable was removed. Editable JSON and HTML design assets remain under docs/designs.
- Root ImathBox.h, ImathVec.h and half.h duplicates were removed. The Imath and OpenEXR public packages export their own header directories.
- Two lowercase/case-duplicate OpenColorIO manifests and the oiio alias were removed. Workbench includes now use the canonical OpenImageIO header.
- The old roundtrip viewer and its four narrow helpers were retired; native APIs and ImagingIO preserve current image capabilities. The current OCIO preview library remains because ImagingWorkbench consumes it.

Two UiTree calls were updated to the current Model() accessor. The workbench test and manual benchmark now use GUI_APP_MAIN, and the benchmark declares its histogram dependency. The OCIO preview test uses CONSOLE_APP_MAIN. These changes repair build and harness defects exposed by the retained targets.

The retained LUT regression exposed an OCIO 2.5.2 registry that allocated raw format objects without an owner. A generated local FileTransform.cpp overlay owns the registry and formats; tools/generate_ocio_file_transform.py reproduces and verifies it. All 39 preview checks must pass with a clean exit.

The workbench's expected-error cases also exposed MinGW main-thread OIIO error storage surviving until U++'s heap audit. Three local translation-unit wrappers release the input/output tables and global error string from a registered shutdown callback. Pending global errors retain upstream reporting behavior. The callback does not access the OIIO pool, whose own destructor controls worker lifetime. Normal image/error behavior and upstream pins are unchanged; the separate upstream per-object error growth issue remains documented.

No leak checks are disabled, and no temporary diagnostic instrumentation is retained. All 2,593 tracked vendored upstream files were compared with the starting commit and are unchanged; all eight relocated submodules have identical commits and clean worktrees. Generated downstream overlays are separate from those pinned trees.

Stages 1–7 built and ran Core, Imaging umbrella, OpenImageIO, OCIO, OpenEXR and FFmpeg representatives. The final retained acceptance set is tests/acceptance.txt; current results are in WINDOWS_ACCEPTANCE.md. The package dependency audit covers every production package through that suite, the application/benchmark builds, and a temporary compile/link check of the OpenJPH public header.
