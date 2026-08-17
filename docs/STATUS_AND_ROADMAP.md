# Status And Roadmap

## Status labels

- **implemented** — source/package/test contract exists and has passed source/static review;
- **Windows-proven** — the relevant U++ CLANGx64 acceptance has been recorded;
- **platform validation pending** — implementation exists but the current accumulated Windows checkpoint is not yet green.

For the exact in-flight boundary, read `docs/ACTIVE_WORK.md` after fetching current `main`.

## Current framework status

- `ImagingCore`: implemented and Windows-proven, 48/0.
- `ImagingIO`: implemented; EXR/PNG baseline Windows-proven at 79/0; exact current-main format accumulation pending.
- `ImagingColor`: implemented and Windows-proven, 66/0 plus independent OCIO 15/0.
- `ImagingAnalysis`: implemented and Windows-proven, 41/0.
- `ImagingDiagnostics`: implemented and Windows-proven, 33/0.
- `Imaging` umbrella: implemented and Windows-proven, 6/0.
- `plugin/exr`: implemented; expanded 22-check focused Windows Debug/Release acceptance pending.
- FFmpeg first slice: implementation/source ownership closed; current-main Debug/Release and repeatability acceptance pending.
- LumaPix: paused/reference only; `upp_imaging` does not depend on it.

## Still-image format line

The code-side format expansion is implemented for:

- OpenEXR and PNG baseline;
- JPEG XL;
- Radiance HDR/RGBE;
- DPX and input-only Cineon according to the documented subset;
- camera RAW input;
- exact-lossless WebP according to the documented subset;
- decode-only HEIF/AVIF;
- TIFF/OpenImageIO expansion.

Important acceptance state:

- JPEG XL prerequisite/backend is Windows-proven 9/0 Debug and 9/0 Release after `a66e1192025032823e93a890e16cc3874034a8a4`.
- Shared static OpenImageIO plugin dependency repair is `5ca436c3ba6265f6431deaf7348332940051686d`.
- The complete current-main OpenImageIO + ImagingIO deterministic accumulation matrix is still platform-validation pending.
- Do not describe the later format line as Windows-accepted until that accumulation pass is green.

Exact repository-owned Debug/Release targets:

1. `openimageio_io_test` — 21/0;
2. `imaging_io_test` — 79/0;
3. `jpegxl_prereq_test` — 9/0;
4. `jpegxl_oiio_test` — 10/0;
5. `jpegxl_imagingio_test` — 50/0;
6. `hdr_oiio_test` — 12/0;
7. `dpx_cineon_oiio_test` — 19/0;
8. `hdr_dpx_imagingio_test` — 38/0;
9. `raw_oiio_test` — 9/0;
10. `raw_imagingio_test` — 10/0;
11. `webp_oiio_test` — 13/0;
12. `webp_imagingio_test` — 21/0;
13. `heif_oiio_test` — 11/0;
14. `heif_imagingio_test` — 10/0;
15. `tiff_oiio_test` — 13/0;
16. `tiff_imagingio_test` — 29/0.

Positive real-camera RAW decode, real 8/10-bit AVIF/HEIC decode and animated-WebP rejection require provenance-reviewed external fixtures that are not stored in the repository. Keep those as separately reported supplementary interoperability evidence; never infer them from the deterministic matrix or fabricate substitute fixtures.

## ImagingIO policy

The framework remains fail-closed outside each documented format subset and preserves a shared transactional load/save contract.

Established baseline includes:

- single-image, non-deep 2D handling according to the documented format policy;
- typed `ImageData`/`ImageSpec` contracts;
- stable structured diagnostics;
- same-directory transactional saves;
- completed save candidates reopened and verified before destination replacement;
- backend-managed metadata omission policy rather than accidental leakage of OIIO implementation state.

Format-specific constraints remain in `ImagingIO/FormatPolicy.*` and `ImagingIO/README.md`; this roadmap must not broaden those claims.

## `plugin/exr`

The opt-in EXR `StreamRaster` bridge is implemented for display-oriented preview, not full-fidelity EXR interchange.

Current focused contract covers:

- encoded EXR from supplied U++ `Stream`;
- ordinary single-image/single-mip/non-deep 2D preview;
- RGB/RGBA;
- Gray/GrayAlpha;
- one-channel masks;
- named MultiChannel images with an unambiguous RGB triplet;
- straight RGBA8 preview output;
- finite values clamped to `[0,1]` and rounded to 8-bit;
- non-finite preview samples mapped deterministically to zero;
- truthful opaque/alpha reporting;
- invalid/truncated input rejection and fixture cleanup.

The current focused test is 22 checks and awaits Windows Debug/Release acceptance.

## FFmpeg first slice

FFmpeg remains a separate media subsystem. It is not part of `ImagingIO` or the `Imaging` umbrella.

Pinned upstream:

- signed release `n9.0.1`;
- exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.

Implemented first slice:

- `ffmpeg_headers` generated/public configuration boundary;
- scalar `ffmpeg_avutil_src`;
- native H.264-only `ffmpeg_avcodec_src`;
- MOV/MP4 + local-file-only `ffmpeg_avformat_src`;
- scalar `ffmpeg_swscale_src`;
- stable direct `FFmpeg` package;
- deterministic embedded one-frame H.264/MP4 decode-to-RGBA test.

Deliberately disabled for this slice:

- threads;
- network protocols;
- external codecs;
- filters/devices;
- audio resampling;
- CLI/encoding;
- external/inline assembly;
- hardware acceleration.

Current expected acceptance gates:

1. `ffmpeg_headers_test` — 8/0, including generated-config parity audit;
2. `ffmpeg_avutil_test` — 13/0;
3. `ffmpeg_avcodec_test` — 12/0;
4. `ffmpeg_avformat_test` — 14/0;
5. `ffmpeg_swscale_test` — 13/0;
6. `ffmpeg_first_frame_test` — 27/0.

Recorded Windows evidence from the earlier acceptance run:

- precheck/submodule pin passed;
- old `ffmpeg_headers_test` contract passed 7/0 before the new parity check was added;
- `ffmpeg_avutil_test` Debug passed 13/0;
- `ffmpeg_avcodec_test` Debug passed 12/0;
- the generated `CONFIG_*` codec errors are closed for that source/config state;
- the run then reached `ffmpeg_avformat_test` link and failed only on `ff_toupper4` and `ff_mpa_freq_tab`.

That avformat failure is repaired in published source ownership:

- `libavformat/to_upper4.c` materializes `ff_toupper4`;
- `libavformat/mpegaudiotabs.c` materializes `ff_mpa_freq_tab` and related tables;
- no new codec/muxer/protocol/feature was enabled;
- the parallel swscale Makefile-to-manifest audit found no analogous ownership gap.

Current-main avformat/swscale/first-frame Debug, all Release gates and first-frame repeatability remain platform-validation pending.

## Current closure milestone

The current phase is **final acceptance**, not feature discovery.

Closure requires:

1. the exact 16-target repository-owned still-image matrix above in Debug and Release;
2. `plugin_exr_test` 22/0 in Debug and Release;
3. the complete six-gate FFmpeg Debug matrix;
4. the same six FFmpeg gates in Release;
5. five Debug and five Release repetitions of `ffmpeg_first_frame_test`, every run 27/0 with clean shutdown/cleanup;
6. any substantive current-main failures repaired as coherent root-cause source/config/dependency slices, followed by focused regression and accumulation rerun;
7. supplementary real-file interoperability evidence recorded separately when provenance-reviewed fixtures are available;
8. repository docs/recovery state updated to mark the bounded generation complete only after required platform evidence is green.

## Dependency direction

- `ImagingCore` depends on U++ Core only.
- `ImagingIO` depends on `ImagingCore` and OpenImageIO privately; OIIO types remain out of public headers.
- `ImagingColor` depends on `ImagingCore` and OpenColorIO privately; OCIO types remain out of public headers.
- `ImagingAnalysis` depends on `ImagingCore` only.
- `ImagingDiagnostics` depends on `ImagingCore` only and remains GUI-independent.
- `Imaging` depends on all five framework packages.
- format/backend source packages do not depend upward on the framework.
- `plugin/exr` remains opt-in and is not automatically included by `Imaging`.
- FFmpeg remains a parallel direct media stack and is not automatically included by `Imaging`.

## Deferred next scope

The following are deliberately **not** blockers to the current 100% closure milestone:

- FFmpeg SIMD/external assembly;
- hardware acceleration;
- broader containers/codecs;
- audio;
- seeking/index behaviour;
- backend-neutral media wrapper;
- waveform/vectorscope expansion.

Do not revive these while current-generation acceptance defects remain.
