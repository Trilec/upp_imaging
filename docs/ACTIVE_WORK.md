# Active Work

This file is the recovery authority for work currently in flight. After fetching `main`, read this file before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Accepted framework foundation: ImagingCore, ImagingIO EXR/PNG baseline, ImagingColor, ImagingAnalysis, ImagingDiagnostics and the `Imaging` umbrella.
- Still-image line includes JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- JPEG XL backend is Windows-proven Debug/Release after `a66e1192`; shared static OpenImageIO dependency repair is `5ca436c3` and awaits accumulation validation.
- `plugin/exr` focused preview contract has been expanded through current `main` to cover GrayAlpha, non-finite preview samples, single-channel masks and alpha/opaque reporting; Windows closure is still pending.
- FFmpeg is a separate media subsystem, not part of ImagingIO or the Imaging umbrella.

**TASK**

- Still-image validator lane: validate repaired aggregate OpenImageIO + later still-image formats as one accumulation pass.
- FFmpeg closure lane: complete first-slice Windows acceptance after generated-config and source/link ownership repairs, using one accumulated Debug/Release matrix rather than one-operation milestones.
- Documentation closure lane: README/architecture/catalogue/status/changelog lag the implemented framework and format roadmap and must be reconciled after the current code checkpoint is stable.

**TOUCHED**

Latest FFmpeg link-ownership closure:

- `ffmpeg_avformat_src/import.ext`
- `ffmpeg_avformat_src/README.md`
- this recovery file

Recent FFmpeg config/preflight closure remains:

- `ffmpeg_headers_test/main.cpp`
- `docs/FFMPEG_PLAN.md`
- `ffmpeg_headers/generated/config.h`
- `ffmpeg_headers/generated/config_components.h`

The latest avformat change adds only the two pinned upstream materializer translation units required by the already-selected MOV/ISO-media source graph. Public APIs, generated registries, component enablement, fixture bytes, upstream pin and feature policy are unchanged.

**STATUS**

- FFmpeg remains pinned to signed `n9.0.1`, exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- Generated-equivalent Windows x86_64 / U++ CLANGx64 configuration remains static, LGPL-only and scalar: no threads/network/external codecs/filters/devices/audio-resample/CLI/encoding, no external/inline assembly, no hardware acceleration.
- `ffmpeg_headers` is a true public-header/generated-config package: generated codec/parser/BSF/demuxer/muxer/protocol registry `.c` fragments remain checked in but are not standalone package translation units.
- The generated host config records the complete 29-function FFmpeg `MATH_FUNCS` capability set required against the Windows UCRT; this repair is Windows-proven by `ffmpeg_avutil_test` Debug 13/0.
- `config.h` explicitly represents FFmpeg 9.0.1's complete 119-entry `CONFIG_EXTRA` helper namespace. Only the selected H.264/MOV dependency closure is 1; non-selected helpers, including `CONFIG_MPEGVIDEODEC`, are explicit 0.
- External/autodetected options referenced by the compiled first slice are explicit disabled values, including `CONFIG_LIBLCEVC_DEC` and `CONFIG_LCMS2`.
- `config_components.h` retains the selected H.264/MOV/file component/helper closure and explicitly disables the complete nine-member H.264 hardware-acceleration family: D3D11VA, D3D11VA2, D3D12VA, DXVA2, NVDEC, VAAPI, VDPAU, VideoToolbox and Vulkan.
- Curt/Gary's small follow-up at `d692702a41ea1e0f639f5fe55666f1dc133b7875` adds `HAVE_DIRECT_H 1`; pinned `libavformat/os_support.h` uses `<direct.h>` under that capability. The value is retained in current `main`.
- `ffmpeg_headers_test` has an eighth preflight contract. It reads the four production source manifests, follows reachable pinned FFmpeg/generated includes, collects `CONFIG_*`, `HAVE_*`, `ARCH_*` and `AV_HAVE_*` identifiers, reports every missing generated macro together, and fails if the namespace is incomplete. Expected result is 8/0; the earlier Windows 7/0 result predates this stronger gate.
- `ffmpeg_avutil_test` expected result: 13/0.
- `ffmpeg_avcodec_test` expected result: 12/0; native H.264 decoder only, no encoder/parser/BSF in the public registry.
- `ffmpeg_avformat_test` expected result: 14/0; MOV/MP4 demux + local `file` protocol, no muxers.
- The pinned `libavformat/Makefile` source-ownership audit is now complete for the selected first slice. The 31 base objects, MSVCRT `file_open`, `ISO_MEDIA`, `RIFFDEC`, seven MOV objects and file protocol were already present. MOV's AC-3 channel-layout duplicate was already local to avformat; `log2_tab` is already materialized by avutil and `h2645_parse` by the selected H.264 avcodec closure. The missing live materializers were exactly `libavformat/to_upper4.c` (`ff_toupper4`) and `libavformat/mpegaudiotabs.c` (`ff_mpa_freq_tab` / MPEG-audio tables). Both are now in the explicit avformat manifest. This does not enable new codecs, muxers, protocols or features.
- The avformat manifest is now 45 translation units and remains explicit/no-glob.
- A parallel pinned `libswscale/Makefile` audit found no analogous source-ownership gap: all 22 base objects plus the three unconditional x86 C dispatch objects are present; shared `log2_tab` and `half2float` materializers are already supplied by avutil; unstable/SIMD assembly objects remain excluded.
- `ffmpeg_swscale_test` expected result: 13/0; deterministic limited-range ITU-601 YUV420P -> RGBA8 conversion.
- Stable direct `FFmpeg` package links the four implementation libraries and forwards standard FFmpeg API types through `FFmpeg/FFmpeg.h`; it adds no Imaging/U++ wrapper policy.
- `ffmpeg_first_frame_test` expected result: 27/0. It embeds a 1,463-byte one-frame 16x16 Constrained Baseline H.264 MP4, verifies standard 64-bit FNV-1a fixture evidence `0x86d54178fbc2b70a`, exact decoded logical YUV420P FNV-1a `0x54009ba1a158e125`, MOV/H.264 decode, RGBA conversion and cleanup.

**PUBLISHED**

Still-image repair line:

- `a66e1192025032823e93a890e16cc3874034a8a4` — JPEG XL skcms linker repair.
- `c37521e050cdb1c04583c0a5bdb06763742b1669` — TIFF expansion.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.
- `323c3dc29938de404fc3411b87dcaf6c6aea4559` — complete focused `plugin/exr` preview test contract.

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
- `d90f96a3a3046176257d5216d3023f377663c40b` — checkpoint generated-config namespace repair.
- `d692702a41ea1e0f639f5fe55666f1dc133b7875` — retain Windows `HAVE_DIRECT_H` capability discovered during continued local work.
- `74247ac00dc4da919a89b5f69995b790ba2606f0` — add deterministic source/config parity audit to the FFmpeg header gate.
- `9cf5d82806cfd0a13777e5361bdf7b3daf2c4b08` — document the parity-gate policy and new 8/0 header contract.
- `fed4e6800bf2f1aee432f708633d3b6ed15c54e7` — add the two missing avformat materializer translation units.
- `375dbcf56eda47ee3f50954e78e2dbdf525a1ed4` — document complete avformat source/link ownership.
- This recovery update follows those functional/documentation checkpoints; fetch current `main` for its exact SHA.

**VALIDATION**

Completed Windows evidence:

- Framework baseline previously accepted: ImagingCore 48/0, public ImagingIO 79/0, ImagingColor 66/0 with independent OCIO 15/0, ImagingAnalysis 41/0, ImagingDiagnostics 33/0 and Imaging umbrella 6/0.
- JPEG XL prerequisite/backend Debug 9/0 and Release 9/0 after `a66e1192`; previous skcms linker failure is closed.
- Prior direct OIIO/JPEG XL accumulation stopped on aggregate DPX/OpenEXR and TIFF header visibility; owning package repair is published at `5ca436c3` and remains unvalidated as an accumulation pass.
- FFmpeg acceptance on task base `d90f96a3a3046176257d5216d3023f377663c40b`, with the permitted local `HAVE_DIRECT_H 1` correction that is now already published in current ancestry: precheck/submodule pin passed; `ffmpeg_headers_test` Debug 7/0, `ffmpeg_avutil_test` Debug 13/0 and `ffmpeg_avcodec_test` Debug 12/0 all passed. The previous undefined `CONFIG_*` codec errors are therefore Windows-closed for that source/config state.
- The same run reached `ffmpeg_avformat_test` link and failed only on undefined `ff_toupper4` and `ff_mpa_freq_tab`; source-manifest editing was correctly stopped and returned to the supervisor. The pinned Makefile audit identified the missing owners as `libavformat/to_upper4.c` and `libavformat/mpegaudiotabs.c`; both are now published in the avformat manifest.
- `git diff --check` passed in Gary's local validation state; Gary made no commit/push.

Static/source review completed:

- Exact pinned `configure` at `bf1b838f` was inspected for the generated namespace and recursive H.264/MOV selection logic.
- FFmpeg configure emits `CONFIG_LIST` plus a 119-entry `CONFIG_EXTRA` helper namespace to `config.h`, and emits component booleans to `config_components.h`; disabled items are still explicit Boolean macros.
- H.264 selects `cabac golomb h264chroma h264dsp h264parse h264pred h264qpel h264_sei videodsp`; recursive selection adds `startcode`, `itut_t35`, `atsc_a53` and `dovi_rpudec`. `mpegvideodec` is not selected and is therefore 0.
- MOV selects `iso_media riffdec`; `iso_media` selects `mpeg4audio`; IAMF and zlib are suggestions only and remain disabled.
- Exact H.264 source inspection found the complete nine-member hardware macro family, including `CONFIG_H264_D3D11VA2_HWACCEL`; all nine are explicit 0.
- Exact `decode.c` inspection confirms runtime use of `CONFIG_LIBLCEVC_DEC`; it is explicit 0.
- Exact pinned `libavformat/Makefile` and materializer sources were inspected for the current link repair; no other live first-slice avformat materializer gap was found.
- Exact pinned `libswscale/Makefile` and x86 Makefile were inspected against the current explicit swscale manifest; no source ownership gap was found.

Not yet Windows-verified:

- OpenImageIO accumulation pass after `5ca436c3`.
- Focused `plugin/exr` expanded 22-check contract in Debug/Release.
- New `ffmpeg_headers_test` 8/0 parity gate.
- Current-main `ffmpeg_avutil_test` / `ffmpeg_avcodec_test` regression after the parity/config follow-ups.
- `ffmpeg_avformat_test` after the new materializer ownership closure.
- `ffmpeg_swscale_test` and `ffmpeg_first_frame_test` in Debug.
- All six FFmpeg tests in Release and first-frame repeatability/clean shutdown.

**NEXT ACTION**

FFmpeg validator lane:

1. Fetch/fast-forward current `origin/main`; record exact `HEAD`; require clean status and initialize `ffmpeg_headers/upstream` at exact `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
2. Build/run `ffmpeg_headers_test` Debug first. Expected **8/0**. If the parity check fails, stop and return the complete `MISSING GENERATED MACRO ...` list and `CONFIG AUDIT ...` summary; do not invent capability values.
3. If green, run `ffmpeg_avutil_test` Debug **13/0**, `ffmpeg_avcodec_test` Debug **12/0**, `ffmpeg_avformat_test` Debug **14/0**, `ffmpeg_swscale_test` Debug **13/0**, and `ffmpeg_first_frame_test` Debug **27/0**.
4. Only after the complete Debug lane is green, run all six in Release with the same totals.
5. Repeat `ffmpeg_first_frame_test` five times Debug and five times Release. Every run must remain 27/0 with clean shutdown/fixture cleanup.
6. Gary may make only a genuinely tiny, unquestionably mechanical local correction. He must show the original error and exact diff, rerun the focused gate, and must not broaden feature policy. Anything spanning source/manifests/dependencies/APIs or requiring configuration judgment stops and returns here.

Still-image validator lane (independent and can run as a separate bounded pass):

1. Fetch/fast-forward current `origin/main`; require clean status.
2. Run the repaired aggregate OpenImageIO/JPEG XL gate after `5ca436c3` in Debug and Release.
3. Run the focused HDR/DPX-Cineon/RAW/WebP/HEIF-AVIF/TIFF direct/framework accumulation matrix already present in the repository.
4. Run `plugin_exr_test` Debug and Release; current expected focused result is **22/0**.
5. Fail fast on the first current-main defect and report it without architectural edits.

Implementation/documentation lane after current validation results:

1. Group any substantive failures by root cause and repair them as coherent source/config/dependency slices rather than one-line ping-pong.
2. Reconcile README, architecture, package catalogue, status/roadmap and changelog with the implemented framework/format/FFmpeg state without claiming unproven Windows acceptance.
3. When FFmpeg and still-image accumulation pass, mark the bounded current-generation milestone complete before considering SIMD/hardware acceleration, broader media scope, waveform/vectorscope expansion or a backend-neutral media wrapper.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
