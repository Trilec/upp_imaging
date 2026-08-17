# Active Work

This file is the recovery authority for work currently in flight. After fetching `main`, read this file before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Accepted framework foundation: ImagingCore, ImagingIO EXR/PNG baseline, ImagingColor, ImagingAnalysis, ImagingDiagnostics and the `Imaging` umbrella.
- Still-image line includes JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- JPEG XL backend is Windows-proven Debug/Release after `a66e1192`; shared static OpenImageIO dependency repair is `5ca436c3` and awaits accumulation validation.
- FFmpeg is a separate media subsystem, not part of ImagingIO or the Imaging umbrella.

**TASK**

- Still-image validator lane: validate repaired aggregate OpenImageIO + later still-image formats as one accumulation pass.
- FFmpeg validator lane: Windows Debug now proves the header boundary 7/0 and libavutil 13/0. The third acceptance stopped compiling `ffmpeg_avcodec_src` because the repository-owned generated configuration exposed a sparse `CONFIG_*` namespace. The generated helper/component closure is repaired and requires focused rerun from the two green regression gates.

**TOUCHED**

Latest FFmpeg corrective checkpoint:

- `ffmpeg_headers/generated/config.h`
- `ffmpeg_headers/generated/config_components.h`
- this recovery file

The completed direct-package slice remains `FFmpeg/*`, `ffmpeg_first_frame_test/*` and `docs/FFMPEG_PLAN.md`.

Source manifests, FFmpeg upstream source, registries, fixture bytes and focused tests are unchanged by the latest repair.

**STATUS**

- FFmpeg remains pinned to signed `n9.0.1`, exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- Generated-equivalent Windows x86_64 / U++ CLANGx64 configuration remains static, LGPL-only and scalar: no threads/network/external codecs/filters/devices/audio-resample/CLI/encoding, no external/inline assembly, no hardware acceleration.
- `ffmpeg_headers` is a true public-header/generated-config package: generated codec/parser/BSF/demuxer/muxer/protocol registry `.c` fragments remain checked in but are not standalone package translation units.
- The generated host config records the complete 29-function FFmpeg `MATH_FUNCS` capability set provided by the Windows UCRT; this repair is Windows-proven by `ffmpeg_avutil_test` Debug 13/0.
- `config.h` now explicitly represents FFmpeg 9.0.1's complete `CONFIG_EXTRA` helper namespace. Only the already-selected H.264/MOV dependency closure is 1; non-selected helpers, including `CONFIG_MPEGVIDEODEC`, are explicit 0.
- External/autodetected options referenced by the compiled first slice are explicit disabled values, including `CONFIG_LIBLCEVC_DEC` and `CONFIG_LCMS2`.
- `config_components.h` retains the selected H.264/MOV/file component/helper closure and explicitly disables the complete H.264 hardware-acceleration family: D3D11VA, D3D11VA2, D3D12VA, DXVA2, NVDEC, VAAPI, VDPAU, VideoToolbox and Vulkan.
- `ffmpeg_headers_test` expected result: 7/0; Windows Debug is proven 7/0.
- `ffmpeg_avutil_test` expected result: 13/0; Windows Debug is proven 13/0.
- `ffmpeg_avcodec_test` expected result: 12/0; public codec registry remains native H.264 decoder only, no encoder/parser/BSF.
- `ffmpeg_avformat_test` expected result: 14/0; public format closure remains MOV/MP4 demux + local `file` protocol, no muxers.
- `ffmpeg_swscale_test` expected result: 13/0; deterministic limited-range ITU-601 YUV420P -> RGBA8 conversion.
- Stable direct `FFmpeg` package links the four implementation libraries and forwards standard FFmpeg API types through `FFmpeg/FFmpeg.h`; it adds no Imaging/U++ wrapper policy.
- `ffmpeg_first_frame_test` expected result: 27/0. It embeds a 1,463-byte one-frame 16x16 Constrained Baseline H.264 MP4, verifies standard 64-bit FNV-1a fixture evidence `0x86d54178fbc2b70a`, exact decoded logical YUV420P FNV-1a `0x54009ba1a158e125`, MOV/H.264 decode, RGBA conversion and cleanup.
- No Windows avcodec/avformat/swscale link/runtime or end-to-end decode result is claimed yet.

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
- `c23a7d515a8a4696f7b7f6f7b1b61f5be89843ce` — harden first-frame send/receive lifecycle and cleanup.
- `452c66810603d5de8970938298d112bdfdf37359` — correct standard FNV-1a first-frame evidence.
- `5b195d270811d609b1a4e03394ff7cbe6365879e` — remove generated registry fragments from the header package compilation set.
- `fa5e72d7ff27180075c7fa71f5bf790cfd3faf81` — complete Windows UCRT/FFmpeg `MATH_FUNCS` host capabilities.
- `743f6f89f091c23015dd549c2e0cbbccfdc71ec8` — complete FFmpeg generated `CONFIG_EXTRA` helper and disabled external-option namespace.
- `da017b6fc8d5ce81d96e6b670c258f7e1497a0e9` — explicitly close the disabled H.264 hardware-acceleration component family.
- This recovery update follows those two functional config commits; fetch current `main` for the exact recovery checkpoint SHA.

**VALIDATION**

Completed Windows evidence:

- JPEG XL backend Debug 9/0 and Release 9/0; previous lcms2/gtest/skcms-baseline failures are closed.
- Prior direct OIIO JPEG XL build stopped on aggregate DPX/OpenEXR and TIFF header visibility; owning package repair is published at `5ca436c3` and remains unvalidated.
- FFmpeg first acceptance exposed standalone generated registry fragments in `ffmpeg_headers`; repaired at `5b195d27`.
- FFmpeg second acceptance at `5b195d27`: `ffmpeg_headers_test` Debug built/ran 7/0, then libavutil failed on missing UCRT math capability macros.
- FFmpeg third acceptance at exact `705cd2cdd482b7b6da2d21dab0026f4d8e22d0ef`: clean checkout and pinned upstream; `ffmpeg_headers_test` Debug 7/0 and `ffmpeg_avutil_test` Debug 13/0 both passed. `ffmpeg_avcodec_test` then failed compiling current pinned sources on undefined `CONFIG_LIBLCEVC_DEC`, `CONFIG_MPEGVIDEODEC` and H.264 hardware component macros; matrix stopped correctly before avformat/swscale/first-frame, Release or repeatability.

Static/source review completed:

- Exact pinned `configure` at `bf1b838f` was inspected; its blob is identical to tag `n9.0` for the reviewed configuration logic.
- FFmpeg configure emits `CONFIG_LIST` plus a 119-entry `CONFIG_EXTRA` helper namespace to `config.h`, and emits component booleans to `config_components.h`; disabled items are still explicit Boolean macros.
- H.264 selects `cabac golomb h264chroma h264dsp h264parse h264pred h264qpel h264_sei videodsp`; recursive selection adds `startcode`, `itut_t35`, `atsc_a53` and `dovi_rpudec`. `mpegvideodec` is not selected and is therefore 0.
- MOV selects `iso_media riffdec`; `iso_media` selects `mpeg4audio`; IAMF and zlib are suggestions only and remain disabled.
- Exact `h264dec.c`/`h264_slice.c` inspection found the complete nine-member H.264 hardware macro family, including `CONFIG_H264_D3D11VA2_HWACCEL`, which was not present in the validator's truncated first-error block. All nine are explicitly 0.
- Exact `decode.c` inspection confirms runtime use of `CONFIG_LIBLCEVC_DEC`; it is explicit 0. `CONFIG_LCMS2` is also explicit 0 to keep the external-library namespace coherent.
- Latest repair changes generated Boolean configuration only. It does not alter source ownership, source manifests, APIs, registries, enabled components, fixture evidence, threading, assembly/SIMD, hardware acceleration, networking or external dependencies.

Not yet Windows-verified:

- OpenImageIO accumulation pass after `5ca436c3`.
- Corrected `ffmpeg_avcodec_test` build/run after the generated namespace repair.
- `ffmpeg_avformat_test`, `ffmpeg_swscale_test` and `ffmpeg_first_frame_test` in Debug.
- All six FFmpeg tests in Release and first-frame repeatability/clean shutdown.

**NEXT ACTION**

Still-image validator lane (independent):

1. Fetch/fast-forward current `origin/main`; require clean status.
2. Run `jpegxl_oiio_test` Debug/Release. If green, run the focused RAW/WebP/HEIF/TIFF direct/framework accumulation matrix. Fail fast on first current-main defect.

FFmpeg validator lane:

1. Fetch/fast-forward current `origin/main`; require clean status and initialize `ffmpeg_headers/upstream` at the pinned commit if needed.
2. Confirm the current generated-config corrective checkpoint is an ancestor of HEAD.
3. Re-run `ffmpeg_headers_test` Debug (7/0) and `ffmpeg_avutil_test` Debug (13/0) as regression gates, then build/run `ffmpeg_avcodec_test` Debug (12/0). The prior undefined `CONFIG_*` compile block must be gone.
4. If green, continue Debug: `ffmpeg_avformat_test` 14/0, `ffmpeg_swscale_test` 13/0, `ffmpeg_first_frame_test` 27/0.
5. Only after all Debug tests pass, run all six in Release with the same expected totals, then repeat `ffmpeg_first_frame_test` five times Debug and five times Release.
6. Gary may make only a genuinely tiny, unquestionably mechanical local correction during validation (for example a one-line typo or one missing explicit disabled `CONFIG_* 0` directly analogous to this closure). He must show the exact diff and rerun the focused gate. He must not commit or push. If the fix touches source/manifests/dependencies/APIs/feature selection, enables anything, needs more than a tiny local edit, spans multiple files, or requires design judgment, stop and report it to Curt instead.

Implementation lane after Windows evidence:

1. If another substantive current-main compile/link/runtime defect is reported, repair the owning package/config/source closure here, publish, then issue a focused validator rerun.
2. If all first-slice acceptance passes, close the initial FFmpeg milestone before considering SIMD/hardware acceleration, broader containers/codecs, seeking/index behavior, audio, or a backend-neutral U++ media wrapper.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
