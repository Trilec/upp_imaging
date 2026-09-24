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

PARTIAL — the FFmpeg family is refreshed and its focused Windows matrix
passes. The clean full suite is running. Workbench manual checks,
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
closes and the generated tree can be rechecked safely.

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

The FFmpeg 9.0.2 focused matrix passed six targets in each Windows
configuration: 87 checks and six exits at 0 per configuration (174 checks,
12 exits total). Five additional first-frame runs in each configuration
passed the exact 27-check fixture/pixel contract, 270 further checks and
ten exits at 0. The official archive signature passed against FFmpeg's
release key; 3,923 C/header/assembly files in the four linked library
trees match the pinned checkout after line-ending normalization. The Git
tag signature is cryptographically good but its published key was expired
at the tag date; see [security review](THIRD_PARTY_SECURITY.md).

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
