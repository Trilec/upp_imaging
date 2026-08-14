# Active Work

This file is the recovery authority for work currently in flight. After fetching `main`, read this file before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Accepted framework foundation: ImagingCore, ImagingIO EXR/PNG baseline, ImagingColor, ImagingAnalysis, ImagingDiagnostics and the `Imaging` umbrella.
- Still-image line includes JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- JPEG XL backend is Windows-proven Debug/Release after `a66e1192`; shared static OpenImageIO dependency repair is `5ca436c3` and awaits accumulation validation.
- FFmpeg is a separate media subsystem, not part of ImagingIO or the Imaging umbrella.

**TASK**

- Validator lane: validate repaired aggregate OpenImageIO + later still-image formats as one accumulation pass.
- FFmpeg implementation lane: first bounded MOV/H.264-to-RGBA8 decode slice is code-side complete; next gate is accumulated Windows acceptance.

**TOUCHED**

Latest FFmpeg completion checkpoint:

- `FFmpeg/*`
- `ffmpeg_first_frame_test/*`
- `docs/FFMPEG_PLAN.md`
- this recovery file

Earlier source checkpoints remain unchanged: `ffmpeg_headers`, `ffmpeg_avutil_src`, `ffmpeg_avcodec_src`, `ffmpeg_avformat_src`, `ffmpeg_swscale_src` and their focused tests.

**STATUS**

- FFmpeg remains pinned to signed `n9.0.1`, exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- Generated-equivalent Windows x86_64 / U++ CLANGx64 configuration is static, LGPL-only and scalar: no threads/network/external codecs/filters/devices/audio-resample/CLI/encoding, no external/inline assembly, no hardware acceleration.
- `ffmpeg_headers_test` expected result: 7/0.
- `ffmpeg_avutil_test` expected result: 13/0.
- `ffmpeg_avcodec_test` expected result: 12/0; public codec registry is native H.264 decoder only, no encoder/parser/BSF.
- `ffmpeg_avformat_test` expected result: 14/0; public format closure is MOV/MP4 demux + local `file` protocol, no muxers.
- `ffmpeg_swscale_test` expected result: 13/0; deterministic limited-range ITU-601 YUV420P -> RGBA8 conversion.
- Stable direct `FFmpeg` package now links the four implementation libraries and forwards standard FFmpeg API types through `FFmpeg/FFmpeg.h`; it adds no Imaging/U++ wrapper policy.
- `ffmpeg_first_frame_test` expected result: 27/0. It embeds a 1,463-byte one-frame 16x16 Constrained Baseline H.264 MP4, verifies fixture FNV-1a `0x6974a106bbf07694`, exact decoded logical YUV420P FNV-1a `0xc011b1b3a98f4583`, MOV/H.264 decode, RGBA conversion and cleanup.
- No Windows FFmpeg compile/link/runtime result is claimed yet.

**PUBLISHED**

Still-image repair line:

- `a66e1192025032823e93a890e16cc3874034a8a4` — JPEG XL skcms linker repair.
- `c37521e050cdb1c04583c0a5bdb06763742b1669` — TIFF expansion.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.

FFmpeg line:

- `901332ce46387e0d09026cfd5f26d4528b8cd9d1` — FFmpeg 9.0.1 header/pin boundary.
- `21a72f6dfccfdc216b4f3cb180cb3ae9415c1c11` — scalar libavutil foundation.
- `69ea15f478cb1c0d25a0461afbaf1e16c23b22f9` — native H.264 libavcodec boundary.
- `0291801b82dc25a78b36c884373a5ee76c92b687` — MOV/local-file libavformat boundary.
- `0505675b0e1529477d1048a796d435b9ad55694d` — scalar libswscale boundary.
- `5de2b47124e8028738fb7112036334345a958d4c` — direct `FFmpeg` package + deterministic first-frame decode slice.
- `3aeb661848a33a1b3a994a048f143e773c37a7d0` — reconcile FFmpeg implementation plan with completed first slice.
- This file is the recovery-log follow-up; fetch `main` for its exact docs commit SHA.

**VALIDATION**

Completed Windows evidence:

- JPEG XL backend Debug 9/0 and Release 9/0; previous lcms2/gtest/skcms-baseline failures are closed.
- Prior direct OIIO JPEG XL build stopped on aggregate DPX/OpenEXR and TIFF header visibility; owning package repair is published at `5ca436c3` and remains unvalidated.

Static/source review completed:

- OIIO direct dependencies were repaired at owning manifests, not tests.
- Exact FFmpeg libavutil/libavcodec/libavformat/libswscale source ownership was reconstructed from pinned Makefiles/configure selections; production manifests are explicit.
- H.264 recursion follows through CABAC/Golomb/chroma/DSP/parse/pred/qpel/SEI/VideoDSP, ITU-T T.35, ATSC A/53 and Dolby Vision RPU.
- MOV recursion follows through ISO-media -> MPEG4AUDIO and RIFFDEC; IAMF/zlib suggestions remain disabled.
- Generated codec/parser/BSF/demuxer/muxer/protocol registries are checked in instead of relying on absent configure output.
- x86 architecture identity remains truthful while assembly dispatch is disabled.
- The direct package depends only on FFmpeg implementation/header packages; no Imaging, CtrlLib, Workbench, external codec or networking dependency was introduced.
- End-to-end fixture is embedded in test source; no external runtime asset/download is required.

Not yet Windows-verified:

- OpenImageIO accumulation pass after `5ca436c3`.
- All six FFmpeg focused/end-to-end tests in Debug and Release, plus repeatability/clean shutdown.

**NEXT ACTION**

Validator lane:

1. Fetch/fast-forward current `origin/main`; require clean status.
2. Close the still-image repair first: run `jpegxl_oiio_test` Debug/Release. If green, run the focused RAW/WebP/HEIF/TIFF direct/framework accumulation matrix. Fail fast on first current-main defect and make no edits.
3. If the still-image lane is green, validate accumulated FFmpeg in order: `ffmpeg_headers_test`, `ffmpeg_avutil_test`, `ffmpeg_avcodec_test`, `ffmpeg_avformat_test`, `ffmpeg_swscale_test`, `ffmpeg_first_frame_test` in Debug and Release.
4. Repeat the end-to-end first-frame test enough times to expose lifecycle/cleanup instability; report new warnings, exit codes, summaries and final clean Git state.

Implementation lane after Windows evidence:

1. If a current-main compile/link/runtime defect is reported, repair the owning package/config/source closure here, publish, then issue a focused validator rerun.
2. If all first-slice acceptance passes, close the initial FFmpeg milestone before considering SIMD/hardware acceleration, broader containers/codecs, seeking/index behavior, audio, or a backend-neutral U++ media wrapper.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
