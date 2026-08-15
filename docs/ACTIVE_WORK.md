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
- FFmpeg validator lane: first Windows acceptance stopped at the `ffmpeg_headers_test` Debug compile gate on current-main packaging; the owning header-package manifest is repaired and requires a focused rerun before the rest of the matrix.

**TOUCHED**

Latest FFmpeg corrective checkpoint:

- `ffmpeg_headers/ffmpeg_headers.upp`
- this recovery file

The completed direct-package slice remains `FFmpeg/*`, `ffmpeg_first_frame_test/*` and `docs/FFMPEG_PLAN.md`.

Earlier source checkpoints remain unchanged: `ffmpeg_headers` generated configuration/submodule content, `ffmpeg_avutil_src`, `ffmpeg_avcodec_src`, `ffmpeg_avformat_src`, `ffmpeg_swscale_src` and their focused tests.

**STATUS**

- FFmpeg remains pinned to signed `n9.0.1`, exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- Generated-equivalent Windows x86_64 / U++ CLANGx64 configuration is static, LGPL-only and scalar: no threads/network/external codecs/filters/devices/audio-resample/CLI/encoding, no external/inline assembly, no hardware acceleration.
- `ffmpeg_headers` is again a true public-header/generated-config package: the six generated codec/parser/BSF/demuxer/muxer/protocol registry `.c` fragments remain checked in under `generated/` but are no longer standalone package translation units. They are consumed textually by the owning libavcodec/libavformat registry sources through the existing generated include route.
- `ffmpeg_headers_test` expected result: 7/0.
- `ffmpeg_avutil_test` expected result: 13/0.
- `ffmpeg_avcodec_test` expected result: 12/0; public codec registry is native H.264 decoder only, no encoder/parser/BSF.
- `ffmpeg_avformat_test` expected result: 14/0; public format closure is MOV/MP4 demux + local `file` protocol, no muxers.
- `ffmpeg_swscale_test` expected result: 13/0; deterministic limited-range ITU-601 YUV420P -> RGBA8 conversion.
- Stable direct `FFmpeg` package links the four implementation libraries and forwards standard FFmpeg API types through `FFmpeg/FFmpeg.h`; it adds no Imaging/U++ wrapper policy.
- `ffmpeg_first_frame_test` expected result: 27/0. It embeds a 1,463-byte one-frame 16x16 Constrained Baseline H.264 MP4, verifies standard 64-bit FNV-1a fixture evidence `0x86d54178fbc2b70a`, exact decoded logical YUV420P FNV-1a `0x54009ba1a158e125`, MOV/H.264 decode, RGBA conversion and cleanup.
- First-frame packet handling follows FFmpeg's send/receive contract: a packet is retained across `avcodec_send_packet()` `EAGAIN`, receive is attempted before retry, demux errors are preserved, EOF is drained with a null packet, and the swscale pointer is cleared with the other released resources.
- The FNV evidence review corrected the 64-bit offset basis to the standard `14695981039346656037`; fixture bytes, decoded-frame behavior, package manifests and FFmpeg configuration were unchanged.
- No successful Windows FFmpeg compile/link/runtime result is claimed yet.

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
- `c23a7d515a8a4696f7b7f6f7b1b61f5be89843ce` — harden first-frame send/receive lifecycle and cleanup.
- `452c66810603d5de8970938298d112bdfdf37359` — correct standard FNV-1a first-frame evidence.
- `f92b55b3a1fae900d9ef22e221b5fe25f002bfe1` — FFmpeg Windows-acceptance handoff checkpoint that exposed the header-package source-set defect.
- The header-package registry-fragment repair and this recovery update are committed together; fetch current `main` for the exact corrective checkpoint SHA.

**VALIDATION**

Completed Windows evidence:

- JPEG XL backend Debug 9/0 and Release 9/0; previous lcms2/gtest/skcms-baseline failures are closed.
- Prior direct OIIO JPEG XL build stopped on aggregate DPX/OpenEXR and TIFF header visibility; owning package repair is published at `5ca436c3` and remains unvalidated.
- FFmpeg acceptance at `f92b55b3a1fae900d9ef22e221b5fe25f002bfe1`: clean current-main checkout, required `c23a7d5` ancestor confirmed, upstream submodule initialized at `bf1b838f`; `ffmpeg_headers_test` Debug build failed before linking because six generated registry fragments were compiled standalone from `ffmpeg_headers`. Errors included unknown internal `FFCodec` / `FFCodecParser` / `FFBitStreamFilter` / `FFInputFormat` / `FFOutputFormat` / `URLProtocol` types and undeclared registry symbols. Matrix stopped correctly; no FFmpeg test executable ran and Release was not attempted.

Static/source review completed:

- OIIO direct dependencies were repaired at owning manifests, not tests.
- Exact FFmpeg libavutil/libavcodec/libavformat/libswscale source ownership was reconstructed from pinned Makefiles/configure selections; production manifests are explicit.
- H.264 recursion follows through CABAC/Golomb/chroma/DSP/parse/pred/qpel/SEI/VideoDSP, ITU-T T.35, ATSC A/53 and Dolby Vision RPU.
- MOV recursion follows through ISO-media -> MPEG4AUDIO and RIFFDEC; IAMF/zlib suggestions remain disabled.
- Generated codec/parser/BSF/demuxer/muxer/protocol registries are checked in instead of relying on absent configure output.
- Upstream `libavcodec/allcodecs.c`, `parsers.c` and `bitstream_filters.c` include the generated codec/parser/BSF list fragments only after their internal types/symbol declarations; `libavformat/allformats.c` and `protocols.c` do the same for muxer/demuxer/protocol lists. The U++ implementation packages already compile those owning sources with `../ffmpeg_headers/generated` on their include path, so the fragments must not also be package-level translation units.
- x86 architecture identity remains truthful while assembly dispatch is disabled.
- The direct package depends only on FFmpeg implementation/header packages; no Imaging, CtrlLib, Workbench, external codec or networking dependency was introduced.
- End-to-end fixture is embedded in test source; no external runtime asset/download is required.
- First-frame decode lifecycle was re-reviewed against the pinned FFmpeg send/receive API; packet retry/drain/EOF error propagation and swscale cleanup are explicit before Windows handoff.
- FNV-1a evidence was independently recalculated from the unchanged embedded MP4 and decoded logical YUV420P bytes using the standard 64-bit offset basis.
- Header-package repair removes only the six generated registry `.c` entries from `ffmpeg_headers.upp`; generated files, implementation manifests, registries, enabled components, tests and public API are unchanged.

Not yet Windows-verified:

- OpenImageIO accumulation pass after `5ca436c3`.
- Corrected `ffmpeg_headers_test` build/run.
- Remaining FFmpeg focused/end-to-end tests in Debug and Release, plus repeatability/clean shutdown.

**NEXT ACTION**

Still-image validator lane (independent):

1. Fetch/fast-forward current `origin/main`; require clean status.
2. Run `jpegxl_oiio_test` Debug/Release. If green, run the focused RAW/WebP/HEIF/TIFF direct/framework accumulation matrix. Fail fast on first current-main defect and make no edits.

FFmpeg validator lane:

1. Fetch/fast-forward current `origin/main`; require clean status and initialize `ffmpeg_headers/upstream` at the pinned commit if needed.
2. Confirm the corrective header-package checkpoint is an ancestor of current HEAD.
3. Re-run `ffmpeg_headers_test` Debug first. It must build, run and report 7/0 before proceeding.
4. If green, continue the accumulated FFmpeg matrix in order: `ffmpeg_avutil_test`, `ffmpeg_avcodec_test`, `ffmpeg_avformat_test`, `ffmpeg_swscale_test`, `ffmpeg_first_frame_test` in Debug and Release, then repeat the end-to-end first-frame test for lifecycle/cleanup stability.
5. Fail fast on the first current-main compile/link/runtime defect and report evidence without patching.

Implementation lane after Windows evidence:

1. If another current-main compile/link/runtime defect is reported, repair the owning package/config/source closure here, publish, then issue a focused validator rerun.
2. If all first-slice acceptance passes, close the initial FFmpeg milestone before considering SIMD/hardware acceleration, broader containers/codecs, seeking/index behavior, audio, or a backend-neutral U++ media wrapper.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
