# Active Work

This file is the recovery authority for the work currently in flight. Read it after fetching `main` before reconstructing state from chat or starting a validation matrix.

## Current checkpoint

**BASE**

- Implementation parent for the current JPEG XL repair: `7623e5013398b41a6f8d1c3eb97d5e6b009ecbcc` (`Add decode-only HEIF and AVIF support`).
- The older validator run at `deeb687c775e128ca4caf74b7438f8892662833e` predates RAW, WebP, HEIF/AVIF and the JPEG XL manifest repair. Its compile failure is diagnostic history, not validation of current `main`.

**TASK**

- `011C8-A/B1` — recover focused JPEG XL Windows acceptance while allowing later format implementation slices to remain published and reviewable on `main`.

**TOUCHED**

Current repair slice:

- `jpegxl_src/jpegxl_src.upp`
- `jpegxl_src/import.ext`
- `jpegxl_src/README.md`

Published format work already present on `main` also includes camera RAW, WebP, and decode-only HEIF/AVIF slices. Those later slices are not rolled back merely because JPEG XL acceptance is pending.

**STATUS**

- JPEG XL production source manifest repair: implemented and published.
- Recursive libjxl source selection was replaced by the pinned upstream production translation-unit set.
- Recursive Highway source selection was replaced by Highway's seven core library translation units.
- `JPEGXL_ENABLE_SKCMS=1` is explicit; lcms2 is not a production dependency.
- libjxl test helpers and googletest are outside the production package.
- Windows post-repair acceptance: pending.
- Development may continue in coherent format slices while Gary/another Windows agent validates meaningful checkpoints.

**PUBLISHED**

- Latest implementation checkpoint: `a2e03f5f2490a50aecab95451544a0d2e77e31fa` — `Fix JPEG XL production source manifest`.
- This `ACTIVE_WORK.md` bootstrap is a repository-state follow-up commit on top of that implementation checkpoint; fetch `main` for its exact metadata commit SHA.

**VALIDATION**

Completed before the repair:

- `TASK 011C8-A/B1-W1` stopped correctly at `deeb687c` on the first focused Debug build.
- Failure evidence: `jxl_cms.cc` reached `lcms2.h` because skcms was not selected; recursive source selection also pulled test code that reached `gtest/gtest.h`.
- Working tree was clean and no validator patch was made.

Completed during repair:

- Static comparison against pinned libjxl 0.12.0 CMake/Bazel source definitions.
- Static comparison against pinned Highway 1.2.0 core-library source definitions.
- Repair commit contains only the three intended `jpegxl_src` package files.

Not yet completed:

- No Windows build or runtime result has been reported for `a2e03f5` or later.

**NEXT ACTION**

Run a small focused Windows checkpoint, not the old giant matrix:

1. Fetch/fast-forward to current `origin/main`, initialize recursive submodules, confirm clean status, and verify `a2e03f5` is an ancestor of HEAD.
2. Build and run `jpegxl_prereq_test` Debug. If build or run fails, stop and report the first real compiler/link/runtime error without patching.
3. If Debug passes, repeat it 5 times, then build/run Release and repeat 5 times.
4. If the direct backend passes, build/run `jpegxl_oiio_test` Debug and Release once each, checking RGB8, RGBA8/alpha and malformed-input contracts.
5. Stop at that checkpoint and report. Do not run the broad OpenImageIO/ImagingIO regression matrix yet.
6. After focused JPEG XL acceptance is green, batch the broader regression matrix with the later RAW/WebP/HEIF work so one meaningful validator pass covers several coherent implementation slices.

## Working rhythm

1. Implement larger coherent slices rather than turning every operation into a milestone.
2. Publish when a slice is internally coherent, reviewable and recoverable; do not publish every tiny edit.
3. Update this file at each meaningful publish with `BASE / TASK / TOUCHED / STATUS / PUBLISHED / VALIDATION / NEXT ACTION`.
4. Use focused validator checkpoints while implementation continues in parallel where dependency boundaries permit it.
5. Reserve broad regression matrices for accumulation points where they validate several coherent slices at once.
6. On session recovery: fetch `main`, read this file first, inspect the referenced published checkpoint, then continue from `NEXT ACTION`.
