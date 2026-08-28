# Active Work

Recovery authority for work currently in flight. After fetching `main`, read this file before using chat history or starting validation. `docs/WINDOWS_ACCEPTANCE.md` is the self-contained validator contract and remains authoritative for gate order and expected totals.

## BASE

- Latest Windows final-acceptance task `IMG-WA-003` used exact SHA `3e33ec9fc39f504ec674edee4f9dbbabdbf3bb2b` and stopped during dav1d archive creation because the U++ package entry `generated/*.c` survived object naming as literal `generated_*.c.o` and was passed to `llvm-ar`.
- The dav1d generated-wrapper manifest repair is published as `b2d53719fdc926aed49f8a3bf90b10279577d33d`.
- The earlier dav1d Windows large-file config repair remains `623b5d30a6755b9ca96a9774e0fb813620d94c10`.
- The earlier libheif C++20 source-boundary repair remains `16aa8472d6e292e8bda18d440aaf26ff46004073`.
- The earlier LibRaw Windows math-constant repair remains `c644ba06a3671e591699d88e8046ae00599e3dac`.
- The earlier DPX/Cineon Blitz repair remains `0523b40b0d1b09798de80760244a27f45b5ccf1b`.
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

- `dav1d_src/dav1d_src.upp`
- `dav1d_src/README.md`
- `docs/ACTIVE_WORK.md`

Relevant inspected dependency/source/build slice:

- `dav1d_src/import.ext`
- all 26 repository-owned files under `dav1d_src/generated/`
- representative generated wrapper `dav1d_src/generated/8_cdef.c`
- pinned dav1d 1.5.4 upstream source boundary
- prior dav1d Windows config repair and validator evidence

## STATUS

### Windows acceptance defect 5 — dav1d generated-wrapper wildcard

Validator report for task `IMG-WA-003` at exact SHA `3e33ec9fc39f504ec674edee4f9dbbabdbf3bb2b`:

- starting worktree clean;
- `openimageio_io_test` Debug progressed through dav1d compilation to archive creation;
- `llvm-ar` failed on literal object path `generated_*.c.o` with `Invalid argument`;
- `dav1d_src/dav1d_src.upp` contained `generated/*.c` as a compiled source entry;
- remaining acceptance gates were not run under fail-fast policy;
- no local edits;
- final worktree clean.

Repository/source review confirmed the package boundary:

- `dav1d_src/import.ext` explicitly owns the ordinary scalar dav1d sources plus the Windows thread shim;
- `dav1d_src/generated/` contains exactly 26 repository-owned wrapper files: 13 upstream bit-depth templates instantiated once for 8-bit and once for 16-bit;
- representative wrappers only set `BITDEPTH` and include the matching pinned upstream template source;
- the wrapper set is therefore fixed repository source, not runtime-generated or discovery-dependent input;
- the wildcard package entry did not expand into concrete archive object membership under the observed U++ build and instead survived as the literal object identity reported by `llvm-ar`.

Repair `b2d53719fdc926aed49f8a3bf90b10279577d33d` replaces `generated/*.c` with the exact 26 wrapper paths in `dav1d_src.upp`. This keeps source ownership deterministic and preserves exactly the existing 13-by-2 wrapper set.

The repair does **not** change wrapper contents, pinned dav1d source, upstream pin, ordinary source selection, dependencies, API, tests, AV1 feature policy, bit-depth coverage or SIMD policy.

### Windows acceptance defect 4 — dav1d Windows large-file config branch

Task `IMG-WA-002` at `dfe34e2f1ec2c65b33cd982eedb633ff02934694` failed because repository-generated Windows config aliased existing `fseeko`/`ftello` declarations to `_fseeki64`/`_ftelli64`. Upstream dav1d 1.5.4 feature-tests those names and, when present, uses `_FILE_OFFSET_BITS=64` instead. Repair `623b5d30a6755b9ca96a9774e0fb813620d94c10` selected that upstream branch. Task `IMG-WA-003` progressed beyond compilation to archive creation.

### Windows acceptance defect 3 — libheif C++20 option ordering

Task `IMG-WA-001` at `1847010c4f6745f5440ff28f4796d83781b62ece` failed because libheif 1.23.1 requires C++20 while U++ CLANGx64's C++17 option followed the package-wide C++20 option. Repair `16aa8472d6e292e8bda18d440aaf26ff46004073` moved C++20 to the imported-source boundary so it appears after the U++ method default. Later runs progressed beyond those libheif errors.

### Windows acceptance defect 2 — LibRaw math constants

The run at `045bb30f5a247b79a82602cd181aabce447eca15` failed compiling pinned LibRaw on undeclared `M_PI` / `M_SQRT1_2`. Repair `c644ba06a3671e591699d88e8046ae00599e3dac` supplies `_USE_MATH_DEFINES` at the U++ LibRaw package boundary while leaving pinned upstream source unchanged. Later runs progressed beyond this failure.

### Windows acceptance defect 1 — DPX/Cineon Blitz collision

The first acceptance run at `19989324cb411fe46d229a0d1fa7cdd51ee7e69f` failed because U++ Blitz aggregated DPX and Cineon upstream sources and exposed an `InStream` namespace collision. Repair `0523b40b0d1b09798de80760244a27f45b5ccf1b` marks the importing `openimageio_plugin_dpxcineon` package `noblitz`, preserving upstream translation-unit assumptions. Later runs progressed beyond this failure.

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

- `b2d53719fdc926aed49f8a3bf90b10279577d33d` — enumerate the exact dav1d generated wrapper source set.
- `3e33ec9fc39f504ec674edee4f9dbbabdbf3bb2b` — validator base for `IMG-WA-003`; failed during dav1d archive creation on literal wildcard object name.
- `623b5d30a6755b9ca96a9774e0fb813620d94c10` — fix dav1d Windows large-file generated config branch.
- `16aa8472d6e292e8bda18d440aaf26ff46004073` — apply libheif C++20 at imported-source boundary.
- `c644ba06a3671e591699d88e8046ae00599e3dac` — enable LibRaw Windows math constants at the package boundary.
- `0523b40b0d1b09798de80760244a27f45b5ccf1b` — prevent DPX/Cineon Blitz namespace collision.
- `32e9f32666aeee129400e5562ad395ee0a94435f` — self-contained Windows acceptance contract.
- `b2b14c8cea9332ed6f997c5f0edd4825826eb353` — exact still-image acceptance matrix.
- `323c3dc29938de404fc3411b87dcaf6c6aea4559` — expanded `plugin/exr` preview contract.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.

## VALIDATION

### REPORTED — task `IMG-WA-003` at `3e33ec9...`

- clean starting and final worktree;
- `openimageio_io_test` Debug reached dav1d archive creation;
- `llvm-ar` rejected literal `generated_*.c.o` with `Invalid argument`;
- expected package result was 21/0 but execution was never reached;
- no local edits;
- remaining gates not run.

### VERIFIED — source/build review

- the repository contains exactly 26 generated wrapper `.c` files, matching the documented 13 templates × two bit depths;
- ordinary dav1d source ownership is separate in `import.ext`;
- generated wrappers are repository-owned fixed source files rather than discovered build artifacts;
- explicit package enumeration is therefore the smallest coherent source-manifest repair;
- repair is published at `b2d53719fdc926aed49f8a3bf90b10279577d33d`.

### PLATFORM VALIDATION PENDING

- `openimageio_io_test` Debug 21/0 on current repaired `main`;
- remaining still-image Debug matrix, then full Release matrix;
- `plugin_exr_test` 22/0 Debug/Release;
- complete FFmpeg Debug/Release matrix and repeatability;
- supplementary real-file evidence separately pending/unavailable as applicable.

## NEXT ACTION

1. Fetch/fast-forward `origin/main`, record exact HEAD and require a clean worktree.
2. Require dav1d generated-wrapper repair `b2d53719fdc926aed49f8a3bf90b10279577d33d` to be the tested HEAD or an ancestor of exact current `main` HEAD.
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
