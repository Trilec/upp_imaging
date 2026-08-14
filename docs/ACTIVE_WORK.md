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
- Implementation lane: complete bounded FFmpeg MOV/H.264-to-RGBA8 first-frame decode.

**TOUCHED**

Latest FFmpeg checkpoint:

- `ffmpeg_headers/generated/{config.h,config_components.h}`
- `ffmpeg_headers/generated/libavformat/{demuxer_list,muxer_list,protocol_list}.c`
- `ffmpeg_headers/ffmpeg_headers.upp`
- `ffmpeg_avcodec_src/{import.ext,README.md}`
- `ffmpeg_avformat_src/*`
- `ffmpeg_avformat_test/*`

**STATUS**

- FFmpeg remains pinned to signed `n9.0.1`, exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- Generated-equivalent Windows x86_64 / U++ CLANGx64 configuration is static, LGPL-only, scalar, no threads/network/external codecs/filters/devices/audio-resample/CLI/encoding.
- `ffmpeg_avutil_src` is published; focused test expects 13/0.
- `ffmpeg_avcodec_src` is published and now contains 71 explicit TUs: the previous native H.264 closure plus MPEG-4 audio helpers selected by MOV's `ISO_MEDIA` dependency. Public registry remains exactly H.264 decoder only; no encoder/parser/BSF.
- `config_components.h` now carries the selected H.264/MOV helper closure required by sources that include it directly, including explicit disabled H.264 peer components where expressions require zero-valued identifiers.
- `ffmpeg_avformat_src` is published with 43 explicit TUs: 31 base objects, Windows MSVCRT file-open helper, ISO-media + RIFF decode, seven MOV objects, local-file protocol, and MOV's private AC-3 channel-layout helper.
- Configure-equivalent format registries expose exactly one demuxer (`ff_mov_demuxer`), no muxers and one protocol (`ff_file_protocol`). Optional IAMF/zlib suggestions remain disabled.
- `ffmpeg_avformat_test` expects 14/0 and covers version/license/configuration, exact demuxer/muxer/protocol registration, local AVIO byte roundtrip, malformed-MOV refusal and cleanup.
- No Windows FFmpeg result is claimed yet. Next boundary is scalar `libswscale`, then deterministic first-frame MOV/H.264 decode evidence.

**PUBLISHED**

- `a66e1192025032823e93a890e16cc3874034a8a4` — JPEG XL skcms linker repair.
- `c37521e050cdb1c04583c0a5bdb06763742b1669` — TIFF expansion.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.
- `901332ce46387e0d09026cfd5f26d4528b8cd9d1` — FFmpeg 9.0.1 header/pin boundary.
- `21a72f6dfccfdc216b4f3cb180cb3ae9415c1c11` — FFmpeg libavutil scalar foundation.
- `69ea15f478cb1c0d25a0461afbaf1e16c23b22f9` — native H.264 libavcodec boundary.
- `0291801b82dc25a78b36c884373a5ee76c92b687` — `Add MOV and local-file FFmpeg format boundary`.
- This file is the recovery-log follow-up; fetch `main` for exact docs SHA.

**VALIDATION**

Completed Windows evidence:

- JPEG XL backend Debug 9/0 and Release 9/0; old lcms2/gtest/skcms-baseline failures closed.
- prior direct OIIO build stopped on shared DPX/OpenEXR and TIFF header visibility; repair at `5ca436c3` remains unvalidated.

Static/source review completed:

- OIIO direct dependencies repaired at owning manifests;
- exact FFmpeg libavutil/libavcodec/libavformat source ownership reconstructed from pinned Makefiles/configure selections;
- H.264 recursion followed through ITU-T T.35, ATSC A/53 and Dolby Vision RPU;
- MOV recursion followed through ISO-media -> MPEG4AUDIO and RIFFDEC; IAMF/zlib remain suggestions only;
- MOV private AC-3 channel-layout symbol handled explicitly for static linkage;
- generated codec/parser/BSF/demuxer/muxer/protocol lists are checked in instead of relying on absent configure output;
- network-disabled base format source avoids Winsock paths; Windows MSVCRT/file-open path is explicit.

Not yet Windows-verified:

- OpenImageIO accumulation pass after `5ca436c3`;
- FFmpeg headers 7/0;
- FFmpeg avutil 13/0;
- FFmpeg avcodec 12/0;
- FFmpeg avformat 14/0;
- libswscale/full decode.

**NEXT ACTION**

Validator lane:

1. Fetch current `origin/main`; confirm `5ca436c3` ancestor and clean status.
2. Run `jpegxl_oiio_test` Debug/Release, then focused RAW/WebP/HEIF/TIFF direct/framework tests if green.
3. Fail fast on first current-main defect; report evidence without patching.
4. Do not start FFmpeg validation yet.

Implementation lane:

1. Reconstruct exact pinned scalar libswscale source closure and generated configuration implications.
2. Add deterministic YUV420P -> RGBA8 conversion coverage with exact/near-exact expected evidence.
3. Add direct native package boundary over the four FFmpeg libraries.
4. Add deterministic first-frame MOV/H.264 decode evidence; only after this slice is coherent hand FFmpeg to Gary.
5. Publish coherent checkpoints and update this file.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
