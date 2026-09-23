# Active Work

Recovery authority for the published hardening checkpoint. See
`docs/HARDENING_REVIEW.md` for the ten-area review and deferred work;
`docs/WINDOWS_ACCEPTANCE.md` remains the acceptance gate-order authority.

## BASE

- START SHA: `f67e9a53b70be84cb5dd40586f59e1009c1724ab`, fetched current main; no intervening changes.
- U++ `E:/upp-18468/umk.exe`, local `GitHubOut`, `CLANGx64`, Debug `DEBUG_FULL` Noblitz and Release.
- Installed U++ and pinned upstream sources are unchanged.

## STATUS

PASS — HARDENING COMPLETE, PERFORMANCE ITEMS DEFERRED

- Save transactions use exclusively reserved process-ID/UUID names and same-directory native promotion without staging the destination away.
- Complete-payload readability verification uses about 1 MiB scratch or one wide row; no universal source-bit comparison is imposed.
- ImageData rejects mismatched specification/buffer sample types; U++ buffer moves transfer ownership without deep copying.
- Packed Float32 RGB/RGBA avoids the float-row gather/scatter path, while excluding alpha from OCIO processing and retaining transactional output.
- Fixed the unused-backend lifecycle defect discovered during this pass. The unchanged umbrella test initially printed 6/0 then crashed during heap-leak reporting (exit -1073741819). libheif's implicit static decoder registration is now paired with ordered fallback cleanup in the same translation unit. Normal OIIO heif_init/heif_deinit remains a single balanced pair, with no test prewarm or extra production thread.
- Pinned OIIO error-map debt and joined-worker workarounds are documented for reassessment on upgrade. Production reader fallback semantics remain unchanged.
- FFmpeg parity scans repository-local materializers; production config/source and bounded scalar scope are unchanged.
- ImagingIO documentation now matches HEIF extensions/layouts and TIFF multichannel refusal.

## PUBLISHED CHECKPOINTS

- `bac083e` — HEIF unused-library shutdown and OIIO dependency-debt documentation.
- `b43594e` — Core sample-storage invariant and move regression checks.
- `2415a54` — save transactions and bounded readability verification.
- `e56d3ba` — packed Float32 colour processing and alpha-preservation regressions.
- `af4914d` — FFmpeg parity coverage and pin-change maintenance.
- This documentation closure — complete findings, updated expected totals and consolidated evidence.

## VALIDATION

The shared lifecycle change justified repeating the still-image dependency boundary.
Every following build/run completed with normal process exit 0; expected negative
codec messages in stderr did not change check totals or exit status.

Debug and Release, each in required still-image order:

| Gate | Passed/failed in each configuration |
| --- | --- |
| openimageio_io_test | 21/0 |
| imaging_io_test | 89/0 |
| jpegxl_prereq_test | 9/0 |
| jpegxl_oiio_test | 10/0 |
| jpegxl_imagingio_test | 50/0 |
| hdr_oiio_test | 12/0 |
| dpx_cineon_oiio_test | 19/0 |
| hdr_dpx_imagingio_test | 38/0 |
| raw_oiio_test | 9/0 |
| raw_imagingio_test | 10/0 |
| webp_oiio_test | 13/0 |
| webp_imagingio_test | 21/0 |
| heif_oiio_test | 11/0 |
| heif_imagingio_test | 10/0 |
| tiff_oiio_test | 13/0 |
| tiff_imagingio_test | 29/0 |

Additional affected Debug and Release gates: Core 52/0, Color 69/0,
independent OCIO 15/0, Analysis 41/0, Diagnostics 33/0, umbrella 6/0,
and unchanged EXR preview 22/0. HEIF's direct gate repeats initialization sixteen times.

FFmpeg Debug, all six in order: headers 8/0, avutil 13/0, avcodec 12/0,
avformat 14/0, swscale 13/0, first-frame 27/0. Parity audit: 238 sources,
609 scanned files, 260 referenced identifiers, 397 generated definitions,
zero missing. Compatibility helper body is token-equivalent to the pin.
FFmpeg Release was not rerun for its test/documentation-only change; the
previous accepted Release evidence remains in the starting checkpoint.

Three additional Debug runs each of imaging_io_test (89/0) and imaging_test
(6/0), all exit 0. Consolidated final validation: **58 runs, 1,576 passed
checks, zero failed checks**. Additionally, two processes each performed twelve
saves to the same destination; both exited 0 and left no transaction residue.

Logs/binaries are under ignored `out/hardening-*`; no diagnostic prewarm or
probe package is tracked. All eight submodules are clean at their existing pins,
including FFmpeg `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.

## LIMITS AND NEXT ACTION

- Prepared OCIO processor/config reuse and EXR chunk/tile preview conversion remain documented performance follow-ups.
- Private float/double sample helpers remain separate to preserve their precision/rounding policies.
- POSIX transaction branches were reviewed but not runtime-tested on this Windows host.
- Optional real-camera RAW, 8/10-bit HEIF and animated-WebP fixture evidence remains pending, unchanged from the accepted baseline.
- No new formats/codecs, FFmpeg SIMD/hardware/audio/container/seek scope, media framework or GUI work was started.
- Publish this coherent checkpoint after diff checks; fetch before push, no force, and verify HEAD equals origin/main and the published diff.
