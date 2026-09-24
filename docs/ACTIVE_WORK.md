# Active Work

## BASE

`a1ed7337a343311d1823c374f31ac3ffda92dc30` on current main after
the process-runner checkpoint. The original required checkpoint,
`58f317135f19ec254e2fae016a9962c82420ff8f`, remains an ancestor.

## TASK

IMG-REL-002: local release preparation, security refresh and cleanup.
This is the fifth coherent checkpoint: refresh the linked OpenImageIO
source/header/plugin family from 3.1.15.0 to 3.1.17.0 after upstream
fixed reachable Cineon and EXR memory corruption defects.

## TOUCHED

- `third_party/openimageio/`: advance the plugin submodule to
  `73bc189f7d8469a9760ce9c5099b686c77695074` and update its separately
  copied main, utility and public-header slices, manifests and package
  provenance together. Retain local include shims and the separate MinGW
  main-thread error-storage wrappers.
- `tests/dpx_cineon_oiio_test/` and `tests/expected_counts.txt`: cover 64
  alternating malformed opens on the ordinary caller thread; minimum rises
  from 19 to 20 (full-suite minimum 1,373 per configuration).
- Security, licensing, package catalogue and changelog docs: record the
  new pin, actual fixes and remaining release gates.

## STATUS

PARTIAL — the OpenImageIO family has been refreshed and its focused Windows
matrix passes. The full clean suite, Workbench manual checks,
other dependency families, input limits and Linux/macOS execution remain
gates.
No release binary is published.

The [security review](THIRD_PARTY_SECURITY.md) records an unfixed upstream
Expat denial of service with unresolved reachability through OCIO user XML,
as well as the installed U++ Windows zlib 1.3.1 provider despite the
repository's 1.3.2 source copy. Do not call this release security-cleared.

The old `out/` tree was inventoried: 12,689 untracked generated files,
26,490,790,815 bytes, no tracked files or reparse points. Historical
structure logs and results were copied to
`build/windows-x64/legacy-evidence/`. An older Workbench process is still
running from `out/ImagingWorkbench.exe`; leave the tree intact until it
closes and a clean full build succeeds.

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

The first checkpoint is `d9b7867619365bd564cf444cf8e3a808bfddccf0`;
Expat is `ccaeaafa34a91f23c413674ae9bd1f580f0cb5be`;
libheif is `319b0e6d8c32f08e325862c1d18ec8622ab45f8e`;
the process runner is `a1ed7337a343311d1823c374f31ac3ffda92dc30`.
This checkpoint's final SHA resolves with `git log -1 -- docs/ACTIVE_WORK.md`.
The branch deletions above are already verified on origin.

## VALIDATION

The new process runner passed `dpx_cineon_oiio_test` 19/0 and
`imaging_core_test` 52/0 in Debug and Release: four clean exits and
142 checks. The final libheif focused pass recorded `heif_imagingio_test` 13/0,
`heif_oiio_test` 12/0 and `imaging_test` 6/0 in both Debug and Release:
six clean exits and 62 checks. The AVIF/HEIC decoded dimensions and FNV
pixel hashes match in both configurations. The oversized AV1 corpus
was rejected promptly. Earlier Expat/OCIO focused tests passed 316
checks and six clean exits; the first checkpoint's Core/IO/EXR matrix
passed 316 checks and eight clean exits. Both OCIO generator `--check`
modes passed. The minimum manifest now has 49 targets and 1,373 checks
per configuration (2,746 total). Logs and executables are under
`build/windows-x64/validation/`.

The 3.1.17.0 OpenImageIO focused matrix passed 10 targets in each Windows
configuration: 153 checks and 10 exits at 0 per configuration (306 checks,
20 exits total). The DPX/Cineon test now performs 64 alternating malformed
opens on the caller thread in addition to the existing worker path; its
Debug and Release runs each passed 20/0. This is focused evidence only.

Two clean full-suite attempts stopped at `dpx_cineon_oiio_test` Debug
after its 19/0 summary because `Start-Process` reported a blank exit
code. The first failed result is preserved at
`build/windows-x64/validation/full-clean-attempt-1-results.txt`.
Direct and warm focused reruns returned 19/0 and exit 0; the new
explicit process launcher also passed that target in both configurations.
The integrated clean suite has not yet passed.

## NEXT ACTION

Publish the OpenImageIO checkpoint. Then clean the
complete intermediate root and run all 49 retained tests in both Windows
configurations, build and exercise Workbench, remove the proven generated
legacy `out/` tree and inspect `bin/`. Continue per-family security and
input-limit work. Resolve the upstream Expat DoS and actual Windows zlib
provider before any security clearance; Linux/macOS remain untested.
