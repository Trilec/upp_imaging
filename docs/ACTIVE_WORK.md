# Active Work

## BASE

`58f317135f19ec254e2fae016a9962c82420ff8f` on current main after
`git fetch --prune origin`; the required checkpoint is an ancestor.
The worktree was clean before IMG-REL-002.

## TASK

IMG-REL-002: local release preparation, security refresh and cleanup.
This is the first coherent checkpoint: finish local build output routing,
tighten validation evidence and stop tests recreating `out/`.

## TOUCHED

- `tools/validate.ps1` and `tests/expected_counts.txt`: exact summary
  validation, expected count floor, stale-executable prevention, evidence.
- `tests/openexr_test/main.cpp` and
  `tests/openexr_core_rgba_zip_test/main.cpp`: runtime fixtures under
  `build/windows-x64/runtime/` when run by the Windows validator instead
  of `out/`; standalone runs use the platform temp directory.
- `docs/BUILD_AND_RUN.md`, `docs/WINDOWS_ACCEPTANCE.md`, this report.
- Local ignored `GitHubOut.var`: absolute
  `OUTPUT = "E:/apps/github/upp_imaging/build/windows-x64/umk";`.

## STATUS

PARTIAL — focused Windows output-path validation passes. The full clean
suite, Workbench release checks, security refresh, input limits and
Linux/macOS execution remain release gates. No release binary is published.

The old `out/` tree was inventoried: 12,689 untracked generated files,
26,490,790,815 bytes, no tracked files or reparse points. Historical
structure logs and results were copied to
`build/windows-x64/legacy-evidence/`. Delete the old tree only after
the clean full build succeeds.

Remote branch cleanup: deleted
`recovery/still-image-acceptance-20260818` at
`2aeae97b4238ee845d0800f47b3f45e806e84b33` (ancestor of main);
deleted `recovery/still-image-acceptance-final`, `final2` and
`final3`, all at `35e23a54f9b835135f11f9c0600cdcdc069effcf`
(the one unmerged documentation/manifest commit is superseded by the
retained migration and current 89-check IO contract). The GitHub API
reported no open PRs; each remote deletion used an exact expected-tip
lease, and fetch/prune confirmed the refs are gone.
Retained `supervisor/img-shutdown-009` at
`f278f14ba9936f3b827396eeb929ab906edce10b`: its
application-scoped `OIIO::shutdown()` experiment is absent from main,
which uses different targeted lifetime fixes. Keep it for a separate
lifecycle decision.

## PUBLISHED

This checkpoint; resolve with `git log -1 -- docs/ACTIVE_WORK.md`.
The branch deletions above are already verified on origin.

## VALIDATION

Focused Windows CLANGx64 / `E:/upp-18468/umk.exe`:
`imaging_core_test` 52/0, `imaging_io_test` 89/0,
`openexr_test` 11/0, `openexr_core_rgba_zip_test` 6/0
in both Debug and Release: eight clean exits and 316 checks.
The compiler created intermediates in `build/windows-x64/umk/`;
executables and logs are in `build/windows-x64/validation/`.
Invalid configuration names are rejected. The first sandboxed IO build
could not write a U++ response file in `E:/upp-18468/cache`; the
unsandboxed retry and the focused matrix passed.
`git diff --check` passes. This focused run predates its commit and
does not claim a clean full release build.

## NEXT ACTION

Publish this checkpoint. Upgrade the reachable Expat 2.7.2 family
using the verified official 2.8.5 archive as a separate checkpoint.
Then audit the remaining dependency graph and advisories, enforce
input limits, complete Windows Workbench/manual checks and the clean
retained suite, remove proven legacy artifacts, and record the
Linux/macOS boundary. No security or cross-platform PASS is claimed.
