# Active Work

Recovery authority for work currently in flight. After fetching `main`, read this file before using chat history or starting validation.

## BASE

- Acceptance-definition work in this checkpoint is based on `d4f033956bccbb51a5ca41ab6e9f616b554c7b3a` on `main`.
- Current-generation framework implementation is complete: `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics`, `Imaging`.
- Established Windows framework baseline: ImagingCore 48/0, ImagingIO 79/0 baseline, ImagingColor 66/0 + independent OCIO 15/0, ImagingAnalysis 41/0, ImagingDiagnostics 33/0, Imaging umbrella 6/0.
- Code-side still-image line: JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF, TIFF.
- JPEG XL prerequisite/backend is Windows-proven 9/0 Debug and 9/0 Release after `a66e1192025032823e93a890e16cc3874034a8a4`.
- Shared static OpenImageIO dependency repair is `5ca436c3ba6265f6431deaf7348332940051686d`; complete current-main accumulation remains pending.
- `plugin/exr` focused preview contract is implemented through `323c3dc29938de404fc3411b87dcaf6c6aea4559`; expanded contract is 22 checks and remains Windows Debug/Release pending.
- FFmpeg is a separate media subsystem, not part of ImagingIO or the Imaging umbrella.

## TASK

Final current-generation closure:

1. complete the exact repository-owned still-image/OpenImageIO/ImagingIO Debug and Release accumulation matrix;
2. complete `plugin_exr_test` 22/0 Debug/Release;
3. complete accumulated Windows acceptance for the FFmpeg first slice, including repeatability;
4. keep real-file RAW/HEIF/WebP interoperability evidence separate from deterministic repository-owned gates;
5. repair only substantive current-main failures as coherent root-cause slices;
6. mark the bounded generation complete only when the required platform gates are green and any unavailable supplementary fixture evidence is reported explicitly.

Documentation reconciliation is complete: README, architecture, package catalogue, status/roadmap, package layout and changelog distinguish **implemented**, **Windows-proven**, and **platform validation pending**. This checkpoint additionally replaces later-format roadmap shorthand with exact repository test targets and totals.

## TOUCHED / RECENT CLOSURE

Still-image acceptance definition:

- `ImagingIO/ImagingIO.upp`
- `ImagingIO/README.md`
- `openimageio_io_test/README.md`
- `jpegxl_prereq_test/README.md`
- `jpegxl_oiio_test/README.md`
- `jpegxl_imagingio_test/README.md`
- `hdr_oiio_test/README.md`
- `dpx_cineon_oiio_test/README.md`
- `docs/PACKAGE_CATALOGUE.md`
- `docs/STATUS_AND_ROADMAP.md`
- `docs/package_layout.md`
- `CHANGELOG.md`
- `docs/ACTIVE_WORK.md`

FFmpeg config/source ownership:

- `ffmpeg_headers/generated/config.h`
- `ffmpeg_headers/generated/config_components.h`
- `ffmpeg_headers_test/main.cpp`
- `ffmpeg_headers_test/README.md`
- `ffmpeg_avformat_src/import.ext`
- `ffmpeg_avformat_src/README.md`
- `docs/FFMPEG_PLAN.md`

EXR focused contract:

- `plugin_exr_test/main.cpp`

## STATUS

### Still-image / OpenImageIO / ImagingIO

- `ImagingIO/FormatPolicy.cpp`, `OpenImageIO/OpenImageIO.upp` and `OpenImageIO/OIIO.cpp` were source-reviewed against the bounded format line; HEIF/AVIF and TIFF are already implemented/registered and no architecture or dependency repair was identified in this documentation checkpoint.
- `ImagingIO` package metadata and README now describe the already-implemented HEIF/AVIF and TIFF policies instead of stopping at WebP.
- Shared static OpenImageIO dependency closure remains `5ca436c3ba6265f6431deaf7348332940051686d`.
- The exact repository-owned deterministic accumulation matrix is:
  - `openimageio_io_test` — 21/0;
  - `imaging_io_test` — 79/0;
  - `jpegxl_prereq_test` — 9/0;
  - `jpegxl_oiio_test` — 10/0;
  - `jpegxl_imagingio_test` — 50/0;
  - `hdr_oiio_test` — 12/0;
  - `dpx_cineon_oiio_test` — 19/0;
  - `hdr_dpx_imagingio_test` — 38/0;
  - `raw_oiio_test` — 9/0;
  - `raw_imagingio_test` — 10/0;
  - `webp_oiio_test` — 13/0;
  - `webp_imagingio_test` — 21/0;
  - `heif_oiio_test` — 11/0;
  - `heif_imagingio_test` — 10/0;
  - `tiff_oiio_test` — 13/0;
  - `tiff_imagingio_test` — 29/0.
- Positive real-camera RAW decode, real 8/10-bit AVIF/HEIC decode and animated-WebP multi-frame rejection intentionally require provenance-reviewed external fixtures. Those fixtures are not stored in the repository. Their evidence is supplementary and must remain separately **reported** or **unknown/pending**; it must not be fabricated or silently promoted from deterministic malformed/routing tests.

### `plugin/exr`

- `plugin_exr_test` is a real 22-check contract covering RGBA clamp/straight alpha, Gray, GrayAlpha, non-finite-to-zero, one-channel mask, named MultiChannel RGB selection, raster registration, invalid/truncated input and cleanup.
- Debug/Release Windows acceptance remains pending.

### FFmpeg first slice

- Exact signed upstream pin: FFmpeg `n9.0.1`, commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- Static LGPL scalar Windows x86_64/U++ CLANGx64 policy remains: no threads, network, external codecs, filters, devices, audio resampling, CLI/encoding, external/inline assembly or hardware acceleration.
- `ffmpeg_headers` owns repository-generated equivalent configuration/public headers and compiles no implementation source.
- Generated registries remain checked in but are not standalone header-package translation units.
- Windows UCRT `MATH_FUNCS` repair is proven by avutil Debug 13/0.
- `config.h` represents the reviewed 119-entry `CONFIG_EXTRA` helper namespace; selected H.264/MOV helpers are enabled and non-selected helpers are explicit 0.
- Relevant disabled external/autodetected options include `CONFIG_LIBLCEVC_DEC` and `CONFIG_LCMS2`.
- All nine reviewed H.264 hardware-acceleration component variants are explicit 0.
- `HAVE_DIRECT_H 1` from `d692702a41ea1e0f639f5fe55666f1dc133b7875` is retained.
- `ffmpeg_headers_test` includes the source/config parity gate over the four production manifests and recursively reached headers. Expected current result: 8/0.
- `ffmpeg_avutil_test`: 13/0.
- `ffmpeg_avcodec_test`: 12/0; native H.264 decoder only.
- `ffmpeg_avformat_test`: 14/0; MOV/MP4 demux + local `file` protocol only.
- Gary's earlier avformat linker failure on `ff_toupper4` and `ff_mpa_freq_tab` was traced to the exact pinned upstream materializers `libavformat/to_upper4.c` and `libavformat/mpegaudiotabs.c`; both are now explicit in `ffmpeg_avformat_src/import.ext`.
- Avformat manifest remains explicit/no-glob at 45 translation units. No codec/muxer/protocol/feature was added by the repair.
- Parallel pinned swscale Makefile-to-manifest audit found no analogous source-ownership gap; SIMD/unstable assembly remains excluded.
- `ffmpeg_swscale_test`: 13/0.
- `ffmpeg_first_frame_test`: 27/0. Embedded fixture is 1,463 bytes, one-frame 16x16 Constrained Baseline H.264 MP4; fixture FNV-1a `0x86d54178fbc2b70a`, logical decoded YUV420P FNV-1a `0x54009ba1a158e125`.

## PUBLISHED

Recent still-image/plugin:

- `a66e1192025032823e93a890e16cc3874034a8a4` — JPEG XL skcms linker repair.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.
- `323c3dc29938de404fc3411b87dcaf6c6aea4559` — expanded `plugin/exr` preview contract.
- Still-image acceptance-definition checkpoint — this file is part of that single multi-file commit; fetch current `main` and record its exact SHA before validation.

Recent FFmpeg:

- `d90f96a3a3046176257d5216d3023f377663c40b` — generated-config namespace repair checkpoint.
- `d692702a41ea1e0f639f5fe55666f1dc133b7875` — retain Windows `HAVE_DIRECT_H 1`.
- `74247ac00dc4da919a89b5f69995b790ba2606f0` — generated source/config parity audit.
- `9cf5d82806cfd0a13777e5361bdf7b3daf2c4b08` — parity-gate plan documentation.
- `bc5cea2befabe614320979ffb408a35e0187f8ea` — FFmpeg config parity recovery checkpoint.
- `fed4e6800bf2f1aee432f708633d3b6ed15c54e7` — avformat materializer ownership repair.
- `375dbcf56eda47ee3f50954e78e2dbdf525a1ed4` — avformat source ownership documentation.
- `d4a6051e909fbe95c8930ad725e2b857e4efc3af` — avformat link-ownership recovery checkpoint.

Repository documentation:

- `a05f300a5514b1bf250a902e080bdae40615a1e7` — reconcile top-level repository status.
- `5f64a1cea60fbdec4cf7e65a81e5dc3be397fa0b` — reconcile architecture boundaries.
- `787ee2aa11b2257bd172bb4b6cb4f2ca7f033e3b` — reconcile catalogue, status/roadmap, package layout, changelog and recovery state.
- `d4f033956bccbb51a5ca41ab6e9f616b554c7b3a` — align FFmpeg parity acceptance recovery.

## VALIDATION

### Windows-proven

- Framework baseline: 48/0, 79/0, 66/0 + OCIO 15/0, 41/0, 33/0, umbrella 6/0 as listed above.
- JPEG XL prerequisite/backend: 9/0 Debug and 9/0 Release.
- Earlier FFmpeg acceptance on task base `d90f96...` with the `HAVE_DIRECT_H` correction now published in ancestry:
  - precheck/submodule pin passed;
  - old headers contract 7/0 Debug passed before the parity gate existed;
  - avutil Debug 13/0 passed;
  - avcodec Debug 12/0 passed;
  - run reached avformat link and stopped only on `ff_toupper4` / `ff_mpa_freq_tab`.
- Gary reported `git diff --check` passed and made no commit/push in that validation run.

### Static/source review complete

- Current still-image package/test names and expected totals were checked against repository `.upp`, test source and README contracts rather than roadmap shorthand.
- `ImagingIO` current format policy and `OpenImageIO` static plugin registration/ownership were reviewed for JPEG XL, HDR/RGBE, DPX/Cineon, RAW, WebP, HEIF/AVIF and TIFF; no new code-side architecture gap was identified.
- Exact pinned FFmpeg configure dependency selections for H.264/MOV reviewed.
- Exact pinned avformat Makefile/materializer ownership reviewed.
- Exact pinned swscale Makefile/source ownership reviewed.
- Documentation state remains explicit about implemented versus Windows-proven versus pending platform evidence.

### Platform validation pending

- complete repository-owned still-image deterministic matrix above in Debug and Release on current `main`;
- `plugin_exr_test` 22/0 Debug/Release;
- current `ffmpeg_headers_test` 8/0;
- current-main avutil/avcodec regression;
- repaired avformat 14/0;
- swscale 13/0;
- first-frame 27/0;
- all six FFmpeg gates in Release;
- first-frame five Debug + five Release repeats, every run 27/0 with clean shutdown/cleanup.

### Supplementary interoperability evidence

- real provenance-reviewed RAW/DNG positive decode: external fixture required; current evidence remains pending unless explicitly reported from such a fixture;
- real 8/10-bit AVIF and HEIC positive decode: external fixtures required; current evidence remains pending unless explicitly reported;
- animated-WebP rejection: real animated fixture required; current evidence remains pending unless explicitly reported.

These supplementary items must remain separate from deterministic repository-owned test results.

## NEXT ACTION

1. Fetch/fast-forward current `origin/main`; record exact HEAD; require clean status.
2. Run the complete repository-owned still-image matrix in Debug in the exact order listed above; stop on the first substantive failure.
3. Only after the entire still-image Debug lane is green, run the same matrix in Release with the same expected totals.
4. Run `plugin_exr_test` Debug and Release; require 22/0 each.
5. Initialize/update `ffmpeg_headers/upstream`; require exact `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
6. Run FFmpeg Debug in order: headers 8/0, avutil 13/0, avcodec 12/0, avformat 14/0, swscale 13/0, first-frame 27/0.
7. Only after the entire FFmpeg Debug lane is green, run all six in Release with the same totals.
8. Run `ffmpeg_first_frame_test` five times Debug and five times Release; every run must remain 27/0 with clean cleanup.
9. If provenance-reviewed RAW/AVIF/HEIC/animated-WebP fixtures are available, record those supplementary results separately. If they are not available, report that evidence as unavailable/pending; do not fabricate fixtures and do not fold the absence into a deterministic package-test PASS.
10. Fail fast on the first substantive current-main defect. Gary may only make a genuinely tiny, unquestionably mechanical one-file correction and must show the exact diff; source manifests, dependencies, APIs, tests, component/feature enablement or multi-file work returns to the supervisor.
11. Group substantive defects by root cause and repair them as coherent source/config/dependency slices. Do not return to one-line ping-pong.
12. When all required bounded platform gates are green, update this file/status docs and declare the current-generation milestone complete while retaining any unavailable supplementary interoperability evidence as a separate explicit boundary.

## WORKING RHYTHM

- Remote GitHub `main` is authoritative; refresh before every edit/publish.
- Implement larger coherent slices; do not make each operation/compiler error a milestone.
- Publish coherent checkpoints often enough that outages cannot strand the state.
- Update this file at each meaningful checkpoint with BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION.
- Review complete touched files, callers/tests/dependencies/package manifests and the full diff before publish.
- Reserve broad regression matrices for accumulation points.
- On recovery: fetch `main`, read this file first, inspect referenced current files/commits, then continue from NEXT ACTION.
