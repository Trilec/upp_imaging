# Active Work

Recovery authority for work currently in flight. After fetching `main`, read this file before using chat history or starting validation. `docs/WINDOWS_ACCEPTANCE.md` is the self-contained validator contract.

## BASE

- Current repair base was `19989324cb411fe46d229a0d1fa7cdd51ee7e69f` on `main`.
- First Windows final-acceptance attempt at that SHA stopped at the first package, `openimageio_io_test` Debug, during compilation.
- The source repair for that first failure is published as `0523b40b0d1b09798de80760244a27f45b5ccf1b`.
- Current-generation framework implementation is complete: `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics`, `Imaging`.
- Established Windows framework baseline remains: ImagingCore 48/0, ImagingIO 79/0 baseline, ImagingColor 66/0 + independent OCIO 15/0, ImagingAnalysis 41/0, ImagingDiagnostics 33/0, Imaging umbrella 6/0.
- Code-side still-image line remains JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF.
- Shared static OpenImageIO dependency closure remains `5ca436c3ba6265f6431deaf7348332940051686d`.
- `plugin/exr` expanded focused contract remains 22 checks, Windows Debug/Release pending.
- FFmpeg remains a separate media subsystem from ImagingIO/Imaging.

## TASK

Complete final current-generation Windows acceptance. Repair only real current-main failures as coherent root-cause slices, publish each meaningful repair promptly, then resume validation from the exact published SHA.

The current immediate task is to validate the DPX/Cineon Blitz-integration repair, then continue the repository-owned still-image matrix. After that complete `plugin_exr_test`, the bounded FFmpeg first slice and FFmpeg repeatability. Keep supplementary real-file interoperability evidence separate from deterministic repository-owned gates.

## TOUCHED

Current repair slice:

- `openimageio_plugin_dpxcineon/openimageio_plugin_dpxcineon.upp`
- `openimageio_plugin_dpxcineon/README.md`
- `docs/ACTIVE_WORK.md`

Relevant source/dependency slice inspected:

- `OpenImageIO/OpenImageIO.upp`
- `openimageio_plugin_dpxcineon/import.ext`
- `openimageio_plugin_dpxcineon/RegisterDPXCineon.cpp`
- `openimageio_plugins_src/openimageio_plugins_src.upp`
- `openimageio_src/openimageio_src.upp`
- pinned OpenImageIO `src/cineon.imageio/cineoninput.cpp`
- pinned OpenImageIO `src/cineon.imageio/libcineon/CineonStream.h`
- pinned OpenImageIO `src/dpx.imageio/libdpx/DPXStream.h`

## STATUS

### First final-acceptance defect and repair

Gary reported that `openimageio_io_test` Debug at `19989324cb411fe46d229a0d1fa7cdd51ee7e69f` failed before execution because U++ Blitz combined the imported DPX and Cineon plugin sources into one translation unit. The resulting translation unit contained both DPX's global `InStream` and `cineon::InStream`; upstream `cineoninput.cpp` uses `using namespace cineon` and unqualified `InStream`, producing the reported ambiguity and follow-on member/constructor errors.

Static/source review confirmed the mechanism against the exact pinned OpenImageIO 3.1.15.0 source commit `cbe57bc005678ca310835473568121719861734c`:

- `openimageio_plugin_dpxcineon/import.ext` imports both DPX and Cineon source sets into one U++ package;
- DPX `DPXStream.h` declares `InStream` in the global namespace;
- Cineon `CineonStream.h` declares `cineon::InStream`;
- upstream `cineoninput.cpp` uses `using namespace cineon` and unqualified `InStream` as valid separate-translation-unit source;
- the repository already marks strict upstream source packages such as `openimageio_src` and the neutral `openimageio_plugins_src` holder `noblitz`.

Repair `0523b40b0d1b09798de80760244a27f45b5ccf1b` therefore marks the *importing* `openimageio_plugin_dpxcineon` package `noblitz`. This preserves the pinned upstream source unchanged and restores the separate-translation-unit assumption under which those two upstream plugins are authored. The package README records the integration rationale.

The repair does **not** change the source manifest, dependencies, APIs, tests, static registration, codec/format policy, upstream pin or supported functionality.

### Deterministic still-image matrix

Expected Debug and Release totals remain:

- `openimageio_io_test` — 21/0
- `imaging_io_test` — 79/0
- `jpegxl_prereq_test` — 9/0
- `jpegxl_oiio_test` — 10/0
- `jpegxl_imagingio_test` — 50/0
- `hdr_oiio_test` — 12/0
- `dpx_cineon_oiio_test` — 19/0
- `hdr_dpx_imagingio_test` — 38/0
- `raw_oiio_test` — 9/0
- `raw_imagingio_test` — 10/0
- `webp_oiio_test` — 13/0
- `webp_imagingio_test` — 21/0
- `heif_oiio_test` — 11/0
- `heif_imagingio_test` — 10/0
- `tiff_oiio_test` — 13/0
- `tiff_imagingio_test` — 29/0

### `plugin/exr`

- `plugin_exr_test` — 22/0 Debug and 22/0 Release required.

### FFmpeg first slice

Exact signed upstream pin remains FFmpeg `n9.0.1`, commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.

Expected Debug and Release totals:

- `ffmpeg_headers_test` — 8/0
- `ffmpeg_avutil_test` — 13/0
- `ffmpeg_avcodec_test` — 12/0
- `ffmpeg_avformat_test` — 14/0
- `ffmpeg_swscale_test` — 13/0
- `ffmpeg_first_frame_test` — 27/0

After both lanes pass, `ffmpeg_first_frame_test` must pass another five Debug and five Release runs, every run 27/0 with clean shutdown/cleanup.

### Supplementary interoperability boundary

Positive real-camera RAW/DNG decode, real 8/10-bit AVIF/HEIC decode and animated-WebP rejection require provenance-reviewed external fixtures not stored in this repository. Report those separately if suitable fixtures exist; otherwise leave them explicitly unavailable/pending. Never substitute fabricated fixtures or promote deterministic routing/malformed-input checks into real-file evidence.

## PUBLISHED

Most relevant current checkpoints:

- `0523b40b0d1b09798de80760244a27f45b5ccf1b` — prevent DPX/Cineon Blitz namespace collision.
- `19989324cb411fe46d229a0d1fa7cdd51ee7e69f` — point recovery state to Windows acceptance contract; failed validation base.
- `32e9f32666aeee129400e5562ad395ee0a94435f` — add self-contained `docs/WINDOWS_ACCEPTANCE.md`.
- `b2b14c8cea9332ed6f997c5f0edd4825826eb353` — publish exact still-image acceptance matrix.
- `323c3dc29938de404fc3411b87dcaf6c6aea4559` — expanded `plugin/exr` preview contract.
- `5ca436c3ba6265f6431deaf7348332940051686d` — static OpenImageIO plugin dependency closure.
- `a66e1192025032823e93a890e16cc3874034a8a4` — JPEG XL skcms linker repair.
- `d4a6051e909fbe95c8930ad725e2b857e4efc3af` — FFmpeg avformat link-ownership recovery checkpoint.
- `fed4e6800bf2f1aee432f708633d3b6ed15c54e7` — FFmpeg avformat materializer ownership repair.
- `74247ac00dc4da919a89b5f69995b790ba2606f0` — FFmpeg generated source/config parity audit.

## VALIDATION

### REPORTED — Windows final acceptance at failed base

Gary reported for exact SHA `19989324cb411fe46d229a0d1fa7cdd51ee7e69f`:

- starting worktree clean;
- `openimageio_io_test` Debug compile failed before execution;
- first useful error was ambiguous `InStream` in `cineoninput.cpp`, with candidates DPX global `InStream` and `cineon::InStream`;
- remaining still-image, EXR and FFmpeg gates were not run per fail-fast policy;
- no local edits;
- final worktree clean;
- `git diff --check` passed.

This is validator-reported evidence, not a supervisor rerun.

### VERIFIED — source/static review

- exact current `main` base before repair was refreshed as `19989324cb411fe46d229a0d1fa7cdd51ee7e69f`;
- the complete importing U++ package manifest and import list were inspected;
- the exact pinned upstream Cineon input source and both stream headers were inspected;
- the ambiguity is caused by Blitz translation-unit aggregation, not by a missing dependency or feature;
- `noblitz` on the importing package preserves upstream source and is consistent with the repository's existing handling of strict upstream source packages;
- source repair is published at `0523b40b0d1b09798de80760244a27f45b5ccf1b`.

### PLATFORM VALIDATION PENDING

- `openimageio_io_test` Debug 21/0 on the repaired published SHA;
- remaining still-image Debug matrix, then complete Release matrix;
- `plugin_exr_test` 22/0 Debug/Release;
- complete FFmpeg Debug/Release matrix and repeatability;
- supplementary real-file evidence remains separately pending unless suitable external fixtures are available.

## NEXT ACTION

1. Fetch/fast-forward `origin/main`, record exact HEAD and require a clean worktree.
2. Require repair commit `0523b40b0d1b09798de80760244a27f45b5ccf1b` to be the tested HEAD or an ancestor of a newer explicitly accepted `main` HEAD.
3. Read `docs/WINDOWS_ACCEPTANCE.md`.
4. Rebuild/run `openimageio_io_test` Debug first; require 21/0.
5. If that package is green, continue the remaining still-image Debug targets in the documented order from `imaging_io_test` onward. Stop at the first substantive defect.
6. Only after the complete still-image Debug lane is green, run the complete still-image Release matrix with the same totals.
7. Then run `plugin_exr_test` Debug/Release 22/0.
8. Then validate FFmpeg upstream pin and run the complete FFmpeg Debug/Release/repeatability contract.
9. Any substantive source-manifest, dependency, API, test, feature/component or multi-file defect returns to the supervisor. Do not solve it by broadening functionality or weakening tests.
10. After every supervisor repair, publish the coherent checkpoint promptly and resume validation from the new published SHA.

## WORKING RHYTHM

- Remote GitHub `main` is authoritative; refresh before every edit/publish.
- Diagnose the first root failure, make the smallest coherent subsystem repair, and do not chase cascaded errors.
- Publish meaningful recovery checkpoints promptly so outages cannot strand work.
- Keep upstream/vendor modifications out of the tree when the defect belongs to U++ integration/build boundaries.
- Keep VERIFIED, REPORTED, INFERRED and UNKNOWN evidence distinct.
- Separate source review from Windows/runtime acceptance.
- On recovery: fetch `main`, read this file, inspect the current referenced commit/files, then continue from NEXT ACTION.
