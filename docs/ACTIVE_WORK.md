# Active Work

Recovery authority for the current hardening checkpoint. `docs/WINDOWS_ACCEPTANCE.md`
is the gate-order contract; `docs/HARDENING_REVIEW.md` will record the complete review.

## BASE

- Fetched `origin/main` at task start: `f67e9a53b70be84cb5dd40586f59e1009c1724ab`; no intervening commits.
- U++ `E:/upp-18468/umk.exe`, local `GitHubOut`, `CLANGx64`, Debug `DEBUG_FULL` Noblitz and Release.
- Installed U++ and pinned upstream source trees are unchanged.

## HARDENING CHECKPOINTS

### 1. Shared backend lifetime and pinned dependency debt

- The link-only umbrella test exposed a real shutdown failure: 6/0 followed by exit -1073741819, with the debugger tracing U++ MemoryDumpLeaks through logging to a destroyed mutex.
- libheif registers built-in plugins before main even without an imaging call. The normal OIIO-owned init/deinit callback was never installed in that case. A diagnostic initializer-only probe confirmed the cause; no test prewarm is part of the repair.
- `libheif_src/plugin_registry_lifetime.cc` includes the unchanged pinned registry and adds ordered cleanup of remaining registrations. Its destructor runs before registry storage and plugin initialization mutexes are destroyed. Normal heif_deinit empties those registries first, preventing duplicate cleanup.
- Existing OIIO call_once initialization and its single paired heif_deinit are retained. HEIF's gate now repeats InitializeOpenImageIO sixteen times.
- Documented OIIO 3.1.15.0 thread-local error-map debt, joined-worker test scopes and upstream fix ce1be9749586ee7ef3f2b9cb82aee8b199991f03. Generic production probing and pinned sources remain unchanged.

## VALIDATION

- Debug still-image matrix passed with clean exits after the lifecycle repair. Release still-image matrix also passed with clean exits.
- Unchanged `imaging_test` now passes 6/0 and exits 0 under the debugger and in the normal Debug run.
- HEIF direct 11/0 and ImagingIO 10/0 pass in both configurations. The direct gate includes sixteen repeated initializer calls.
- Remaining focused hardening checkpoints and final consolidated evidence are in progress; do not infer final completion from this checkpoint.

## NEXT ACTION

Complete Core/ImagingIO/colour/config-audit checkpoints, repeat shutdown-sensitive
Debug gates, verify the combined published diff and synchronize main without force.

## CHECKPOINT 2 — CORE INVARIANT

- `ImagingCore/ImagingCore.h`: require buffer/specification sample-type agreement in ImageData::IsValid; mismatched widths could otherwise allow consumers to read beyond the buffer.
- Added equal-width and differing-width mismatch tests and pointer-identity move construction/assignment checks. Existing pick/move implementations remain unchanged.
- `imaging_core_test` passes 52/0 in Debug and Release. Affected numerical/colour/IO Debug gates pass; combined Release/repeat validation continues.
- Preceding lifecycle commit: `bac083e`.
