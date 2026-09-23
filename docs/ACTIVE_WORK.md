# Active Work

Recovery authority for the current published state. `docs/WINDOWS_ACCEPTANCE.md` remains the self-contained validator contract and gate-order authority.

## BASE

- Final acceptance started from `d4016ee168b38c66af5db4bdd5cc68f9ff541c0e` (`origin/main` at refresh).
- U++ installation: `E:/upp-18468`; builder: `E:/upp-18468/umk.exe`; method: `GitHubOut`, `CLANGx64`, Debug `DEBUG_FULL` Noblitz and Release.
- Installed U++ and pinned upstream sources were not modified.

## TASK

The bounded Windows acceptance generation is complete. Mandatory still-image, opt-in EXR raster, FFmpeg Debug/Release and FFmpeg repeatability lanes are green; only optional external-fixture interoperability evidence remains pending.

## TOUCHED

- `OpenImageIO/OIIO.cpp`, `openimageio_plugin_heif/RegisterHEIF.*` - pair the statically linked OIIO HEIF initialization with process-exit `heif_deinit()` so libde265 state is released before U++ HEAPDBG.
- backend integration tests - keep deliberate OIIO error paths on joined workers so upstream thread-local diagnostics are destroyed before main-thread HEAPDBG teardown.
- `plugin/exr/exr.cpp`, `plugin_exr_test/main.cpp` - select the OpenEXR reader directly, drain expected open errors and isolate malformed-input diagnostics on a joined worker.
- `ffmpeg_headers/generated/config.h`, `config_components.h`, `ffmpeg_headers_test/main.cpp` - make the bounded generated-equivalent configuration explicit and audit the imported source closure.
- `ffmpeg_swscale_src/chroma_pos_compat.c`, `import.ext` - materialize the pinned stable-graph helper hidden by `CONFIG_UNSTABLE=0`, without enabling unstable backends.
- closure documentation - distinguish implemented, Windows-proven and deferred scope using the completed evidence.

## STATUS

- Root shutdown defect closed: libheif's built-in libde265 registration allocated a 60-byte `pps_scan_cache` through `pps_scan_cache_init()` -> `de265_init()` -> `libde265_init_plugin()` without matching teardown before U++ leak diagnostics. Repository-owned HEIF integration now calls `heif_deinit()` at process exit, reaching `de265_free()` before `MemoryDumpLeaks()`.
- Temporary VppLog prewarm and all source-level diagnostic experiments are removed. `OIIO_USTRING_CLEANUP` is not required.
- All 16 still-image gates passed in required order in Debug and Release with exact totals and exit `0`.
- `plugin_exr_test` passed `22/0` in Debug and Release; three additional focused Debug runs also exited `0` after its repair.
- FFmpeg remains pinned to signed `n9.0.1`, commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`, with the scalar static LGPL feature policy unchanged.
- All six FFmpeg gates passed in required order in Debug and Release; five additional first-frame runs per configuration each passed `27/0`, exit `0`.
- Provenance-reviewed real-camera RAW/DNG, 8-bit and 10-bit AVIF/HEIC, and animated-WebP fixtures are not stored in the repository: fixture unavailable / evidence pending.

## PUBLISHED

- Final acceptance/documentation closure: this checkpoint.
- `ffb191d` - scalar swscale configuration and pinned stable-graph closure.
- `eb56539` - FFmpeg generated-configuration coverage closure.
- `06e195d` - EXR negative-input teardown and direct-reader repair.
- `93c4bc3` - ImagingIO accumulated-gate teardown repair.
- `626282d` - focused OIIO teardown and pinned-API repair.
- `5ef0f3e` - HEIF/libde265 lifecycle and focused-gate repair.

## VALIDATION

- Still image Debug and Release, exact order: `openimageio_io_test` 21/0; `imaging_io_test` 79/0; `jpegxl_prereq_test` 9/0; `jpegxl_oiio_test` 10/0; `jpegxl_imagingio_test` 50/0; `hdr_oiio_test` 12/0; `dpx_cineon_oiio_test` 19/0; `hdr_dpx_imagingio_test` 38/0; `raw_oiio_test` 9/0; `raw_imagingio_test` 10/0; `webp_oiio_test` 13/0; `webp_imagingio_test` 21/0; `heif_oiio_test` 11/0; `heif_imagingio_test` 10/0; `tiff_oiio_test` 13/0; `tiff_imagingio_test` 29/0. Every build/link/run completed normally.
- Root blocker regression: five consecutive `imaging_io_test` Debug runs at 79/0, exit `0`, with `OIIO_USTRING_CLEANUP` unset and no HEAPDBG/access violation.
- EXR: Debug 22/0, exit `0`, plus three additional 22/0 Debug runs; Release 22/0, exit `0`.
- FFmpeg pin: exact tag `n9.0.1`, submodule commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- FFmpeg Debug and Release, exact order: headers 8/0; avutil 13/0; avcodec 12/0; avformat 14/0; swscale 13/0; first-frame 27/0. Every build/link/run completed normally.
- FFmpeg repeatability: five additional Debug and five additional Release first-frame runs, every run 27/0 and exit `0`.
- Hygiene: no tracked build artifacts/logs, no tracked output directory, no retired probe/lifecycle references, pinned submodules clean, `git diff --check` clean.

## NEXT ACTION

- No mandatory acceptance action remains for this bounded generation.
- Keep optional provenance-reviewed real-file interoperability evidence separate when suitable fixtures become available.
- Preserve deferred next scope: FFmpeg SIMD/assembly, hardware acceleration, broader codecs/containers, audio, seeking/indexing, backend-neutral media wrapper and waveform/vectorscope work.
