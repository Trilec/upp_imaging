# Windows Acceptance

## Version 1.0 Windows release — 3 October 2026

The current release record is [V1_RELEASE.md](V1_RELEASE.md). It supersedes
older pending-publication statements below. The version 1 checkpoint adds
explicit Open types, eight image exports, in-app Help, and ImagingPluginDemo
with compiled generated C++. The integrated Debug suite passed 51 targets /
1,527 checks / 51 exits at 0. Release builds and artifact identities are recorded
there separately from Debug acceptance. Security scope remains trusted stable
local inputs with enforced limits; Linux/macOS, sanitizer/fuzz and hostile-file
isolation remain unvalidated or deferred. Workbench manually accepted by Curt;
that earlier acceptance does not establish a new executable hash.

## Historical records

Authoritative acceptance set after the repository structure migration. The manifest
`tests/acceptance.txt` contains 51 retained deterministic test packages (the original 49 plus raster/video integration);
`tools/validate.ps1` supports Debug and Release. Current delivery validation uses Debug only, as requested by Curt on 1 October 2026. Minimum
per-target check counts are maintained in `tests/expected_counts.txt`; the
table below is historical evidence from the structure migration. The runner
rejects invalid configurations, stale executables after failed builds,
missing/duplicate/malformed summaries, reduced counts, failed checks,
nonzero exits and timeouts.

## Final Windows Debug acceptance — 2 October 2026

All 51 retained targets passed **1,512 checks, zero failures and 51 exits at 0**
through `tools/validate.ps1 -Configuration debug`. The preserved byte-identical
ledger is `build/windows-x64/release/acceptance-debug-finish-20261002-results.txt`,
SHA-256 `0e63e53fb618d1d3d09fe09496c719ba0b2143f8465b6772894eb254b8c244c0`.
Its header records pre-commit HEAD `ed15a3aaf855e6100df87bda2701dc8555b91905`;
the compiled worktree includes the JPEG/input-policy/TIFF/OCIO source committed
with this completion record. Production source did not change during the
successful integrated run. The final manifest totals exactly 1,512 checks.
No rerun is needed merely because those changes are subsequently committed.

This was incremental Debug acceptance, not a clean Release rebuild. A first
attempt stopped at the first target's link, before a test ran; the Windows
LoadImage macro collision was repaired before the complete successful run.
The actual app is rebuilt separately. See DELIVERY_COMPLETION_20261002.md.
Release publication remains PARTIAL; non-Windows/sanitizer/fuzz execution is
unvalidated. Workbench manually accepted by Curt; no exact new artifact identity
is inferred from that earlier human acceptance.

## Historical acceptance records

## Focused security checkpoint — 2 October 2026

See [SECURITY_VALIDATION_20261002.md](SECURITY_VALIDATION_20261002.md): 293 checks across eight Debug targets, zero failures and eight exits at 0. The actual app rebuilt after the PNG refresh. The manifest minimum is now 1,493; no full run at that count is claimed. Historical full-suite ledgers below retain their exact counts and source scope.

## Current delivery Debug checkpoint — 1 October 2026

All 51 retained targets passed: 1,482 checks, zero failures, 51 exits at 0.
The suite used incremental CLANGx64 Debug builds through the existing runner;
it is not a clean rebuild or fresh Release evidence. The immutable ledger is
`build/windows-x64/release/acceptance-debug-delivery-20261001-results.txt`,
SHA-256 `c55be65e0d8a520d81060906bd96c390d2cfcfd5efab807b3b72ca573d9678bd`.
This is an LF-normalized preserved copy. The original CRLF ledger at
validation/results.txt has SHA-256 fe3eb523cc9ee235d682481cb756d521db6f98d12fefa76f2f49709d66edd083; counts and lines are
unchanged. Its source header is pre-commit d944562640194668377f8832af0c762f34aac3dc;
the run compiled the image/video/Workbench worktree source committed with
this delivery. No compiled source changed during the suite. The minimum at that earlier checkpoint
was exactly 1,482; the current manifest is 1,512. The new raster test adds 57 checks,
the video test adds 22 and combined Workbench adds 15 over the prior 151.
The outer Klick request timed out while the native driver continued; the
complete saved per-target records establish the results without restarting.
Debug Workbench artifact identity and focused video evidence are in
[IMAGING_VIDEO_VALIDATION.md](IMAGING_VIDEO_VALIDATION.md). Workbench manually
accepted by Curt; that historical acceptance does not identify this new hash.
Release/security publication and non-Windows execution remain separate gates.

To repeat this block deliberately, use `tools/validate.ps1 -Configuration debug`
or build and run `imaging_debug_acceptance`, the fixed Windows driver with
process-tree containment and a 550-second block deadline. It forwards no user
commands and uses process-scoped PowerShell execution policy for this script.

## Reproduce

Configure the active family-nest assembly using `GitHubOut.var.example` and
[BUILD_AND_RUN.md](BUILD_AND_RUN.md), including its absolute compiler-output path.
Then run from the repository:

```powershell
powershell -ExecutionPolicy Bypass -File tools/validate.ps1
```

The accepted environment is Windows U++ `E:/upp-18468/umk.exe`, `CLANGx64`,
Debug (`-H8`) and Release (`-rH8`). Record the checked-out commit before validation.
Use `-Rebuild` to clean the first target and its dependencies in each configuration.
The runner now writes test executables/logs to
`build/windows-x64/validation/debug/` and `build/windows-x64/validation/release/`,
with `results.txt` in their parent. U++ intermediate output is controlled
separately by the active assembly: `build/windows-x64/umk/`.
The runner records source SHA, method, configuration, per-target counts,
exit codes and log paths in `results.txt`. An increased check count is
accepted and should be reviewed and reflected in the minimum-count manifest
at a coherent checkpoint.

## Clean IMG-REL-002 Windows run — FFmpeg 9.0.2 source checkpoint

The complete `build/windows-x64/umk/` intermediate root was verified to
contain no tracked files or reparse points and emptied before this run.
`tools/validate.ps1 -Umk E:/upp-18468/umk.exe -Rebuild` then completed:
49 Debug targets and 49 Release targets, 1,373 checks per configuration,
2,746 total checks and 98 process exits at 0. This is historical evidence
for the FFmpeg 9.0.2 source checkpoint. The later Workbench-format test
raises the current minimum to 1,388 checks per configuration after the
subimage-bound regression, and the
subsequent OpenEXR/zlib/input-limit changes have focused checks pending a
new clean integrated release-candidate run. The full historical ledger is
`build/windows-x64/release/acceptance-26c4944-clean-results.txt`
(SHA-256 `5244B2EC1B15ACDE06E9EAA1FC588F31CCBECEAEFFE49FF462AF82ED4E6A684D`).

Its `source=` header says `b80ddbfa1ae35dbce797b111d9b53620dac7509f`:
the runner read HEAD before the already-applied FFmpeg 9.0.2 worktree
change was committed as `26c494498e6d975c77c552d002ce4e65a06c1960`.
No compiled source, generated header or test assertion changed after the
runner started; the later edits before that commit were documentation.
Both configurations passed the exact 9.0.2 `ffmpeg_avutil_test` assertion.
The ledger therefore validates the compiled source at the published
FFmpeg checkpoint despite its pre-commit header. No repeat suite was run
solely to update that label. This is Windows evidence only; security and
other-platform gates are tracked in [ACTIVE_WORK.md](ACTIVE_WORK.md).

The table below remains the earlier structure-migration evidence.

## Recorded migration evidence — 17bdbb3e326a07e71f159cb034c34534fb6f6bff

All test C++ entry points and headers were timestamp-touched before the final run,
forcing their compilation against the new nests rather than reusing old test objects.
The final run completed **98 builds/runs, 2734 passed checks, zero failed checks**.
Every process exited normally with code 0, including the Debug heap-audit shutdown.
Earlier clean Debug and Release dependency builds and staged representative builds
are recorded in `STRUCTURE_MIGRATION.md`.

Historical local evidence paths (not current output destinations):
`out/structure-authoritative.log`, `out/validation/results.txt`,
per-target build/run/stderr logs, and `out/structure-app-builds-final.log`.

| Test package | Debug passed/failed | Release passed/failed |
| --- | --- | --- |
| dpx_cineon_oiio_test | 19/0 | 19/0 |
| expat_test | 3/0 | 3/0 |
| ffmpeg_avcodec_test | 12/0 | 12/0 |
| ffmpeg_avformat_test | 14/0 | 14/0 |
| ffmpeg_avutil_test | 13/0 | 13/0 |
| ffmpeg_first_frame_test | 27/0 | 27/0 |
| ffmpeg_headers_test | 8/0 | 8/0 |
| ffmpeg_swscale_test | 13/0 | 13/0 |
| hdr_dpx_imagingio_test | 38/0 | 38/0 |
| hdr_oiio_test | 12/0 | 12/0 |
| heif_imagingio_test | 10/0 | 10/0 |
| heif_oiio_test | 11/0 | 11/0 |
| imaging_analysis_test | 41/0 | 41/0 |
| imaging_color_ocio_test | 15/0 | 15/0 |
| imaging_color_test | 69/0 | 69/0 |
| imaging_core_test | 52/0 | 52/0 |
| imaging_diagnostics_test | 33/0 | 33/0 |
| imaging_histogram_test | 155/0 | 155/0 |
| imaging_io_oiio_test | 21/0 | 21/0 |
| imaging_io_test | 89/0 | 89/0 |
| imaging_preview_coalescing_test | 13/0 | 13/0 |
| imaging_roundtrip_viewer_ocio_smoke_test | 39/0 | 39/0 |
| imaging_test | 6/0 | 6/0 |
| imaging_tone_conversion_test | 124/0 | 124/0 |
| imaging_view_transform_test | 111/0 | 111/0 |
| imaging_workbench_ocio_test | 136/0 | 136/0 |
| imath_test | 4/0 | 4/0 |
| jpegxl_imagingio_test | 50/0 | 50/0 |
| jpegxl_oiio_test | 10/0 | 10/0 |
| libdeflate_test | 2/0 | 2/0 |
| libjpeg_turbo_test | 5/0 | 5/0 |
| libpng_roundtrip_test | 3/0 | 3/0 |
| libpng_src_roundtrip_test | 3/0 | 3/0 |
| libtiff_test | 10/0 | 10/0 |
| minizip_ng_test | 9/0 | 9/0 |
| opencolorio_test | 18/0 | 18/0 |
| openexr_core_rgba_zip_test | 6/0 | 6/0 |
| openexr_test | 11/0 | 11/0 |
| openimageio_io_test | 21/0 | 21/0 |
| plugin_exr_test | 22/0 | 22/0 |
| pystring_test | 8/0 | 8/0 |
| raw_imagingio_test | 10/0 | 10/0 |
| raw_oiio_test | 9/0 | 9/0 |
| tiff_imagingio_test | 29/0 | 29/0 |
| tiff_oiio_test | 13/0 | 13/0 |
| webp_imagingio_test | 21/0 | 21/0 |
| webp_oiio_test | 13/0 | 13/0 |
| yaml_cpp_test | 4/0 | 4/0 |
| zlib_test | 2/0 | 2/0 |

## Production and source integrity

- All retained production packages are covered by the acceptance/app dependency
  graph, plus a separate public `openjph` API compile/run in both configurations.
- `ImagingWorkbench` and `imaging_workbench_bench` build in Debug and Release.
- The manual benchmark completed `--quick` in Release with exit 0 and coordinate
  checks passing. Its Debug smoke exceeded the 120-second observation limit and
  was stopped; Debug benchmark timing is not an acceptance gate or performance claim.
- All eight relocated submodules retain their existing commits and are clean.
  FFmpeg remains `n9.0.1`, commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- FFmpeg parity: 238 sources, 609 scanned files, 260 referenced identifiers,
  397 generated definitions, zero missing definitions.
- All 2,593 tracked vendored upstream files match the starting commit after Git
  line-ending normalization. Generated downstream overlays are outside upstream.
- Both OCIO generators pass `--check`. Package identities are case-insensitively
  unique; the acceptance manifest exactly matches the retained test packages.

## Scope

This is Windows validation. Existing POSIX runtime coverage and optional external
camera RAW, HEIF and animated-WebP fixtures remain outside this deterministic set.
Prepared OCIO processor reuse and EXR chunk/tile preview performance remain deferred
as described in `HARDENING_REVIEW.md`. Removed probes are classified individually
in `STRUCTURE_MIGRATION.md`; do not recreate them as acceptance requirements.
