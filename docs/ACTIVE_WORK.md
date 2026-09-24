# Active Work

## BASE

`d9b7867619365bd564cf444cf8e3a808bfddccf0` on current main after
the first IMG-REL-002 checkpoint. The original required checkpoint,
`58f317135f19ec254e2fae016a9962c82420ff8f`, remains an ancestor.

## TASK

IMG-REL-002: local release preparation, security refresh and cleanup.
This is the second coherent checkpoint: update the linked Expat family
from 2.7.2 to 2.8.5, review the advisories against the compiled code,
and record the Windows and other-platform boundaries.

## TOUCHED

- `third_party/support/expat_src/`: all upstream library source and public
  headers from the verified 2.8.5 release archive, plus a reviewed generated
  Windows x64 configuration and `rand_s` entropy source in the U++ manifest.
- `tests/expat_test/main.cpp` and `tests/expected_counts.txt`: 64 repeated
  caller-thread malformed UTF-16 parses and a 4-check minimum.
- `THIRD_PARTY.md`, `LICENSES.md`, `README.md`, `CHANGELOG.md`, Expat README,
  package catalogue, build/release docs and this report: provenance and status.
- New `docs/THIRD_PARTY_SECURITY.md` and `docs/PORTABILITY_MATRIX.md`.

## STATUS

PARTIAL — the linked Expat update and OCIO callers pass the focused Debug
and Release matrix. The full clean suite, Workbench manual checks, other dependency
families, input limits and Linux/macOS execution remain release gates.
No release binary is published.

The [security review](THIRD_PARTY_SECURITY.md) records an unfixed upstream
Expat denial of service with unresolved reachability through OCIO user XML,
as well as the installed U++ Windows zlib 1.3.1 provider despite the
repository's 1.3.2 source copy. Do not call this release security-cleared.

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

The first checkpoint is `d9b7867619365bd564cf444cf8e3a808bfddccf0`.
This checkpoint's final SHA resolves with `git log -1 -- docs/ACTIVE_WORK.md`.
The branch deletions above are already verified on origin.

## VALIDATION

The final focused runner pass recorded `expat_test` 4/0,
`opencolorio_test` 18/0 and `imaging_workbench_ocio_test` 136/0
in both Debug and Release: six clean exits and 316 checks. Executables,
build/run/stderr logs and `results.txt` are under
`build/windows-x64/validation/`. The direct Expat tests also passed
4/0 and exit 0 in each configuration before the final runner pass.
Both OCIO generator `--check` modes passed. The new minimum manifest has
49 targets, 1,368 checks per configuration. The first checkpoint's
Core/IO/EXR focused matrix passed 316 checks and eight clean exits.
This is not a clean full-suite or manual Workbench acceptance claim.

## NEXT ACTION

Publish the Expat/OCIO checkpoint. Then clean the complete intermediate
root and run all 49 retained tests in both Windows
configurations, build and exercise Workbench, remove the proven generated
legacy `out/` tree and inspect `bin/`. Continue per-family security and
input-limit work. Resolve the upstream Expat DoS and actual Windows zlib
provider before any security clearance; Linux/macOS remain untested.
