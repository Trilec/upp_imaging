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
- FFmpeg closure lane: stop discovering sparse generated configuration one translation unit at a time. The header/config boundary now audits the exact four production source manifests plus recursively reached pinned FFmpeg headers and reports every referenced generated configuration identifier that lacks a definition.
- Documentation closure lane: README/architecture/catalogue/status/changelog lag the implemented framework and format roadmap and must be reconciled after the current code checkpoint is stable.

**TOUCHED**

Latest FFmpeg closure checkpoint:

- `ffmpeg_headers_test/main.cpp`
- `docs/FFMPEG_PLAN.md`
- this recovery file

Recent generated configuration remains:

- `ffmpeg_headers/generated/config.h`
- `ffmpeg_headers/generated/config_components.h`

Source manifests, FFmpeg upstream source, registries, fixture bytes, public APIs and implementation source are unchanged by the parity-test checkpoint.

**STATUS**

- FFmpeg remains pinned to signed `n9.0.1`, exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- Generated-equivalent Windows x86_64 / U++ CLANGx64 configuration remains static, LGPL-only and scalar: no threads/network/external codecs/filters/devices/audio-resample/CLI/encoding, no external/inline assembly, no hardware acceleration.
- `ffmpeg_headers` is a true public-header/generated-config package: generated codec/parser/BSF/demuxer/muxer/protocol registry `.c` fragments remain checked in but are not standalone package translation units.
- The generated host config records the complete 29-function FFmpeg `MATH_FUNCS` capability set required against the Windows UCRT; this repair is Windows-proven by `ffmpeg_avutil_test` Debug 13/0.
- `config.h` explicitly represents FFmpeg 9.0.1's complete 119-entry `CONFIG_EXTRA` helper namespace. Only the selected H.264/MOV dependency closure is 1; non-selected helpers, including `CONFIG_MPEGVIDEODEC`, are explicit 0.
- External/autodetected options referenced by the compiled first slice are explicit disabled values, including `CONFIG_LIBLCEVC_DEC` and `CONFIG_LCMS2`.
- `config_components.h` retains the selected H.264/MOV/file component/helper closure and explicitly disables the complete nine-member H.264 hardware-acceleration family: D3D11VA, D3D11VA2, D3D12VA, DXVA2, NVDEC, VAAPI, VDPAU, VideoToolbox and Vulkan.
- Curt/Gary's small follow-up at `d692702a41ea1e0f639f5fe55666f1dc133b7875` adds `HAVE_DIRECT_H 1`; source review confirmed pinned `libavformat/os_support.h` uses `HAVE_DIRECT_H` on Windows. This one-line host-header capability is retained.
- `ffmpeg_headers_test` now has an eighth preflight contract. It reads `ffmpeg_avutil_src/import.ext`, `ffmpeg_avcodec_src/import.ext`, `ffmpeg_avformat_src/import.ext` and `ffmpeg_swscale_src/import.ext`, follows reachable pinned FFmpeg/generated includes, collects `CONFIG_*`, `HAVE_*`, `ARCH_*` and `AV_HAVE_*` identifiers, subtracts definitions owned by generated headers/source/build options, sorts and prints every missing generated macro, and fails as one aggregate gate if the configuration namespace is incomplete.
- The parity gate deliberately checks definition completeness only. It must not turn an unknown capability on merely to make the audit green.
- Current expected `ffmpeg_headers_test` result is **8/0**. The earlier Windows 7/0 result predates the new parity gate and remains evidence only for the original seven public-header checks.
- `ffmpeg_avutil_test` expected result: 13/0; Windows Debug is proven 13/0 on the pre-parity checkpoint.
- `ffmpeg_avcodec_test` expected result: 12/0; public codec registry remains native H.264 decoder only, no encoder/parser/BSF.
- `ffmpeg_avformat_test` expected result: 14/0; public format closure remains MOV/MP4 demux + local `file` protocol, no muxers.
- `ffmpeg_swscale_test` expected result: 13/0; deterministic limited-range ITU-601 YUV420P -> RGBA8 conversion.
- Stable direct `FFmpeg` package links the four implementation libraries and forwards standard FFmpeg API types through `FFmpeg/FFmpeg.h`; it adds no Imaging/U++ wrapper policy.
- `ffmpeg_first_frame_test` expected result: 27/0. It embeds a 1,463-byte one-frame 16x16 Constrained Baseline H.264 MP4, verifies standard 64-bit FNV-1a fixture evidence `0x86d54178fbc2b70a`, exact decoded logical YUV420P FNV-1a `0x54009ba1a158e125`, MOV/H.264 decode, RGBA conversion and cleanup.
- No Windows avcodec/avformat/swscale link/runtime or end-to-end decode result is claimed yet after the config-namespace repairs.

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
- `d90f96a3a3046176257d5216d3023f377663c40b` — checkpoint generated-config namespace repair.
- `d692702a41ea1e0f639f5fe55666f1dc133b7875` — retain Windows `HAVE_DIRECT_H` capability discovered during continued local work.
- `74247ac00dc4da919a89b5f69995b790ba2606f0` — add deterministic source/config parity audit to the FFmpeg header gate.
- `9cf5d82806cfd0a13777e5361bdf7b3daf2c4b08` — document the parity-gate policy and new 8/0 header contract.
- This recovery update follows those functional/documentation checkpoints; fetch current `main` for its exact SHA.

**VALIDATION**

Completed Windows evidence:

- Framework baseline previously accepted: ImagingCore 48/0, public ImagingIO 79/0, ImagingColor 66/0 with independent OCIO 15/0, ImagingAnalysis 41/0, ImagingDiagnostics 33/0 and Imaging umbrella 6/0.
- JPEG XL prerequisite/backend Debug 9/0 and Release 9/0 after `a66e1192`; previous skcms linker failure is closed.
- Prior direct OIIO/JPEG XL accumulation stopped on aggregate DPX/OpenEXR and TIFF header visibility; owning package repair is published at `5ca436c3` and remains unvalidated as an accumulation pass.
- FFmpeg first acceptance exposed standalone generated registry fragments in `ffmpeg_headers`; repaired at `5b195d27`.
- FFmpeg second acceptance at `5b195d27`: `ffmpeg_headers_test` Debug built/ran 7/0, then libavutil failed on missing UCRT math capability macros.
- FFmpeg third acceptance at exact `705cd2cdd482b7b6da2d21dab0026f4d8e22d0ef`: clean checkout and pinned upstream; `ffmpeg_headers_test` Debug 7/0 and `ffmpeg_avutil_test` Debug 13/0 both passed. `ffmpeg_avcodec_test` then failed compiling pinned sources on undefined `CONFIG_LIBLCEVC_DEC`, `CONFIG_MPEGVIDEODEC` and H.264 hardware component macros; matrix stopped correctly before avformat/swscale/first-frame, Release or repeatability.
- The new 8/0 config-parity header gate at/after `74247ac0` is **not yet Windows-validated**. Do not report 8/0 until Gary builds/runs it.

Static/source review completed:

- Exact pinned `configure` at `bf1b838f` was inspected for the generated namespace and recursive H.264/MOV selection logic.
- FFmpeg configure emits `CONFIG_LIST` plus a 119-entry `CONFIG_EXTRA` helper namespace to `config.h`, and emits component booleans to `config_components.h`; disabled items are still explicit Boolean macros.
- H.264 selects `cabac golomb h264chroma h264dsp h264parse h264pred h264qpel h264_sei videodsp`; recursive selection adds `startcode`, `itut_t35`, `atsc_a53` and `dovi_rpudec`. `mpegvideodec` is not selected and is therefore 0.
- MOV selects `iso_media riffdec`; `iso_media` selects `mpeg4audio`; IAMF and zlib are suggestions only and remain disabled.
- Exact H.264 source inspection found the complete nine-member hardware macro family, including `CONFIG_H264_D3D11VA2_HWACCEL`; all nine are explicit 0.
- Exact `decode.c` inspection confirms runtime use of `CONFIG_LIBLCEVC_DEC`; it is explicit 0.
- Exact pinned `libavformat/os_support.h` inspection confirms Windows code conditions `<direct.h>` on `HAVE_DIRECT_H`; current main explicitly defines it 1.
- The new parity scanner uses only U++ Core filesystem/string/container APIs verified in current Ultimate++ Core headers (`GetCurrentDirectory`, path helpers, `FileExists`, `LoadFile`, `StringStream`, `Index`). It changes test/preflight behavior only; production FFmpeg source, package manifests and enabled feature policy are unchanged.

Not yet Windows-verified:

- OpenImageIO accumulation pass after `5ca436c3`.
- New `ffmpeg_headers_test` 8/0 parity gate.
- Current `ffmpeg_avutil_test` regression after the parity/config follow-ups.
- Corrected `ffmpeg_avcodec_test` build/run after the generated namespace repair.
- `ffmpeg_avformat_test`, `ffmpeg_swscale_test` and `ffmpeg_first_frame_test` in Debug.
- All six FFmpeg tests in Release and first-frame repeatability/clean shutdown.
- `plugin/exr` focused Windows closure if it is included in the current completion milestone.

**NEXT ACTION**

FFmpeg implementation/validator lane:

1. Fetch/fast-forward current `origin/main`; require clean status and initialize `ffmpeg_headers/upstream` at exact `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
2. Build/run `ffmpeg_headers_test` Debug first. New expected result is 8/0.
3. If the config-parity check fails, do **not** continue to avutil/avcodec. Return the complete `MISSING GENERATED MACRO ...` list plus `CONFIG AUDIT sources=... scanned=... referenced=... generated=... missing=...`. The implementation lane will classify and close all missing definitions together without blindly enabling capabilities.
4. If the header parity gate is 8/0, build/run `ffmpeg_avutil_test` Debug 13/0, then `ffmpeg_avcodec_test` Debug 12/0, followed by `ffmpeg_avformat_test` 14/0, `ffmpeg_swscale_test` 13/0 and `ffmpeg_first_frame_test` 27/0.
5. Only after all Debug tests pass, run all six in Release with the same expected totals, then repeat `ffmpeg_first_frame_test` five times Debug and five times Release.
6. Gary may make only a genuinely tiny, unquestionably mechanical local correction. He must show the original error and exact diff, rerun the focused gate, and must not broaden feature policy. Anything spanning source/manifests/dependencies/APIs or requiring configuration judgment stops and returns here.

Still-image validator lane (independent and can run as a separate bounded pass):

1. Fetch/fast-forward current `origin/main`; require clean status.
2. Run the repaired aggregate OpenImageIO/JPEG XL gate after `5ca436c3` in Debug and Release.
3. If green, run the focused HDR/DPX-Cineon/RAW/WebP/HEIF-AVIF/TIFF direct/framework accumulation matrix already present in the repository. Fail fast on the first current-main defect and report it without architectural edits.

Implementation/documentation lane after the first current validation result:

1. If the parity gate reports missing definitions, classify each against pinned configure/Windows policy and publish one coherent generated-config closure, then rerun 8/0 before implementation packages.
2. Reconcile README, architecture, package catalogue, status/roadmap and changelog with the implemented framework/format/FFmpeg state without claiming unproven Windows acceptance.
3. Close `plugin/exr` focused acceptance if still outstanding.
4. When FFmpeg and still-image accumulation pass, mark the bounded current-generation milestone complete before considering SIMD/hardware acceleration, broader media scope, waveform/vectorscope expansion or a backend-neutral media wrapper.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
