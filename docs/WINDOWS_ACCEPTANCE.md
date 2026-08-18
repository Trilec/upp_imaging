# Windows Acceptance

Self-contained validator contract for the current bounded `upp_imaging` generation.

## Validator role

The validator confirms that the exact published repository state builds and runs correctly under the established Windows U++ CLANGx64 environment. Architecture, source ownership, dependencies, public APIs, tests and feature policy are supervisor-owned.

Before any build:

1. fetch and fast-forward `origin/main`;
2. record `git rev-parse HEAD` and use that exact SHA in the report;
3. require a clean `git status --short`;
4. use the repository's established U++ CLANGx64 Debug/Release configuration; do not invent or change feature flags to make a target pass.

If HEAD does not match the SHA supplied with the validator task, stop and report both SHAs.

## Phase A — still-image deterministic matrix

Run Debug first, in this order. Stop on the first substantive compile, link or runtime failure.

1. `openimageio_io_test` — require 21/0
2. `imaging_io_test` — require 79/0
3. `jpegxl_prereq_test` — require 9/0
4. `jpegxl_oiio_test` — require 10/0
5. `jpegxl_imagingio_test` — require 50/0
6. `hdr_oiio_test` — require 12/0
7. `dpx_cineon_oiio_test` — require 19/0
8. `hdr_dpx_imagingio_test` — require 38/0
9. `raw_oiio_test` — require 9/0
10. `raw_imagingio_test` — require 10/0
11. `webp_oiio_test` — require 13/0
12. `webp_imagingio_test` — require 21/0
13. `heif_oiio_test` — require 11/0
14. `heif_imagingio_test` — require 10/0
15. `tiff_oiio_test` — require 13/0
16. `tiff_imagingio_test` — require 29/0

Only after all sixteen Debug targets are green, run the same sixteen targets in Release with the same totals.

These are the repository-owned deterministic acceptance gates for HDR/RGBE, DPX/Cineon, camera RAW routing, WebP, decode-only HEIF/AVIF, TIFF, JPEG XL and the shared OpenImageIO/ImagingIO boundary.

## Phase B — EXR raster integration

Run `plugin_exr_test` in Debug and Release. Require 22/0 in each configuration.

## Phase C — FFmpeg first slice

The repository submodule/source pin must resolve to signed FFmpeg `n9.0.1`, exact upstream commit:

`bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`

Do not validate FFmpeg against another upstream revision.

Run Debug first, in this order:

1. `ffmpeg_headers_test` — require 8/0
2. `ffmpeg_avutil_test` — require 13/0
3. `ffmpeg_avcodec_test` — require 12/0
4. `ffmpeg_avformat_test` — require 14/0
5. `ffmpeg_swscale_test` — require 13/0
6. `ffmpeg_first_frame_test` — require 27/0

Only after the complete Debug lane is green, run all six in Release with the same totals.

Then run `ffmpeg_first_frame_test` five additional times in Debug and five additional times in Release. Every run must remain 27/0 and exit cleanly without shutdown/cleanup failure.

The accepted first slice remains scalar LGPL decode only. Do not enable threads, network, external codecs, filters, devices, audio resampling, CLI/encoding, external/inline assembly, hardware acceleration, extra codecs or extra containers to make validation pass.

## Supplementary real-file evidence

The repository does not store provenance-reviewed fixtures for:

- positive real-camera RAW/DNG decode;
- real 8/10-bit AVIF and HEIC decode;
- animated-WebP multi-frame rejection.

If suitable fixtures are already available, report those interoperability results separately. If they are not available, report `fixture unavailable / evidence pending`. Do not fabricate substitute fixtures and do not convert their absence into either PASS or FAIL for the deterministic package matrix.

## Failure handling

On the first substantive failure, stop that lane and report:

- exact repository SHA;
- package name;
- Debug or Release;
- compile, link or runtime stage;
- first useful complete error block, unresolved symbol or failed check;
- immediately relevant source/object/file context if the tool reports it;
- whether the worktree was clean before the run;
- whether any local edit was made.

Do not continue through a cascade after the first root failure merely to collect more symptoms.

## Tiny local corrections

A validator may make one genuinely mechanical one-file correction only when the cause is obvious and does not change architecture or behavior, for example a spelling mistake, simple local type/include compile nuisance, or exact search-and-replace correction.

Before making it:

1. preserve and report the original failure;
2. show the exact diff;
3. keep the change to one file;
4. do not commit or push it unless Curt explicitly asks.

Any locally edited run is no longer exact-SHA acceptance. Report the post-edit result separately.

Return immediately to the supervisor for any change involving:

- source manifests or source ownership;
- package dependencies or build architecture;
- public/private APIs;
- test expectations or weakened tests;
- FFmpeg `CONFIG_*`, `HAVE_*`, component or feature enablement;
- format policy;
- more than one file;
- any fix whose correctness is not mechanically obvious.

## Successful report

A complete successful report must state:

- exact validated repository SHA;
- clean starting worktree;
- all sixteen still-image Debug totals and all sixteen Release totals;
- `plugin_exr_test` 22/0 Debug and 22/0 Release;
- exact FFmpeg upstream pin;
- all six FFmpeg Debug and Release totals;
- five-repeat Debug and five-repeat Release first-frame results;
- supplementary fixture results or explicit `unavailable/pending` status;
- final `git status --short`;
- confirmation that no architecture/dependency/API/test/feature-policy changes were made.

The validator's job is to prove the published state. Substantive repair belongs to the supervisor and is published as a new exact SHA before acceptance resumes.
