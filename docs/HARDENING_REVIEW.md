# Final hardening review

This records the preceding hardening checkpoint. The current layout and
acceptance set are documented in STRUCTURE_MIGRATION.md and
WINDOWS_ACCEPTANCE.md; the migration supersedes this report's package/test list.

Start: `f67e9a53b70be84cb5dd40586f59e1009c1724ab` (fetched current origin/main).
Final: the commit containing this report; resolve with `git rev-parse HEAD`.

## Findings and changes

1. **ImagingIO transaction.** Replaced process-local counter names with process ID + fresh 128-bit random U++ UUID names, exclusively created before opening the writer. Stale candidates cannot be overwritten even on a name collision. Candidates remain in the destination directory. Windows MoveFileExW(REPLACE_EXISTING) and POSIX rename promote without moving the destination aside. No backup/restoration race remains. Considered ReplaceFileW, but its metadata-merging/partial-failure recovery contract adds complexity not required by the existing content-replacement semantics. This does not promise power-loss durability, hostile-directory security or preservation of destination ACL identity. POSIX implementation is reviewed, not runtime-tested here.
2. **Temporary verification.** Explicit complete-payload readability/integrity check, named VerifyTemporaryReadability. Decodes batches into about 1 MiB scratch (or one unusually wide row), rather than retaining a second full image. Backend-internal buffers are outside that bound. Pixel identity remains format-specific test evidence, not an invalid universal promise for quantized RGBE. Added a multi-batch PNG case.
3. **Pinned OIIO error lifetime.** Confirmed thread-local maps and disabled destructor erasure in pinned 3.1.15.0. geterror clears entries but not map capacity; undrained object errors can remain. Documented all joined-worker workaround sites and upstream fix ce1be9749586ee7ef3f2b9cb82aee8b199991f03. Retained production generic probing because replacing it with extension-only factories changes content sniffing/fallback acceptance; EXR preview already validates magic and uses the explicit reader. No upstream patches or production threads added.
4. **HEIF lifecycle.** Normal initialization has one call_once-protected OIIO heif_init and one exit heif_deinit, including repeated InitializeOpenImageIO calls. Found an additional unused-library shutdown defect: libheif implicitly registers decoder plugins before main even if no imaging API is called, but normal teardown was registered only on first use. The unchanged imaging_test printed 6/0 then crashed in U++ MemoryDumpLeaks -> logging -> destroyed mutex. An isolated initializer probe exited cleanly. Added a package-owned translation unit including the unchanged upstream plugin registry, followed by a destructor that empties remaining plugin registries before their storage and initialization-time mutexes are destroyed. Normal heif_deinit already empties them, so this fallback is inert on the usual path and does not change init reference counts. No test prewarm remains. Repository HEIF objects are operation-scoped and worker threads joined; native clients must not retain live objects into static teardown.
5. **Core ownership/moves.** Existing U++ pick/move implementations correctly transfer buffers; added pointer-identity move construction/assignment checks. Fixed ImageData::IsValid accepting mismatched specification/buffer sample types (including UInt16 versus Float16 of equal width). Expanded this foundational invariant for readability. Copy paths remain intentional value copies.
6. **Sample utilities.** Reviewed Color's float normalization/round-to-nearest-even half writes and Analysis's double normalization/statistics reads. Diagnostics compares already supplied doubles and does not duplicate a pixel decoder. Retained the private routines: a shared numeric policy would need to preserve float-versus-double rounding and non-finite conventions; no new public abstraction was justified in this pass.
7. **ImagingColor performance.** Float32 contiguous RGB/RGBA now gives OCIO an RGB-only descriptor over the candidate buffer, with explicit pixel stride. This removes the temporary row and gather/scatter passes on that path while preserving alpha bits, including signed zero/NaN payloads. Other samples/layouts keep one reused row. Candidate copying remains required for failure atomicity and alias safety. Added packed-versus-general multi-row comparison.
8. **EXR preview memory.** Left decoder unchanged. A scanline-only replacement would narrow existing tiled-image acceptance; a robust chunk/tile implementation needs additional tile/origin fixtures. Current 22-check preview semantics are preserved.
9. **FFmpeg compatibility/configuration.** ff_sws_chroma_pos body is token-equivalent to the pin, after comments/whitespace are removed. !CONFIG_UNSTABLE prevents duplicate ownership; unstable/backends/SIMD policy unchanged. Fixed config parity scan skipping repository-local C materializers by resolving every C manifest entry relative to its package. Generated registries remain covered by recursive include scanning. Literal macro coverage remains a source audit, not a full preprocessor or a proof of capability values. Added explicit pin-change maintenance instructions. No production FFmpeg source/config change.
10. **Hygiene.** No tracked generated binaries, output directories or transient diagnostics. Existing legitimate packages and compatibility forwarder retained. Corrected ImagingIO README overclaims for HEIF extensions and TIFF multichannel support. Pinned submodules remain unchanged. Temporary build/debug artifacts stay under ignored out; the investigative lifetime probe is removed after validation.

## Deferred performance work

- Prepared OCIO config/CPU processor: expected to remove repeated config load/validation and processor lookup for frame sequences. Deferred until profiling and file-config invalidation/lifetime rules justify a small reusable API.
- EXR chunk/tile conversion: expected to reduce float-image scratch to a bounded chunk while retaining encoded stream + RGBA8 output. Deferred to preserve tiled and negative-origin semantics with suitable fixtures.
- Sample conversion consolidation: modest maintenance benefit; deferred rather than alter established float/double rounding policies alongside correctness fixes.

## Validation

Validation results and published checkpoint identifiers are recorded in ACTIVE_WORK.md.
The shared libheif lifecycle correction justified rechecking the still-image
Debug/Release boundary. Other unrelated acceptance programmes were not repeated.
The previously failing link-only umbrella test is a required shutdown regression,
not a passing check total with a crashed exit.

## Changes by checkpoint

| Commit | Files / reason |
| --- | --- |
| bac083e | libheif_src registry materializer/manifest/README; HEIF registration comment, README and repeat-init gate; OpenImageIO README; fixes unused-library shutdown and records pinned error debt |
| b43594e | ImagingCore header and core test; sample-type coherence and zero-copy move evidence |
| 2415a54 | ImagingIO implementation/private transaction header/package/README and imaging_io_test; exclusive naming, native promotion and bounded verification |
| e56d3ba | ImagingColor implementation/README and colour test; direct Float32 path with alpha preservation |
| af4914d | ffmpeg_headers_test and FFMPEG_PLAN; include local materializers in parity and document pin maintenance |
| Documentation closure | architecture, catalogue, status, Windows acceptance, EXR memory deferral and this review; align claims and totals |

Every checkpoint updates ACTIVE_WORK.md. Final verified evidence: 58 runs,
1,576 passed checks, zero failed checks, all process exits 0; plus two concurrent
12-save worker processes with exits 0 and no transaction residue. See the exact
per-gate Debug/Release table in ACTIVE_WORK.md. The original shutdown crash was
investigated and fixed; it is not included as a successful validation run.

Final status: **PASS — HARDENING COMPLETE, PERFORMANCE ITEMS DEFERRED**.
