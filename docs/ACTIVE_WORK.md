# Active Work

This file is the recovery authority for work currently in flight. After fetching `main`, read this file before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Current implementation line contains accepted Core/IO/Color/Analysis/Diagnostics/Imaging umbrella work plus later format slices for JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP and decode-only HEIF/AVIF.
- The historical validator failure at `deeb687c775e128ca4caf74b7438f8892662833e` predates RAW, WebP, HEIF/AVIF and both JPEG XL package repairs.
- The first JPEG XL source-boundary repair is `a2e03f5f2490a50aecab95451544a0d2e77e31fa`.

**TASK**

- `011C8-A/B1-R2` — close the remaining JPEG XL backend linker defect while continuing later format implementation in parallel.
- Next code-side format milestone: TIFF/OpenImageIO expansion.

**TOUCHED**

Latest JPEG XL linker repair:

- `jpegxl_src/import.ext`

Published later-format work already on `main` includes:

- camera RAW / LibRaw input support
- WebP exact-lossless static RGB/RGBA support
- decode-only HEIF/AVIF using pinned dav1d + libde265 + libheif

**STATUS**

- JPEG XL compile-layer repair: verified by Windows evidence; previous `lcms2.h` and `gtest/gtest.h` errors no longer reproduce.
- JPEG XL remaining linker root cause: pinned skcms production package omitted `src/skcms_TransformBaseline.cc`.
- skcms upstream build definition confirms `skcms.cc` depends on the baseline transform target; HSW/SKX targets are already disabled in `jpegxl_src.upp`.
- Baseline transform TU has now been added to the explicit production manifest.
- Windows post-linker-repair acceptance: pending.
- Development continues in coherent format slices while Gary/another Windows agent validates focused checkpoints.

**PUBLISHED**

- `a66e1192025032823e93a890e16cc3874034a8a4` — `Add skcms baseline transform to JPEG XL backend`.
- This `ACTIVE_WORK.md` update is the recovery-log follow-up on top of that implementation checkpoint; fetch `main` for its exact docs commit SHA.

**VALIDATION**

Completed before latest repair:

- `TASK 011C8-A/B1-R1-W1` on `f91de7d...` proved the libjxl/Highway source-manifest repair compiles through to link.
- libjxl/Brotli/Highway/skcms pins matched.
- `JPEGXL_ENABLE_SKCMS=1` was confirmed.
- lcms2 and googletest are no longer production dependencies.
- Failure moved to linker only: undefined `skcms_private::baseline::run_program`.
- Validator made no source changes.

Source review for latest repair:

- pinned skcms `BUILD.bazel` confirms `skcms_public` depends on `skcms_TransformBaseline` and x86 HSW/SKX targets separately;
- pinned `skcms.cc` initializes dispatch from `baseline::run_program`;
- `jpegxl_src.upp` already defines `SKCMS_DISABLE_HSW` and `SKCMS_DISABLE_SKX`;
- therefore the smallest coherent scalar fix is adding `src/skcms_TransformBaseline.cc` only.

Not yet completed:

- no Windows build/runtime result has been reported for `a66e1192` or later.

**NEXT ACTION**

Validator lane:

1. Fetch/fast-forward to current `origin/main`; verify `a66e1192` is an ancestor of HEAD and initialize required submodules.
2. Build `jpegxl_prereq_test` Debug. If compile/link/run fails, stop and report the first real current-main error without patching.
3. If Debug passes 9/0, run it five additional times; then Release once + five repeats.
4. If backend is green, run `jpegxl_oiio_test` Debug and Release once each; expected 10/0.
5. Stop at that checkpoint. Do not run the broad ImagingIO/OpenImageIO regression matrix yet.

Implementation lane:

1. Continue TIFF/OpenImageIO expansion as the next coherent format slice.
2. After TIFF is internally coherent, publish it and update this file.
3. Batch wider Windows regressions across JPEG XL + RAW + WebP + HEIF/AVIF + TIFF instead of serially blocking development on every internal package step.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
