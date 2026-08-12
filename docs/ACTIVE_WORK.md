# Active Work

This file is the recovery authority for work currently in flight. After fetching `main`, read this file before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Accepted framework foundation: ImagingCore, ImagingIO EXR/PNG baseline, ImagingColor, ImagingAnalysis, ImagingDiagnostics and the `Imaging` umbrella.
- Format implementation line now includes JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- The historical JPEG XL failures at `deeb687c...` and `f91de7d...` are diagnostic history; current JPEG XL repair starts at `a66e1192025032823e93a890e16cc3874034a8a4`.

**TASK**

- Validator lane: `011C8-A/B1-R2` — focused JPEG XL backend/OIIO acceptance after the skcms baseline-link repair.
- Implementation lane: continue the remaining format roadmap without waiting on that validator lane.

**TOUCHED**

Latest coherent implementation slice:

- `openimageio_plugin_tiff/*`
- `OpenImageIO/OpenImageIO.upp`
- `OpenImageIO/OIIO.cpp`
- `ImagingIO/FormatPolicy.cpp`
- `tiff_oiio_test/*`
- `tiff_imagingio_test/*`

Explicitly unchanged by the TIFF slice:

- `libtiff_src/*` — existing strict vendored libtiff 4.7.2 backend reused as-is
- `.gitmodules`
- `ImagingIO/ImagingIO.cpp`
- public ImagingIO headers/API

**STATUS**

- JPEG XL R2 linker repair: implementation complete; Windows focused validation pending.
- Camera RAW: code-side complete; broader Windows acceptance pending.
- WebP: code-side complete; broader Windows acceptance pending.
- HEIF/AVIF: decode-only code-side slice complete; broader Windows acceptance pending. AVIF output remains a separate encoder/backend milestone rather than an implied capability.
- TIFF: code-side complete for the first strict ImagingIO slice; Windows validation pending.
- TIFF direct OIIO registration uses the existing bundled libtiff 4.7.2 Windows CLANGx64 package rather than adding or replacing a third-party dependency.
- TIFF ImagingIO policy: `.tif/.tiff`, single-image/non-deep 2D through the shared structural inspector, zero-origin, UInt8/UInt16/Float32, Gray/GrayAlpha/RGB/RGBA, straight alpha, ZIP/Deflate output, verified transactional saves.
- TIFF does not claim JPEG/OJPEG, JBIG, LERC, LZMA, Zstd or WebP-in-TIFF support in this slice.

**PUBLISHED**

- JPEG XL linker repair: `a66e1192025032823e93a890e16cc3874034a8a4` — `Add skcms baseline transform to JPEG XL backend`.
- TIFF integration: `c37521e050cdb1c04583c0a5bdb06763742b1669` — `Add TIFF to OpenImageIO and ImagingIO`.
- This file is the recovery-log follow-up on top of the TIFF implementation commit; fetch `main` for the exact current docs SHA.

**VALIDATION**

Verified by repository/source review:

- TIFF publish is one fast-forward commit over the prior active-work checkpoint.
- TIFF commit changes only the intended OIIO registration/package, private `FormatPolicy`, and focused tests.
- Existing `libtiff_src` is version 4.7.2 and already configured for static Windows CLANGx64 with LZW, PackBits, ZIP/zlib and libdeflate; external JPEG/LERC/LZMA/Zstd/WebP codecs remain disabled.
- OIIO 3.1.15.0 TIFF exports and extension tables were checked against the pinned upstream source.
- TIFF load zero-origin enforcement already lives in the shared ImagingIO load path through `RequiresZeroOrigin`.
- focused direct TIFF contract target: `13 passed, 0 failed`.
- focused ImagingIO TIFF contract target: `29 passed, 0 failed`.

Not yet verified on Windows:

- JPEG XL R2 at/after `a66e1192`.
- RAW/WebP/HEIF/TIFF accumulation-point runtime matrix.
- TIFF Debug/Release compile/link/runtime.

**NEXT ACTION**

Validator lane:

1. Fetch/fast-forward to current `origin/main`; confirm `a66e1192` is an ancestor of HEAD.
2. Build/run `jpegxl_prereq_test` Debug; expected 9/0. If green, five repeats, then Release + five repeats.
3. If backend is green, run `jpegxl_oiio_test` Debug/Release once; expected 10/0.
4. Stop and report. Do not run the broad format matrix until that focused prerequisite is green.

Implementation lane:

1. Resolve the remaining HEIF/AVIF output boundary: package a reproducible AV1 encoder only if it can be done cleanly without importing GPL HEVC encoding or build-time generator dependence.
2. If AVIF output is not a clean bounded slice, leave HEIF/AVIF explicitly decode-only and move to FFmpeg as the next separate major milestone.
3. At the next coherent implementation publish, update this file again.
4. Once JPEG XL focused acceptance is green, batch one Windows accumulation pass across JPEG XL + RAW + WebP + HEIF/AVIF + TIFF rather than serially retesting every internal commit.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
