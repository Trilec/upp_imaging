# Active Work

This file is the recovery authority for work currently in flight. After fetching `main`, read this file before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Accepted framework foundation: ImagingCore, ImagingIO EXR/PNG baseline, ImagingColor, ImagingAnalysis, ImagingDiagnostics and the `Imaging` umbrella.
- Format implementation line includes JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- JPEG XL backend repair is now Windows-proven in Debug and Release; the remaining failure observed by `011C8-A/B1-R2-W1` was in the shared static OpenImageIO plugin compilation closure, not libjxl/skcms.

**TASK**

- Validator lane: close the repaired shared OpenImageIO static-plugin boundary, then run one accumulation pass across the later still-image formats.
- Implementation lane: begin FFmpeg as a separate major subsystem without adding video semantics to ImagingIO.

**TOUCHED**

Latest coherent repair slice:

- `openimageio_plugin_dpxcineon/openimageio_plugin_dpxcineon.upp`
- `openimageio_plugin_jpegxl/openimageio_plugin_jpegxl.upp`
- `openimageio_plugin_png/openimageio_plugin_png.upp`
- `openimageio_plugin_raw/openimageio_plugin_raw.upp`
- `openimageio_plugin_webp/openimageio_plugin_webp.upp`
- `openimageio_plugin_heif/openimageio_plugin_heif.upp`

No plugin source files, public framework APIs, format policies or third-party source pins changed in this repair.

**STATUS**

- JPEG XL backend: Windows Debug/Release build, link and runtime pass at 9/0 after `a66e1192`; previous lcms2/gtest/skcms-baseline failures are closed.
- Direct OIIO JPEG XL validation on prior main stopped before plugin execution because the aggregate `OpenImageIO` package also compiled DPX/Cineon and TIFF-related consumers with incomplete direct include visibility.
- Shared static-plugin dependency closure is now repaired code-side:
  - DPX/Cineon receives the public OpenEXR include root required by `<OpenEXR/ImfTimeCode.h>`;
  - JPEG XL, PNG, RAW, WebP and HEIF declare the stable `libtiff` package and include root required by `OpenImageIO/tiffutils.h`;
  - PNG no longer reaches into `libtiff_src` without declaring the dependency;
  - HEIF receives the pinned libheif generated/API include roots and `LIBHEIF_STATIC_BUILD=1`, preventing Windows DLL-import annotations against the static backend.
- Camera RAW, WebP, decode-only HEIF/AVIF and TIFF remain code-side complete and await the accumulation Windows pass.
- AVIF output remains intentionally deferred because adding it cleanly requires a separate AOM/SVT/rav1e encoder backend; the current HEIF-family contract remains decode-only.
- Next major implementation subsystem is FFmpeg, kept separate from ImagingIO.

**PUBLISHED**

- `a66e1192025032823e93a890e16cc3874034a8a4` — `Add skcms baseline transform to JPEG XL backend`.
- `c37521e050cdb1c04583c0a5bdb06763742b1669` — `Add TIFF to OpenImageIO and ImagingIO`.
- `5ca436c3ba6265f6431deaf7348332940051686d` — `Fix static OpenImageIO plugin dependency closure`.
- This file is the recovery-log follow-up on top of that repair; fetch `main` for its exact docs commit SHA.

**VALIDATION**

Windows evidence from `TASK 011C8-A/B1-R2-W1` at `c64e304e...`:

- `a66e1192` ancestor check: pass.
- `jpegxl_prereq_test` Debug: build pass, `SUMMARY passed=9 failed=0`, exit 0.
- `jpegxl_prereq_test` Release: build pass, `SUMMARY passed=9 failed=0`, exit 0.
- old JPEG XL failures (`lcms2.h`, `gtest/gtest.h`, undefined `skcms_private::baseline::run_program`) do not reproduce.
- `jpegxl_oiio_test` stopped during aggregate OpenImageIO compilation on missing `<OpenEXR/ImfTimeCode.h>` from DPX and `tiff.h` through `OpenImageIO/tiffutils.h`.
- validator made no edits; status and `git diff --check` were clean.

Source review for `5ca436c3...`:

- pinned OIIO DPX source directly includes `<OpenEXR/ImfTimeCode.h>`;
- pinned OIIO JPEG XL, RAW, WebP and HEIF sources directly include `OpenImageIO/tiffutils.h`, whose public header includes `tiff.h`;
- pinned OIIO HEIF source directly includes `<libheif/heif_cxx.h>` and may include `<libheif/heif_properties.h>`;
- pinned libheif public export header uses `__declspec(dllimport)` on Windows unless `LIBHEIF_STATIC_BUILD` is defined;
- the repair changes six package manifests only and keeps all backend pins/source sets unchanged.

Not yet verified on Windows:

- aggregate OpenImageIO compilation after `5ca436c3`;
- direct OIIO JPEG XL Debug/Release 10/0;
- RAW/WebP/HEIF/TIFF accumulation-point compile/link/runtime matrix.

**NEXT ACTION**

Validator lane:

1. Fetch/fast-forward to current `origin/main`; confirm `5ca436c3` is an ancestor of HEAD and status is clean.
2. Build/run `jpegxl_oiio_test` Debug and Release. This is now a shared OpenImageIO closure checkpoint, not another libjxl backend test.
3. If aggregate OIIO compilation is green, run the focused RAW, WebP, HEIF/AVIF and TIFF direct/framework tests as one accumulation pass.
4. Fail fast on the first new current-main compiler/link/runtime defect and report exact evidence without patching.

Implementation lane:

1. Start FFmpeg as a separate package/framework subsystem; do not add movie/video semantics to ImagingIO.
2. Pin one upstream FFmpeg release and define the first bounded decode surface around `libavformat`, `libavcodec`, `libavutil` and `libswscale`; keep audio, filters, devices, CLI tools and encoding out of the first slice unless a dependency audit proves they are required.
3. Publish at coherent recovery points and update this file after each meaningful FFmpeg slice.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
