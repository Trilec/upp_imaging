# Active Work

## BASE

`b80ddbfa1ae35dbce797b111d9b53620dac7509f` on current main after
the OpenImageIO checkpoint. The original required checkpoint,
`58f317135f19ec254e2fae016a9962c82420ff8f`, remains an ancestor.

## TASK

IMG-REL-002: local release preparation, security refresh and cleanup.
This is the sixth coherent checkpoint: refresh the bounded FFmpeg source
and generated header/configuration family from 9.0.1 to 9.0.2, then run
integrated clean Windows acceptance and stage the Workbench.

## TOUCHED

- `third_party/ffmpeg/`: advance the upstream submodule to
  `946fcce07b6dcd0331c8cc609192aeff5e1924f8`, update the generated
  version/configuration labels and package manifests, and retain the
  reviewed `chroma_pos_compat.c` overlay.
- `tests/ffmpeg_avutil_test/` and `tests/ffmpeg_headers_test/`: assert the
  new release string and version contract without changing check counts.
- Security, licensing, package catalogue, roadmap and changelog docs:
  record the applicable MOV/H.264 changes and remaining release gates.

## STATUS

PARTIAL — the FFmpeg family and clean Windows Debug/Release suite pass.
Workbench manually accepted by Curt. That confirmation does not identify
the exact staged executable hash. Other dependency families, input limits,
security clearance and Linux/macOS execution remain gates. No release
binary is published.

The [security review](THIRD_PARTY_SECURITY.md) records an unfixed upstream
Expat denial of service with unresolved reachability through OCIO user XML,
as well as the installed U++ Windows zlib 1.3.1 provider despite the
repository's 1.3.2 source copy. Do not call this release security-cleared.

The old `out/` tree was inventoried: 12,689 untracked generated files,
26,490,790,815 bytes, no tracked files or reparse points. Historical
structure logs and results were copied to
`build/windows-x64/legacy-evidence/`. An older Workbench was running from
`out/ImagingWorkbench.exe` at the earlier inventory. Recheck its current
process identity and arrange normal closure before rechecking and removing
that generated tree.

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
the process runner is `a1ed7337a343311d1823c374f31ac3ffda92dc30`;
OpenImageIO is `b80ddbfa1ae35dbce797b111d9b53620dac7509f`.
FFmpeg is `26c494498e6d975c77c552d002ce4e65a06c1960`.
This evidence update's final SHA resolves with `git log -1 -- docs/ACTIVE_WORK.md`.
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

The FFmpeg 9.0.2 focused matrix passed six targets in each Windows
configuration: 87 checks and six exits at 0 per configuration (174 checks,
12 exits total). Five additional first-frame runs in each configuration
passed the exact 27-check fixture/pixel contract, 270 further checks and
ten exits at 0. The official archive signature passed against FFmpeg's
release key; 3,923 C/header/assembly files in the four linked library
trees match the pinned checkout after line-ending normalization. The Git
tag signature is cryptographically good but its published key was expired
at the tag date; see [security review](THIRD_PARTY_SECURITY.md).

The clean integrated Windows run passed all 49 Debug targets and all 49
Release targets: 1,373 checks and 49 process exits at 0 in each
configuration, 2,746 checks and 98 exits at 0 overall. The complete
intermediate root was checked for tracked files and reparse points, then
emptied before the run. The immutable ledger is
`build/windows-x64/release/acceptance-26c4944-clean-results.txt`
(SHA-256 `5244B2EC1B15ACDE06E9EAA1FC588F31CCBECEAEFFE49FF462AF82ED4E6A684D`).
Its `source=` header names `b80ddbfa1ae35dbce797b111d9b53620dac7509f`
because the runner started before the FFmpeg source update was committed.
The tested worktree already contained the FFmpeg 9.0.2 submodule,
generated headers and version assertions subsequently published in
`26c4944`; only documentation changed while the suite ran. The
`ffmpeg_avutil_test` 9.0.2 assertion passed in both configurations.
This records the tested source relationship without rerunning the suite
solely to change the ledger header. Earlier stopped clean attempts are
historical; their logs remain under `build/windows-x64/validation/`.

Both Workbench staging builds passed after the clean suite. The staged
Debug SHA-256 is `18BCE9C2EACE0777543C5495C6553953E8D0CF4FFA0FAA0F2300FFBDC0E723CB`;
the staged Release SHA-256 is
`8970C47030AB437ED127C23012911D19008588AC0D4B1ED2E269B9E200468619`.
These identify build artifacts, not necessarily the exact executable
Curt manually accepted. The earlier automated GUI observations used
different staged hashes and are historical. Workbench manually accepted
by Curt; the GUI is no longer a release gate.

## NEXT ACTION

Determine applicability of the unresolved Expat DoS through OCIO's actual
XML path and resolve the linked Windows U++ zlib provider. Complete the
remaining dependency-family audit and configurable input limits with
focused regressions. Recheck the old Workbench process, then remove only
verified generated `out/` contents after normal closure. Finish review
of `supervisor/img-shutdown-009`. Do not publish `bin/` until the
security and final artifact gates pass. Linux/macOS and sanitizer/fuzz
execution remain untested.
