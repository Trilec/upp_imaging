# Active Work

Recovery authority for work currently in flight. After fetching `main`, read this file before using chat history or starting validation. `docs/WINDOWS_ACCEPTANCE.md` is the self-contained validator contract and remains authoritative for gate order and expected totals.

## BASE

- The latest Windows final-acceptance run used exact SHA `045bb30f5a247b79a82602cd181aabce447eca15` and stopped compiling `openimageio_io_test` Debug on LibRaw math constants.
- The LibRaw integration repair is published as `c644ba06a3671e591699d88e8046ae00599e3dac`.
- The earlier DPX/Cineon Blitz repair remains `0523b40b0d1b09798de80760244a27f45b5ccf1b` and is an ancestor of the current repair line.
- Current-generation framework implementation remains complete: `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics`, `Imaging`.
- Established Windows framework baseline remains: ImagingCore 48/0, ImagingIO 79/0 baseline, ImagingColor 66/0 + independent OCIO 15/0, ImagingAnalysis 41/0, ImagingDiagnostics 33/0, Imaging umbrella 6/0.
- Code-side still-image line remains JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- `plugin/exr` expanded focused contract remains 22 checks, Windows Debug/Release pending.
- FFmpeg remains a separate media subsystem from ImagingIO/Imaging.

## TASK

Continue final current-generation Windows acceptance from current published `main`. First re-run `openimageio_io_test` Debug and require 21/0. If green, continue the remaining still-image Debug targets in the exact order in `docs/WINDOWS_ACCEPTANCE.md`; stop on the first substantive failure. Only after the full still-image Debug lane is green proceed to Release, then `plugin_exr_test`, then the bounded FFmpeg first slice and repeatability.

Repair only real current-main failures as coherent root-cause slices. Keep supplementary real-file interoperability evidence separate from deterministic repository-owned gates.

## TOUCHED

Current repair slice:

- `libraw_src/libraw_src.upp`
- `libraw_src/README.md`
- `docs/ACTIVE_WORK.md`

Relevant inspected dependency/source slice:

- `libraw_src/import.ext`
- pinned LibRaw 0.22.2 `src/decoders/decoders_dcraw.cpp`
- pinned LibRaw 0.22.2 `internal/libraw_cxx_defs.h`
- pinned LibRaw 0.22.2 `internal/defines.h`
- `.gitmodules`

## STATUS

### Windows acceptance defect 2 — LibRaw math constants

Validator report for exact SHA `045bb30f5a247b79a82602cd181aabce447eca15`:

- starting worktree clean;
- `openimageio_io_test` Debug failed during compilation before execution;
- first useful errors were undeclared `M_PI` and `M_SQRT1_2` in pinned LibRaw `src/decoders/decoders_dcraw.cpp`;
- remaining acceptance gates were not run under fail-fast policy;
- no local edits;
- final worktree clean.

Source review confirmed the integration mechanism against pinned LibRaw 0.22.2 commit `b93f6e45c194f5df9b02a43b1af9a54b4f41f33f`:

- `decoders_dcraw.cpp` uses `M_PI` and `M_SQRT1_2`;
- `internal/libraw_cxx_defs.h` includes `<math.h>` before it includes `internal/defines.h`;
- `internal/defines.h` later defines `_USE_MATH_DEFINES` and includes `<math.h>` again;
- under the Windows CRT, defining `_USE_MATH_DEFINES` after the first `<math.h>` include is too late to expose those constants;
- `libraw_src/libraw_src.upp` previously supplied only `LIBRAW_NODLL` and `LIBRAW_BUILDLIB`.

Repair `c644ba06a3671e591699d88e8046ae00599e3dac` therefore supplies `-D_USE_MATH_DEFINES=1` at the U++ LibRaw package compile boundary. This makes the compatibility definition visible before the first upstream `<math.h>` include while keeping pinned LibRaw source unchanged.

The repair does **not** change LibRaw source files, the source manifest, dependencies, APIs, tests, static registration, format policy, upstream pin or supported functionality.

### Earlier DPX/Cineon Blitz defect

The first acceptance run at `19989324cb411fe46d229a0d1fa7cdd51ee7e69f` failed because U++ Blitz aggregated DPX and Cineon upstream sources and exposed an `InStream` namespace collision. Repair `0523b40b0d1b09798de80760244a27f45b5ccf1b` marks the importing `openimageio_plugin_dpxcineon` package `noblitz`, preserving upstream translation-unit assumptions. The later run at `045bb30...` progressed to the LibRaw compile failure, so DPX/Cineon was no longer the first blocker; complete package acceptance still awaits a green `openimageio_io_test` run.

### Remaining deterministic acceptance

Use `docs/WINDOWS_ACCEPTANCE.md` for the exact current matrix and totals. Required broad order remains:

1. sixteen still-image targets in Debug;
2. the same sixteen targets in Release;
3. `plugin_exr_test` Debug and Release, 22/0 each;
4. six FFmpeg first-slice gates in Debug and Release;
5. five additional Debug and five additional Release `ffmpeg_first_frame_test` runs, every run 27/0 with clean shutdown.

Supplementary real-camera RAW/DNG decode, real 8/10-bit AVIF/HEIC decode and animated-WebP rejection remain separate external-fixture evidence and must not block deterministic acceptance when suitable provenance-reviewed fixtures are unavailable.

## PUBLISHED

Most relevant checkpoints:

- `c644ba06a3671e591699d88e8046ae00599e3dac` — enable LibRaw Windows math constants at the package boundary.
- `045bb30f5a247b79a82602cd181aabce447eca15` — previous acceptance/recovery HEAD; failed on LibRaw math constants.
- `0523b40b0d1b09798de80760244a27f45b5ccf1b` — prevent DPX/Cineon Blitz namespace collision.
- `32e9f32666aeee129400e5562ad395ee0a94435f` — self-contained Windows acceptance contract.
- `b2b14c8cea9332ed6f997c5f0edd4825826eb353` — exact still-image acceptance matrix.
- `323c3dc29938de404fc3411b87dcaf6c6aea4559` — expanded `plugin/exr` preview contract.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.

## VALIDATION

### REPORTED — Windows acceptance at `045bb30...`

- clean starting and final worktree;
- `openimageio_io_test` Debug failed compiling pinned LibRaw on undeclared `M_PI` / `M_SQRT1_2`;
- expected package result was 21/0 but execution was never reached;
- no local edits;
- remaining gates not run.

### VERIFIED — source/static review

- current failure sites use the missing math constants exactly as reported;
- pinned LibRaw include order places the first `<math.h>` include before its internal `_USE_MATH_DEFINES` definition;
- the U++ package lacked an early compatibility definition;
- package-level compile definition is the smallest boundary repair and leaves pinned upstream source unchanged;
- repair is published at `c644ba06a3671e591699d88e8046ae00599e3dac`.

### PLATFORM VALIDATION PENDING

- `openimageio_io_test` Debug 21/0 on current repaired `main`;
- remaining still-image Debug matrix, then full Release matrix;
- `plugin_exr_test` 22/0 Debug/Release;
- complete FFmpeg Debug/Release matrix and repeatability;
- supplementary real-file evidence separately pending/unavailable as applicable.

## NEXT ACTION

1. Fetch/fast-forward `origin/main`, record exact HEAD and require a clean worktree.
2. Require LibRaw repair `c644ba06a3671e591699d88e8046ae00599e3dac` to be the tested HEAD or an ancestor of the exact current `main` HEAD.
3. Read `docs/WINDOWS_ACCEPTANCE.md`.
4. Rebuild/run `openimageio_io_test` Debug first; require 21/0.
5. If green, continue the remaining still-image Debug targets in documented order and stop at the first substantive failure.
6. Any substantive source-manifest, dependency, API, test, feature/component, format-policy or multi-file defect returns to the supervisor.
7. After every supervisor repair, publish a coherent checkpoint and resume validation from the new exact SHA.

## WORKING RHYTHM

- Remote GitHub `main` is authoritative; refresh before every edit/publish.
- Diagnose the first root failure, make the smallest coherent subsystem repair, and do not chase cascaded errors.
- Preserve pinned upstream source when the defect belongs to U++ integration/build boundaries.
- Keep VERIFIED, REPORTED, INFERRED and UNKNOWN evidence distinct.
- Separate source review from Windows/runtime acceptance.
