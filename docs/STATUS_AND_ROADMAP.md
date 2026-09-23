# Status And Roadmap

## Status labels

- **implemented** — source/package/test contract exists and has passed source/static review;
- **Windows-proven** — the relevant U++ CLANGx64 acceptance has been recorded;
- **platform validation pending** — implementation exists but the current accumulated Windows checkpoint is not yet green.

For the exact accepted checkpoint and evidence, read `docs/ACTIVE_WORK.md` after fetching current `main`.

## Current framework status

- `ImagingCore`: implemented and Windows-proven, 48/0.
- `ImagingIO`: implemented and Windows-proven; the 79/0 shared boundary and complete current format accumulation passed in Debug and Release.
- `ImagingColor`: implemented and Windows-proven, 66/0 plus independent OCIO 15/0.
- `ImagingAnalysis`: implemented and Windows-proven, 41/0.
- `ImagingDiagnostics`: implemented and Windows-proven, 33/0.
- `Imaging` umbrella: implemented and Windows-proven, 6/0.
- `plugin/exr`: implemented and Windows-proven at 22/0 in Debug and Release.
- FFmpeg first slice: implemented and Windows-proven; all six gates passed in Debug and Release plus five additional 27/0 first-frame runs per configuration.
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

- JPEG XL prerequisite/backend is Windows-proven 9/0 Debug and 9/0 Release.
- Shared static OpenImageIO plugin dependency repair is `5ca436c3ba6265f6431deaf7348332940051686d`.
- The complete current-main OpenImageIO + ImagingIO deterministic accumulation matrix is Windows-proven in Debug and Release.

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

The current focused test is Windows-proven at 22/0 in Debug and Release.

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

Source-ownership and generated-configuration failures exposed during acceptance are repaired in published checkpoints:

- `libavformat/to_upper4.c` materializes `ff_toupper4`;
- `libavformat/mpegaudiotabs.c` materializes `ff_mpa_freq_tab` and related tables;
- no new codec/muxer/protocol/feature was enabled;
- the swscale package explicitly disables synthesized CPU capability names and materializes the pinned stable-graph `ff_sws_chroma_pos()` helper without enabling unstable backends.

Current-main Windows evidence is complete: headers 8/0, avutil 13/0, avcodec 12/0, avformat 14/0, swscale 13/0 and first-frame 27/0 all passed in Debug and Release; five additional first-frame runs per configuration also passed 27/0 with exit 0.

## Current closure milestone

The current bounded generation is complete. The 16-target still-image matrix, `plugin_exr_test`, both six-gate FFmpeg lanes and all ten additional first-frame runs are Windows-proven with exact totals and clean exits. Supplementary real-file interoperability evidence remains separate and optional when provenance-reviewed fixtures become available.

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
