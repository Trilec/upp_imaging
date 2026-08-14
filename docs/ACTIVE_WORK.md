# Active Work

This file is the recovery authority for work currently in flight. After fetching `main`, read this file before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Accepted framework foundation: ImagingCore, ImagingIO EXR/PNG baseline, ImagingColor, ImagingAnalysis, ImagingDiagnostics and the `Imaging` umbrella.
- Still-image format implementation line includes JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- JPEG XL backend is Windows-proven Debug/Release after `a66e1192`; the shared static OpenImageIO dependency-closure repair is `5ca436c3` and awaits Windows accumulation validation.
- FFmpeg is a separate media subsystem and is not part of ImagingIO or the Imaging umbrella.

**TASK**

- Validator lane: validate the repaired aggregate OpenImageIO closure and then RAW/WebP/HEIF/TIFF as one accumulation pass.
- Implementation lane: build the first bounded FFmpeg video-frame decode stack from the pinned upstream release.

**TOUCHED**

Latest FFmpeg checkpoint:

- `.gitmodules`
- `ffmpeg_headers/upstream` pinned gitlink
- `ffmpeg_headers/*`
- `ffmpeg_headers_test/*`
- `docs/FFMPEG_PLAN.md`

Previous shared OpenImageIO repair touched only six plugin `.upp` manifests and remains unchanged.

**STATUS**

- Shared OpenImageIO dependency-closure repair remains published at `5ca436c3ba6265f6431deaf7348332940051686d`.
- FFmpeg upstream release is pinned to signed tag `n9.0.1`, exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`, tagged 2026-08-12.
- `ffmpeg_headers` is implemented as a strict header-only boundary over the submodule, with repository-owned `generated/libavutil/avconfig.h` for Windows CLANGx64 public headers.
- `ffmpeg_headers_test` is implemented; expected compile/runtime result is `SUMMARY passed=7 failed=0`. It links no FFmpeg implementation objects.
- The FFmpeg implementation configuration is explicitly LGPL-only: no GPL/nonfree enablement, no external codecs, no filters/devices/CLI, no encoding in the first slice.
- Planned first decode closure remains libavutil + libavcodec + libavformat + libswscale, MOV/MP4 demuxing, native H.264 decode, local file input and RGBA8 conversion.
- AVIF output remains intentionally deferred; HEIF/AVIF stays decode-only in the still-image stack.

**PUBLISHED**

- `a66e1192025032823e93a890e16cc3874034a8a4` — JPEG XL skcms linker repair.
- `c37521e050cdb1c04583c0a5bdb06763742b1669` — TIFF OpenImageIO/ImagingIO expansion.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.
- `901332ce46387e0d09026cfd5f26d4528b8cd9d1` — `Pin FFmpeg 9.0.1 public header boundary`.
- This file is the recovery-log follow-up on top of that FFmpeg checkpoint; fetch `main` for its exact docs commit SHA.

**VALIDATION**

Windows evidence already completed:

- JPEG XL backend Debug: 9/0, exit 0.
- JPEG XL backend Release: 9/0, exit 0.
- old JPEG XL lcms2/gtest/skcms-baseline failures are closed.
- prior direct OIIO JPEG XL build stopped in aggregate OIIO compilation on DPX OpenEXR and TIFF header visibility; validator made no edits.

Source review completed since that run:

- direct plugin dependencies were repaired at their owning package manifests, not in tests;
- FFmpeg `n9.0.1` signed tag and exact commit verified against upstream;
- FFmpeg license file confirms the default codebase is LGPL-2.1+ and GPL portions require explicit enablement;
- upstream H.264 and MOV configure dependency selections and Makefile object ownership have been inspected for the next source-manifest step.

Not yet Windows-verified:

- aggregate OpenImageIO after `5ca436c3`;
- RAW/WebP/HEIF/TIFF accumulation matrix;
- `ffmpeg_headers_test`;
- any FFmpeg implementation library.

**NEXT ACTION**

Validator lane:

1. Fetch current `origin/main`; confirm `5ca436c3` is an ancestor and status is clean.
2. Build/run `jpegxl_oiio_test` Debug/Release; if green, run focused RAW/WebP/HEIF/TIFF direct/framework tests as one accumulation pass.
3. Fail fast on first current-main defect and report exact evidence without patching.
4. Do not validate FFmpeg yet; implementation work is still progressing beyond the header checkpoint.

Implementation lane:

1. Reconstruct the exact Windows CLANGx64 FFmpeg generated-configuration slice for the selected libraries/components.
2. Implement explicit `_src` package manifests from upstream Makefiles; no recursive production globs.
3. Bring up libavutil first, then H.264 codec closure, MOV/file demux closure and libswscale conversion.
4. Publish coherent recovery checkpoints while continuing; do not hand FFmpeg to Gary until the full first decode slice is code-side coherent.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
