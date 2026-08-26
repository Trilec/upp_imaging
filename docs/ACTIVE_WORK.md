# Active Work

Recovery authority for work currently in flight. After fetching `main`, read this file before using chat history or starting validation. `docs/WINDOWS_ACCEPTANCE.md` is the self-contained validator contract and remains authoritative for gate order and expected totals.

## BASE

- The latest Windows final-acceptance run used exact SHA `1847010c4f6745f5440ff28f4796d83781b62ece` and stopped compiling `openimageio_io_test` Debug on libheif C++20 requirements being compiled effectively as C++17.
- The libheif C++20 source-boundary repair is published as `16aa8472d6e292e8bda18d440aaf26ff46004073`.
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

- `libheif_src/libheif_src.upp`
- `libheif_src/README.md`
- `docs/ACTIVE_WORK.md`

Relevant inspected dependency/source/build slice:

- `libheif_src/import.ext`
- pinned libheif 1.23.1 `CMakeLists.txt`
- pinned libheif 1.23.1 `libheif/context.cc`
- pinned libheif 1.23.1 `libheif/codecs/avc_boxes.cc`
- pinned libheif 1.23.1 `libheif/nclx.h`
- current U++ CLANGx64 build-method definition
- current U++ `MakeBuild::CreateBuilder` option loading
- current U++ `GccBuilder::BuildPackage` compile-command ordering
- U++ package file-option contract for per-source compiler options

## STATUS

### Windows acceptance defect 3 — libheif C++20 option ordering

Validator report for task `IMG-WA-001` at exact SHA `1847010c4f6745f5440ff28f4796d83781b62ece`:

- starting worktree clean;
- `openimageio_io_test` Debug failed during compilation before execution;
- first useful errors were missing `std::map::contains`, missing `std::set::contains`, and failed `nclx_profile` comparison in pinned libheif sources;
- remaining acceptance gates were not run under fail-fast policy;
- no local edits;
- final worktree clean;
- `git diff --check` passed.

Source/build review confirmed the mechanism:

- pinned libheif 1.23.1 commit `2c4bbb54c2738d4a5efbbe3e5fa1d5d76bb88eb0` explicitly requires C++20 in upstream `CMakeLists.txt`;
- `libheif_src/libheif_src.upp` attempted to request `-std=c++20` as a package-wide compiler option;
- the U++ CLANGx64 build method supplies `COMMON_CPP_OPTIONS = "-std=c++17"`;
- U++ `GccBuilder::BuildPackage` adds package-wide `options` to the base compiler command first, then appends `COMMON_CPP_OPTIONS` for C++ translation units, so the previous command ordering effectively ended `-std=c++20 ... -std=c++17` and C++17 won;
- U++ file-specific options are appended later still, after `COMMON_CPP_OPTIONS`;
- imported sources inherit the file options attached to their `import.ext` package entry, so that boundary can supply the required language mode after the build-method default without altering the global CLANGx64 method.

Repair `16aa8472d6e292e8bda18d440aaf26ff46004073` therefore removes `-std=c++20` from package-wide `options` and attaches it to the `import.ext` file entry for CLANG/GCC. The imported libheif `.cc` sources now receive C++20 at the per-source boundary after U++'s C++17 method default. The rest of the application remains on the established U++ language mode.

The repair does **not** change the libheif upstream pin, upstream source files, imported source selection, dependencies, APIs, tests, format policy, codec enablement or application-wide build method.

### Windows acceptance defect 2 — LibRaw math constants

The run at `045bb30f5a247b79a82602cd181aabce447eca15` failed compiling pinned LibRaw on undeclared `M_PI` / `M_SQRT1_2`. Source review showed `_USE_MATH_DEFINES` was defined only after LibRaw's first `<math.h>` include. Repair `c644ba06a3671e591699d88e8046ae00599e3dac` supplies `-D_USE_MATH_DEFINES=1` at the U++ LibRaw package boundary while leaving pinned upstream source unchanged. The later run at `1847010...` progressed beyond this failure to libheif.

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

- `16aa8472d6e292e8bda18d440aaf26ff46004073` — apply libheif C++20 at imported-source boundary.
- `1847010c4f6745f5440ff28f4796d83781b62ece` — validator base for `IMG-WA-001`; failed on effective C++17 libheif compilation.
- `c644ba06a3671e591699d88e8046ae00599e3dac` — enable LibRaw Windows math constants at the package boundary.
- `0523b40b0d1b09798de80760244a27f45b5ccf1b` — prevent DPX/Cineon Blitz namespace collision.
- `32e9f32666aeee129400e5562ad395ee0a94435f` — self-contained Windows acceptance contract.
- `b2b14c8cea9332ed6f997c5f0edd4825826eb353` — exact still-image acceptance matrix.
- `323c3dc29938de404fc3411b87dcaf6c6aea4559` — expanded `plugin/exr` preview contract.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.

## VALIDATION

### REPORTED — task `IMG-WA-001` at `1847010...`

- clean starting and final worktree;
- `openimageio_io_test` Debug failed compiling pinned libheif on C++20 library/language features while effective mode remained C++17;
- expected package result was 21/0 but execution was never reached;
- no local edits;
- remaining gates not run;
- `git diff --check` passed.

### VERIFIED — source/build review

- libheif 1.23.1 requires C++20 upstream;
- current U++ CLANGx64 method defaults C++ sources to C++17;
- U++ builder ordering places `COMMON_CPP_OPTIONS` after package-wide options;
- U++ builder ordering places file-specific source options after `COMMON_CPP_OPTIONS`;
- `import.ext` source expansion inherits the file entry's per-source options;
- moving C++20 to that boundary is the smallest coherent integration repair and does not require changing U++ globally or editing pinned libheif source;
- repair is published at `16aa8472d6e292e8bda18d440aaf26ff46004073`.

### PLATFORM VALIDATION PENDING

- `openimageio_io_test` Debug 21/0 on current repaired `main`;
- remaining still-image Debug matrix, then full Release matrix;
- `plugin_exr_test` 22/0 Debug/Release;
- complete FFmpeg Debug/Release matrix and repeatability;
- supplementary real-file evidence separately pending/unavailable as applicable.

## NEXT ACTION

1. Fetch/fast-forward `origin/main`, record exact HEAD and require a clean worktree.
2. Require libheif repair `16aa8472d6e292e8bda18d440aaf26ff46004073` to be the tested HEAD or an ancestor of the exact current `main` HEAD.
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
