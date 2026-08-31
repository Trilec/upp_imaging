# Active Work

Recovery authority for work currently in flight. After fetching `main`, read this file before using chat history or starting validation. `docs/WINDOWS_ACCEPTANCE.md` is the self-contained validator contract and remains authoritative for gate order and expected totals.

## BASE

- Latest Windows final-acceptance task `IMG-WA-004` used exact SHA `6b91c4a5833be3f9d3facda98869800da40ff204` and stopped compiling `openimageio_io_test` Debug in OpenColorIO apphelpers because `LogWarning`, `LogDebug` and `IsDebugLoggingEnabled` were undeclared.
- OpenColorIO private-logging include repair is published as `3f4c0a44740e91bd845fe078cd400e5d7eef0935`.
- Earlier dav1d generated-wrapper manifest repair remains `b2d53719fdc926aed49f8a3bf90b10279577d33d`.
- Earlier dav1d Windows large-file config repair remains `623b5d30a6755b9ca96a9774e0fb813620d94c10`.
- Earlier libheif C++20 source-boundary repair remains `16aa8472d6e292e8bda18d440aaf26ff46004073`.
- Earlier LibRaw Windows math-constant repair remains `c644ba06a3671e591699d88e8046ae00599e3dac`.
- Earlier DPX/Cineon Blitz repair remains `0523b40b0d1b09798de80760244a27f45b5ccf1b`.
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

- `opencolorio_src/Logging.h`
- `opencolorio_src/README.md`
- `opencolorio_src/opencolorio_src.upp`
- `libheif_src/libheif_src.upp`
- `libraw_src/libraw_src.upp`
- `docs/ACTIVE_WORK.md`

Relevant inspected source/build slice:

- `opencolorio_src/upstream/src/OpenColorIO/Logging.h`
- `opencolorio_src/upstream/src/OpenColorIO/Logging.cpp`
- `opencolorio_src/upstream/src/OpenColorIO/apphelpers/ColorSpaceHelpers.cpp`
- `libheif_src/upstream/libheif/plugins/decoder_libde265.cc`
- `libde265_src/upstream/libde265/de265.h`
- `libraw_src/upstream/src/**` (hton*/ntoh* reference sites)
- validator diagnostics from `IMG-WA-004`, `IMG-DIAG-006`, `IMG-FIX-007`

## STATUS

### Windows acceptance defect 6 — OpenColorIO private logging include resolution

Validator report for task `IMG-WA-004` at exact SHA `6b91c4a5833be3f9d3facda98869800da40ff204`:

- starting worktree clean;
- `openimageio_io_test` Debug progressed beyond the prior dav1d archive-manifest failure;
- compilation then failed in OpenColorIO apphelpers on undeclared `LogWarning`, with additional missing `LogDebug` and `IsDebugLoggingEnabled`;
- representative failures were in `apphelpers/ColorSpaceHelpers.cpp`, `apphelpers/mergeconfigs/SectionMerger.h` and `apphelpers/mergeconfigs/OCIOMYaml.cpp`;
- expected package result was 21/0 but execution was never reached;
- remaining acceptance gates were not run under fail-fast policy;
- no local edits;
- final worktree clean.

Verified source/package facts:

- `opencolorio_src.upp` already compiles upstream `Logging.cpp` and lists upstream `Logging.h`;
- the package also supplies `upstream/src/OpenColorIO` as an include root and defines `OCIO_HEADLESS_ENABLED`;
- upstream `upstream/src/OpenColorIO/Logging.h` unconditionally declares `LogWarning`, `LogDebug` and `IsDebugLoggingEnabled` in `OCIO_NAMESPACE`; these declarations are not removed by headless mode;
- upstream apphelpers include the private API by the generic quoted name `"Logging.h"` rather than by a path-qualified include;
- therefore the observed undeclared identifiers are inconsistent with the intended upstream private header being the header visible to those translation units.

The exact alternate header identity selected by the Windows compiler was not captured in the validator report, so do not claim a specific competing header without new evidence. The integration defect is bounded to making OpenColorIO's intended private logging header resolution deterministic inside the U++ aggregate include graph.

Repair `3f4c0a44740e91bd845fe078cd400e5d7eef0935` adds repository-owned package-root `opencolorio_src/Logging.h`, which forwards directly to `upstream/src/OpenColorIO/Logging.h`. The package already exposes its root as an include path, so apphelper `"Logging.h"` lookup now has a package-owned deterministic target before relying on a generic cross-package name. The package README records this integration boundary.

The repair does **not** modify vendored OpenColorIO 2.5.2 source, the OCIO pin, `Logging.cpp`, dependencies, public API, tests, headless policy, CPU/GPU policy, SIMD policy or source selection.

### Windows acceptance defect 7 — OpenColorIO quote-resolution and HEIF/libde265 link boundary

The earlier package-root forwarding `Logging.h` from `3f4c0a4` did not control resolution because the aggregate U++ include graph searched `libheif_src/upstream/libheif` before OpenColorIO's `-Iupstream/src/OpenColorIO`, so Clang resolved `"Logging.h"` to LibHeif's header (verifier IMG-DIAG-006). `IMG-FIX-007` replaced the forwarder with a package-level `-iquoteupstream/src/OpenColorIO` in `opencolorio_src.upp`, deleted the obsolete forwarder and documented the mechanism. `-H` now resolves `"Logging.h"` to `opencolorio_src/upstream/src/OpenColorIO/Logging.h` with no case warning.

With OpenColorIO compiling, the next Debug failure was a Windows link boundary in `openimageio_io_test`:

- `libheif_src` consumes `upstream/libheif/plugins/decoder_libde265.cc`, but `libheif_src.upp` did not define `LIBDE265_STATIC_BUILD=1`. Pinned `libde265/de265.h` declares `LIBDE265_API` as `dllimport` unless `LIBDE265_STATIC_BUILD` is defined for the consuming TU, so the linker saw `__imp_de265_*` references even though `libde265_src` defines the real symbols. Repair adds `-DLIBDE265_STATIC_BUILD=1` to `libheif_src.upp` (no `LIBDE265_EXPORTS` added).
- The `htons`/`ntohs`/`htonl`/`ntohl` references originate in `libraw_src`; on Windows these require Winsock. Repair adds `library(WIN32) ws2_32;` to `libraw_src.upp`, the package that owns the references (no Winsock placed on OpenImageIO/ImagingIO/the test).

Both repairs are bounded to the owning package manifests; no upstream source/manifest/dependency-architecture change was made.

### Earlier acceptance defects — preserved checkpoints

1. **DPX/Cineon Blitz collision** — acceptance at `19989324cb411fe46d229a0d1fa7cdd51ee7e69f` exposed an `InStream` collision after Blitz combined upstream translation units. Repair `0523b40b0d1b09798de80760244a27f45b5ccf1b` marks the importing package `noblitz`.
2. **LibRaw Windows math constants** — acceptance at `045bb30f5a247b79a82602cd181aabce447eca15` failed on undeclared `M_PI` / `M_SQRT1_2`. Repair `c644ba06a3671e591699d88e8046ae00599e3dac` supplies `_USE_MATH_DEFINES` at the package boundary.
3. **libheif C++20 option ordering** — `IMG-WA-001` at `1847010c4f6745f5440ff28f4796d83781b62ece` failed because U++'s C++17 option followed package-wide C++20. Repair `16aa8472d6e292e8bda18d440aaf26ff46004073` applies C++20 at the imported-source boundary.
4. **dav1d Windows large-file config** — `IMG-WA-002` at `dfe34e2f1ec2c65b33cd982eedb633ff02934694` failed because generated config aliased existing `fseeko`/`ftello`. Repair `623b5d30a6755b9ca96a9774e0fb813620d94c10` follows dav1d's feature-tested `_FILE_OFFSET_BITS=64` branch.
5. **dav1d generated-wrapper wildcard** — `IMG-WA-003` at `3e33ec9fc39f504ec674edee4f9dbbabdbf3bb2b` reached archive creation but `llvm-ar` received literal `generated_*.c.o`. Repair `b2d53719fdc926aed49f8a3bf90b10279577d33d` explicitly enumerates the fixed 26 wrapper files.

Later validator progress past an earlier failure is evidence that the build moved beyond that failure point; it is not by itself full runtime acceptance of the affected subsystem.

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

- `50544b8` — isolate OpenColorIO private include resolution via `-iquoteupstream/src/OpenColorIO`; removes the obsolete package-root forwarding `Logging.h`.
- `4e3a9fa0a1ba8eebe19a84a738091ee4c53a2e06` — validator base for the link-boundary task; also the upstream base checked for dependency advancement.
- `3f4c0a44740e91bd845fe078cd400e5d7eef0935` — earlier pin of OpenColorIO apphelper `"Logging.h"` (superseded by `50544b8` for resolution).
- `6b91c4a5833be3f9d3facda98869800da40ff204` — validator base for `IMG-WA-004`; failed compiling OCIO apphelpers on missing logging declarations.
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

### REPORTED — task `IMG-WA-004` at `6b91c4a...`

- clean starting and final worktree;
- `openimageio_io_test` Debug failed compiling OCIO apphelpers on missing logging identifiers;
- expected package result was 21/0 but execution was never reached;
- no local edits;
- remaining gates not run.

### VERIFIED — source/build review

- the intended upstream private `Logging.h` declares every missing identifier from the report;
- the declarations are not headless-gated;
- upstream apphelpers request that header by the generic quoted name `"Logging.h"`;
- package-root forwarding is a bounded U++ integration repair that leaves vendored source and source ownership intact;
- source repair is published at `3f4c0a44740e91bd845fe078cd400e5d7eef0935`.

### PLATFORM VALIDATION — task `IMG-FIX-008` base `4e3a9fa...`

- `openimageio_io_test` Debug: build clean, link clean, **21/0** (Gate 1 green).
- OpenColorIO `-H` on the exact `ColorSpaceHelpers.cpp` TU: `"Logging.h"` resolves to `opencolorio_src/upstream/src/OpenColorIO/Logging.h`; case-mismatch warning absent; LibHeif `Logging.h` no longer wins.
- Link diagnosis verified: `de265.cc` object `upstream_libde265_de265.cc.o` defines `T de265_get_version`, `T de265_init`, `T de265_new_decoder`, etc.; unresolved references were the consuming-TU `dllimport` spelling; `htons`/`ntohs`/`htonl`/`ntohl` owned entirely by `libraw_src`.
- Subsequent Gate 2 Debug target `imaging_io_test`: summary `79/0` but process exits with access violation `0xC0000005` (reproducible). This is a separate runtime defect, not a compile/link failure.

### PLATFORM VALIDATION PENDING

- remaining still-image Debug matrix (stopped at `imaging_io_test` abnormal exit);
- full Release matrix;
- `plugin_exr_test` 22/0 Debug/Release;
- complete FFmpeg Debug/Release matrix and repeatability;
- supplementary real-file evidence separately pending/unavailable as applicable.

## NEXT ACTION

1. Fetch/fast-forward `origin/main`, record exact HEAD and require a clean worktree.
2. Re-run `openimageio_io_test` Debug first; require 21/0 (Gate 1 already green on the current published link-fix head).
3. Diagnose `imaging_io_test` Debug exit-time access violation `0xC0000005` (summary 79/0 but abnormal shutdown), which currently stops the still-image Debug lane.
4. After that defect is repaired, continue the remaining still-image Debug targets in documented order; then Release, `plugin_exr_test`, FFmpeg and repeatability.
5. Any substantive source-manifest, dependency, API, test, feature/component, format-policy or multi-file defect returns to the supervisor.
6. After every supervisor repair, publish a coherent checkpoint and resume validation from the new exact SHA.

## WORKING RHYTHM

- Remote GitHub `main` is authoritative; refresh before every edit/publish.
- Diagnose the first root failure, make the smallest coherent subsystem repair, and do not chase cascaded errors.
- Preserve pinned upstream source when the defect belongs to U++ integration/build boundaries.
- Keep VERIFIED, REPORTED, INFERRED and UNKNOWN evidence distinct.
- Separate source review from Windows/runtime acceptance.
