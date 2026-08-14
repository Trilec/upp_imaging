# Active Work

This file is the recovery authority for work currently in flight. After fetching `main`, read this file before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Accepted framework foundation: ImagingCore, ImagingIO EXR/PNG baseline, ImagingColor, ImagingAnalysis, ImagingDiagnostics and the `Imaging` umbrella.
- Still-image format implementation line includes JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- JPEG XL backend is Windows-proven Debug/Release after `a66e1192`; the shared static OpenImageIO dependency-closure repair is `5ca436c3` and awaits Windows accumulation validation.
- FFmpeg is a separate media subsystem and is not part of ImagingIO or the Imaging umbrella.

**TASK**

- Validator lane: validate the repaired aggregate OpenImageIO closure and later still-image formats as one accumulation pass.
- Implementation lane: complete the first bounded FFmpeg MOV/H.264-to-RGBA8 video-frame decode stack.

**TOUCHED**

Latest FFmpeg implementation checkpoint:

- `ffmpeg_headers/generated/config.h`
- `ffmpeg_headers/generated/config_components.h`
- `ffmpeg_headers/generated/libavutil/ffversion.h`
- `ffmpeg_headers/ffmpeg_headers.upp`
- `ffmpeg_headers/README.md`
- `ffmpeg_avutil_src/*`
- `ffmpeg_avutil_test/*`

Previous OpenImageIO and FFmpeg header-pin checkpoints remain unchanged.

**STATUS**

- Shared OpenImageIO dependency-closure repair remains published at `5ca436c3ba6265f6431deaf7348332940051686d`.
- FFmpeg upstream remains pinned to signed `n9.0.1`, exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- The repository now owns the generated-equivalent Windows x86_64 / U++ CLANGx64 source configuration: static, LGPL-only, no external or inline assembly, no threads/network/external codecs/filters/devices/audio-resample/CLI/encoding.
- Architecture identity remains truthful (`ARCH_X86=1`, `ARCH_X86_64=1`); scalar behavior is obtained by disabling X86ASM/inline assembly rather than pretending the target is generic.
- `ffmpeg_avutil_src` is implemented from the complete pinned upstream base `libavutil` object list plus the no-Vulkan hardware stub, static swscale half-float helper and required x86 CPU C source. The production source manifest is explicit; no recursive globs or tests/tools are imported.
- `ffmpeg_avutil_test` is implemented as a linked runtime contract; expected result is `SUMMARY passed=13 failed=0`. It covers release/config/license identity, scalar CPU dispatch, buffers/frames, rational timing, aligned allocation, Win32 time and UTF-8 file mapping/cleanup.
- No Windows build/runtime result is yet claimed for FFmpeg; implementation continues before handoff.
- Next source boundary is `libavcodec` with native H.264 only, followed by MOV/file `libavformat` and scalar `libswscale`.
- AVIF output remains intentionally deferred; HEIF/AVIF stays decode-only in the still-image stack.

**PUBLISHED**

- `a66e1192025032823e93a890e16cc3874034a8a4` — JPEG XL skcms linker repair.
- `c37521e050cdb1c04583c0a5bdb06763742b1669` — TIFF OpenImageIO/ImagingIO expansion.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.
- `901332ce46387e0d09026cfd5f26d4528b8cd9d1` — FFmpeg 9.0.1 header/pin boundary.
- `21a72f6dfccfdc216b4f3cb180cb3ae9415c1c11` — `Add FFmpeg libavutil scalar source foundation`.
- This file is the recovery-log follow-up; fetch `main` for its exact docs commit SHA.

**VALIDATION**

Windows evidence already completed:

- JPEG XL backend Debug: 9/0, exit 0.
- JPEG XL backend Release: 9/0, exit 0.
- old JPEG XL lcms2/gtest/skcms-baseline failures are closed.
- prior direct OIIO JPEG XL build stopped in aggregate OIIO compilation on DPX OpenEXR and TIFF header visibility; validator made no edits.

Static/source review completed since that run:

- direct OIIO plugin dependencies were repaired at their owning package manifests;
- FFmpeg signed release pin and LGPL baseline were verified against upstream;
- FFmpeg public/generated header boundary is explicit and separate from implementation;
- exact pinned `libavutil/Makefile` base objects and `libavutil/x86/Makefile` were reconstructed into the U++ source manifest;
- x86 CPU source was retained while X86ASM/inline-asm paths are disabled;
- Windows CRT aliases, aligned allocation, Win32 time and file-map paths are represented in the generated configuration and focused test.

Not yet Windows-verified:

- aggregate OpenImageIO after `5ca436c3`;
- RAW/WebP/HEIF/TIFF accumulation matrix;
- `ffmpeg_headers_test` (expected 7/0);
- `ffmpeg_avutil_test` (expected 13/0);
- later FFmpeg implementation libraries.

**NEXT ACTION**

Validator lane:

1. Fetch current `origin/main`; confirm `5ca436c3` is an ancestor and status is clean.
2. Build/run `jpegxl_oiio_test` Debug/Release; if green, run focused RAW/WebP/HEIF/TIFF direct/framework tests as one accumulation pass.
3. Fail fast on the first current-main defect and report exact evidence without patching.
4. Do not start a separate FFmpeg validation loop yet; the implementation lane is deliberately continuing to the full first decode slice.

Implementation lane:

1. Reconstruct and implement the explicit native H.264 `libavcodec` closure from the pinned Makefile/configure selections, including required base/support objects but no unrelated decoders/encoders.
2. Add direct codec registration/allocation/decode-contract tests without external fixtures where possible.
3. Implement MOV/file `libavformat` closure and scalar `libswscale` conversion packages.
4. Add one deterministic first-frame decode fixture/contract only after those library boundaries are coherent.
5. Publish at meaningful recovery checkpoints and update this file; hand FFmpeg to Gary only when the first decode slice is code-side coherent.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
