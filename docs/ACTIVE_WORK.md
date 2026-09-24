# Active Work

## BASE

`ccaeaafa34a91f23c413674ae9bd1f580f0cb5be` on current main after
the Expat checkpoint. The original required checkpoint,
`58f317135f19ec254e2fae016a9962c82420ff8f`, remains an ancestor.

## TASK

IMG-REL-002: local release preparation, security refresh and cleanup.
This is the third coherent checkpoint: update the linked libheif family
from 1.23.1 to the 1.23.5 security release, retain its decode-only
backend and lifetime overlay, and extend the HEIF/AVIF regressions.

## TOUCHED

- `third_party/codecs/libheif_src/upstream`: submodule pin moved from
  `2c4bbb54c2738d4a5efbbe3e5fa1d5d76bb88eb0` to the verified
  upstream 1.23.5 tag at `413e2a87e6a70b3eccc3a3adc5801179dd2d9e00`.
- Generated `heif_version.h`, package manifests/READMEs and the retained
  `plugin_registry_lifetime.cc` comment now match the pin.
- `tests/heif_imagingio_test/main.cpp`, `tests/heif_oiio_test/main.cpp`
  and `tests/expected_counts.txt`: exact AVIF/HEIC pixel fixtures,
  oversized AV1-header rejection, repeated caller-thread malformed input.
- `tools/validate.ps1`: drain redirected process streams and capture a
  scalar exit code after the timed wait; still reject missing/nonzero exits.
- `THIRD_PARTY.md`, `LICENSES.md`, `CHANGELOG.md`, package catalogue,
  security review and this report: provenance and advisory applicability.

## STATUS

PARTIAL — linked Expat/OCIO and libheif/HEIF focused Debug and Release
matrices pass. The full clean suite, Workbench manual checks, other
dependency families, input limits and Linux/macOS execution remain gates.
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
the Expat checkpoint is `ccaeaafa34a91f23c413674ae9bd1f580f0cb5be`.
This checkpoint's final SHA resolves with `git log -1 -- docs/ACTIVE_WORK.md`.
The branch deletions above are already verified on origin.

## VALIDATION

The final libheif focused pass recorded `heif_imagingio_test` 13/0,
`heif_oiio_test` 12/0 and `imaging_test` 6/0 in both Debug and Release:
six clean exits and 62 checks. The AVIF/HEIC decoded dimensions and FNV
pixel hashes match in both configurations. The oversized AV1 corpus
was rejected promptly. Earlier Expat/OCIO focused tests passed 316
checks and six clean exits; the first checkpoint's Core/IO/EXR matrix
passed 316 checks and eight clean exits. Both OCIO generator `--check`
modes passed. The minimum manifest now has 49 targets and 1,372 checks
per configuration (2,744 total). Logs and executables are under
`build/windows-x64/validation/`.

The first clean full-suite attempt stopped at `dpx_cineon_oiio_test`
Debug after its 19/0 summary because `Start-Process` reported a blank
exit code. Its failed result is preserved at
`build/windows-x64/validation/full-clean-attempt-1-results.txt`.
Direct and focused runner repeats both returned 19/0 and exit 0;
the runner now completes the stream-draining wait before reading the
exit code. The integrated clean suite has not yet passed.

## NEXT ACTION

Publish the libheif checkpoint. Then clean the complete intermediate
root and run all 49 retained tests in both Windows
configurations, build and exercise Workbench, remove the proven generated
legacy `out/` tree and inspect `bin/`. Continue per-family security and
input-limit work. Resolve the upstream Expat DoS and actual Windows zlib
provider before any security clearance; Linux/macOS remain untested.
