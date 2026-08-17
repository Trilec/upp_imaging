# Active Work

This file is the recovery authority for work currently in flight. After fetching `main`, read this file before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Current-generation framework implementation is complete: ImagingCore, ImagingIO, ImagingColor, ImagingAnalysis, ImagingDiagnostics and the `Imaging` umbrella.
- Established Windows framework baseline: ImagingCore 48/0, ImagingIO 79/0 baseline, ImagingColor 66/0 plus independent OCIO 15/0, ImagingAnalysis 41/0, ImagingDiagnostics 33/0, Imaging umbrella 6/0.
- Code-side still-image line includes JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- JPEG XL prerequisite/backend is Windows-proven Debug/Release 9/0 after `a66e1192025032823e93a890e16cc3874034a8a4`.
- Shared static OpenImageIO plugin dependency repair is `5ca436c3ba6265f6431deaf7348332940051686d`; complete later-format accumulation is still pending.
- `plugin/exr` focused preview contract is implemented through `323c3dc29938de404fc3411b87dcaf6c6aea4559`; current 22-check Debug/Release Windows acceptance is pending.
- FFmpeg is a separate media subsystem, not part of ImagingIO or the Imaging umbrella.

**TASK**

- Final current-generation closure: complete accumulated Windows validation for repaired still-image/OpenImageIO, `plugin/exr`, and the FFmpeg first slice; repair only substantive current-main failures as coherent root-cause slices; then mark the bounded milestone complete.
- Documentation reconciliation is complete through the current documentation closure line; README, architecture, catalogue, status/roadmap, package layout and changelog now distinguish implemented vs Windows-proven vs platform-validation-pending state.

**TOUCHED**

Latest FFmpeg link-ownership closure:

- `ffmpeg_avformat_src/import.ext`
- `ffmpeg_avformat_src/README.md`
- `docs/ACTIVE_WORK.md`

Recent FFmpeg config/preflight closure:

- `ffmpeg_headers_test/main.cpp`
- `docs/FFMPEG_PLAN.md`
- `ffmpeg_headers/generated/config.h`
- `ffmpeg_headers/generated/config_components.h`

Latest EXR focused-contract closure:

- `plugin_exr_test/main.cpp`

Latest repository-state reconciliation:

- `README.md`
- `docs/ARCHITECTURE.md`
- `docs/PACKAGE_CATALOGUE.md`
- `docs/STATUS_AND_ROADMAP.md`
- `docs/package_layout.md`
- `CHANGELOG.md`
- this recovery file

**STATUS**

FFmpeg:

- Exact signed upstream pin: FFmpeg `n9.0.1`, commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- First-slice configuration remains static LGPL scalar Windows x86_64/U++ CLANGx64: no threads, network, external codecs, filters, devices, audio resampling, CLI/encoding, external/inline assembly or hardware acceleration.
- `ffmpeg_headers` owns generated/public configuration and compiles no implementation source; generated codec/parser/BSF/demuxer/muxer/protocol registries remain checked in but are not standalone header-package translation units.
- The 29-function FFmpeg `MATH_FUNCS` Windows UCRT capability repair is Windows-proven by `ffmpeg_avutil_test` Debug 13/0.
- `config.h` contains the complete 119-entry FFmpeg 9.0.1 `CONFIG_EXTRA` helper namespace for this reviewed configuration; selected H.264/MOV helpers are enabled and non-selected helpers such as `CONFIG_MPEGVIDEODEC` are explicit 0.
- Relevant external/autodetected options used by the compiled slice are explicit, including disabled `CONFIG_LIBLCEVC_DEC` and `CONFIG_LCMS2`.
- `config_components.h` explicitly disables all nine H.264 hardware-acceleration variants reviewed for the slice: D3D11VA, D3D11VA2, D3D12VA, DXVA2, NVDEC, VAAPI, VDPAU, VideoToolbox and Vulkan.
- Published `HAVE_DIRECT_H 1` at `d692702a41ea1e0f639f5fe55666f1dc133b7875` remains part of current ancestry.
- `ffmpeg_headers_test` now has an eighth parity gate that reads the four production manifests, recursively follows pinned FFmpeg/generated includes, collects `CONFIG_*`, `HAVE_*`, `ARCH_*` and `AV_HAVE_*`, and fails with the complete missing-definition list rather than allowing one-translation-unit-at-a-time discovery. Expected current result: 8/0.
- `ffmpeg_avutil_test` expected 13/0.
- `ffmpeg_avcodec_test` expected 12/0; native H.264 decoder only.
- `ffmpeg_avformat_test` expected 14/0; MOV/MP4 demux + local `file` protocol only.
- Pinned `libavformat/Makefile` ownership audit found the two missing live materializers from Gary's linker failure: `libavformat/to_upper4.c` (`ff_toupper4`) and `libavformat/mpegaudiotabs.c` (`ff_mpa_freq_tab` and tables). Both are now explicit in `ffmpeg_avformat_src/import.ext`; no codec/muxer/protocol/feature was added.
- The avformat manifest remains explicit/no-glob with 45 translation units.
- Parallel pinned `libswscale/Makefile` audit found no analogous first-slice ownership gap: all base and unconditional x86 C dispatch objects are represented; shared `log2_tab` and `half2float` materializers are supplied by avutil; SIMD/unstable assembly remains excluded.
- `ffmpeg_swscale_test` expected 13/0.
- `ffmpeg_first_frame_test` expected 27/0 and embeds the 1,463-byte one-frame 16x16 Constrained Baseline H.264 MP4; fixture FNV-1a `0x86d54178fbc2b70a`, decoded logical YUV420P FNV-1a `0x54009ba1a158e125`.

Still-image / plugin:

- Static OpenImageIO later-format code is implemented; post-`5ca436c3` Debug/Release accumulation remains the acceptance gate.
- `plugin_exr_test` is a real 22-check focused contract covering RGBA clamp/straight alpha, Gray, GrayAlpha, non-finite-to-zero, one-channel mask, named multichannel RGB selection, registration, invalid/truncated input and cleanup.

**PUBLISHED**

Important recent still-image/plugin line:

- `a66e1192025032823e93a890e16cc3874034a8a4` — JPEG XL skcms linker repair; backend later Windows-proven 9/0 Debug and Release.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.
- `323c3dc29938de404fc3411b87dcaf6c6aea4559` — expanded focused `plugin/exr` preview contract.

Important recent FFmpeg line:

- `d90f96a3a3046176257d5216d3023f377663c40b` — generated-config namespace repair checkpoint.
- `d692702a41ea1e0f639f5fe55666f1dc133b7875` — retain `HAVE_DIRECT_H 1` Windows capability.
- `74247ac00dc4da919a89b5f69995b790ba2606f0` — deterministic source/config parity audit in `ffmpeg_headers_test`.
- `9cf5d82806cfd0a13777e5361bdf7b3daf2c4b08` — parity-gate plan documentation.
- `bc5cea2befabe614320979ffb408a35e0187f8ea` — FFmpeg config parity recovery checkpoint.
- `fed4e6800bf2f1aee432f708633d3b6ed15c54e7` — add avformat `to_upper4.c` and `mpegaudiotabs.c` materializers.
- `375dbcf56eda47ee3f50954e78e2dbdf525a1ed4` — document avformat source ownership closure.
- `d4a6051e909fbe95c8930ad725e2b857e4efc3af` — FFmpeg avformat link-ownership recovery checkpoint.

Repository-state reconciliation line:

- `a05f300a5514b1bf250a902e080bdae40615a1e7` — reconcile top-level imaging repository status in README.
- `5f64a1cea60fbdec4cf7e65a81e5dc3be397fa0b` — reconcile architecture boundaries.
- The following documentation closure commit updates package catalogue, status/roadmap, package layout and changelog; fetch current `main` to obtain/verify its exact SHA before work.

**VALIDATION**

Windows-proven:

- Framework baseline: ImagingCore 48/0, ImagingIO 79/0 baseline, ImagingColor 66/0 + OCIO 15/0, ImagingAnalysis 41/0, ImagingDiagnostics 33/0, Imaging umbrella 6/0.
- JPEG XL prerequisite/backend 9/0 Debug and 9/0 Release after `a66e1192`.
- FFmpeg acceptance on the earlier `d90f96...` task base (with the `HAVE_DIRECT_H` correction now already published in current ancestry): precheck/pin passed; old headers contract 7/0 Debug; avutil 13/0 Debug; avcodec 12/0 Debug.
- That same FFmpeg run reached avformat link and failed only on undefined `ff_toupper4` and `ff_mpa_freq_tab`; Gary correctly stopped rather than editing the manifest. The owning source repair is now published.
- Gary reported `git diff --check` passed in that local validation state and made no commit/push.

Static/source review complete:

- Exact pinned FFmpeg configure dependency selections for H.264/MOV were inspected.
- Exact avformat and swscale pinned Makefile/source ownership was reviewed for the current source slice.
- Current README/architecture/catalogue/status/layout/changelog state has been reconciled without promoting pending Windows gates to accepted state.

Platform validation pending:

- post-`5ca436c3` OpenImageIO/later-format accumulation Debug/Release;
- `plugin_exr_test` 22/0 Debug/Release;
- current `ffmpeg_headers_test` 8/0 parity gate;
- current-main avutil/avcodec regression;
- repaired `ffmpeg_avformat_test` 14/0;
- `ffmpeg_swscale_test` 13/0;
- `ffmpeg_first_frame_test` 27/0;
- all six FFmpeg tests in Release;
- first-frame five Debug + five Release repeat runs, all 27/0 with clean shutdown/cleanup.

**NEXT ACTION**

1. Fetch/fast-forward current `origin/main`; record exact HEAD; require clean status. Main may have advanced.
2. Initialize/update `ffmpeg_headers/upstream` and require exact `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
3. Run the complete FFmpeg Debug lane in order: headers 8/0, avutil 13/0, avcodec 12/0, avformat 14/0, swscale 13/0, first-frame 27/0.
4. If Debug is fully green, run all six in Release with the same totals.
5. Repeat first-frame five times Debug and five times Release; every run must remain 27/0 with clean cleanup.
6. Independently run the repaired OpenImageIO/later-format accumulation in Debug and Release, then `plugin_exr_test` Debug/Release at 22/0.
7. Fail fast on the first substantive current-main defect. Gary may only make a genuinely tiny, unquestionably mechanical one-file correction; source manifests, dependencies, APIs, tests, component/feature enablement or multi-file changes return to the supervisor.
8. Group substantive failures by root cause and repair them as coherent source/config/dependency slices; do not return to one-line ping-pong.
9. When all bounded platform gates are green, update this file and status docs to mark the current-generation milestone complete before opening deferred next scope.

## Working rhythm

1. Remote GitHub `main` is authoritative; refresh HEAD before every implementation/publish action.
2. Implement larger coherent slices rather than one operation or compiler error per milestone.
3. Publish only internally coherent/reviewable checkpoints, but publish often enough that outages do not strand work.
4. Update this recovery file at each meaningful checkpoint with BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION.
5. Review full touched files, callers/tests/dependencies/package manifests and the complete diff before publishing.
6. Reserve broad regression matrices for accumulation points.
7. On recovery: fetch `main`, read this file first, then inspect the referenced current files/commits before continuing.
