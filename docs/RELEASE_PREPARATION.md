# Release preparation

Current execution evidence is in [ACTIVE_WORK.md](ACTIVE_WORK.md).
This document preserves the original release-preparation contract and
its starting-point statements below; those statements are historical
after IMG-REL-002 began.
The clean Windows suite has since passed 49 Debug and 49 Release targets
with 2,746 total checks and 98 clean exits. Workbench manually accepted
by Curt. Security clearance and final `bin/` publication remain pending.
The later OpenEXR 3.4.14, OpenJPH 0.27.1, Windows zlib 1.3.2 provider,
OIIO read limits and expanded Workbench input path have focused Windows
checks recorded in [ACTIVE_WORK.md](ACTIVE_WORK.md). The
historical 2,746-check ledger does not validate those later changes; run a
new clean integrated suite at the next substantive release-candidate point.
The unresolved Expat XML denial-of-service and remaining dependency/input
review still prevent security clearance. The obsolete `out/` tree and five
reviewed remote branches have been removed; their evidence remains under
`build/windows-x64/` and in [ACTIVE_WORK.md](ACTIVE_WORK.md).

## Boundary and authority

This is the next release-preparation contract, not a completed release report.
Baseline: `17bdbb3e326a07e71f159cb034c34534fb6f6bff`.
The recorded Windows migration passed 49 Debug and 49 Release test executions,
2,734 checks, with clean exits. Workbench/benchmark builds passed; the optional
Debug benchmark exceeded its observation limit. That timeout is not a measured
Release performance result or a failure of the required test suite.

The current checkpoint changes output routing and documentation only. It does
not update a dependency pin, prove a security fix, run Windows binaries, remove
local artifacts/branches or establish Linux/macOS support.

Keep the eight source directories and current package boundaries. Read
`tests/acceptance.txt`, `docs/WINDOWS_ACCEPTANCE.md`, `docs/HARDENING_REVIEW.md`
and `docs/STRUCTURE_MIGRATION.md`; do not restart retired acceptance tasks.

## 1. Complete local build/bin migration

Follow [BUILD_AND_RUN.md](BUILD_AND_RUN.md). Update the active local U++ assembly,
check TheIDE target/run settings and update any remaining live build commands.
Test executables/logs stay under `build/`; verified Release applications go to
`bin/<platform>/`. Validate Workbench runtime log/settings paths, launch from a
different working directory and reject stale executable reuse after a failed build.

Review `tools/validate.ps1` before final evidence: reject invalid configurations,
missing/duplicate/malformed summaries, zero-check or unexpectedly reduced test
runs and nonzero exits; preserve useful timeout/failure logs. Report the tested
source SHA and actual per-test counts, not merely a matching `failed=0` substring.
Keep one authoritative expected-count source and explain deliberate test additions.

Inventory and remove only known generated stale executables/artifacts after fresh
builds work. Do not equate deleting an old binary with deleting a current test.
Keep all 49 retained tests unless a reviewed coverage mapping proves a test
obsolete or replaces its distinct contract. Preserve the no-Core PNG/zlib route.

## 2. Review obsolete branches before deletion

Candidates only (refresh all refs and open pull requests before acting):

```text
recovery/still-image-acceptance-20260818
recovery/still-image-acceptance-final
recovery/still-image-acceptance-final2
recovery/still-image-acceptance-final3
supervisor/img-shutdown-009
```

Record each current tip and inspect commits absent from main. An ancestor is
safe to retire; an unmerged branch requires patch/content comparison showing
that its work is incorporated or intentionally superseded. Do not merge old
shutdown experiments into the new tree to make a branch appear merged.
Delete only these reviewed retired refs, preserving main and active work.
Recheck the expected tip immediately before deletion; prefer an explicit
lease-protected ref deletion so a concurrently advanced branch is not removed.
Record reasons and verify the remote ref is gone. Do not force-rewrite main.

## 3. Verify dependency security, then refresh coherent families

Inventory every retained dependency, including nested libjxl dependencies,
OCIO bundled internals, and the actual U++ plugin/z provider when that route is
linked. A version label in a README is not proof of the implementation in a binary.
Record exact source pin/hash, downstream overlays, enabled features and provider.

Priority review families are libheif/libde265/dav1d, OpenImageIO, OpenEXR with
Imath/OpenJPH/libdeflate, Expat, minizip-ng, FFmpeg, and YAML/OCIO configuration
parsers. Cover the remaining PNG/JPEG/JXL/WebP/TIFF/RAW/support dependencies too.

Check official release notes/advisories at execution time. Choose the latest
supported compatible security-patched release; record why any newer feature
release is not selected. Earlier chat upgrade numbers are leads, not an
authoritative fixed-version list. For each advisory record its identifier/link,
affected and fixed versions, affected function/component, compiled/reachable
status in this build and disposition. Use affected / not affected / unresolved;
no search results is not proof that a dependency has no vulnerabilities.

Initial official sources checked on 2026-09-24 establish useful review leads,
not a complete applicability audit:

- Expat's official news lists 2.8.5 (2026-09-22) with security fixes:
  https://libexpat.github.io/doc/news/
- FFmpeg's official download page lists 9.0.2 (2026-09-18) for the 9.0 branch:
  https://www.ffmpeg.org/download.html
- OpenEXR's official news describes security/hardening changes after 3.4.13;
  several advisories are specific to Python, tools or 32-bit builds, so inspect
  applicability rather than marking the whole native 64-bit stack affected:
  https://openexr.com/en/latest/news.html

Update one dependency family per coherent checkpoint. Refresh its generated
configuration, headers, implementation slices and plugin copies together.
For OIIO especially, changing only the plugin-source submodule does not update
separately copied main/util/header sources. Reconcile every existing downstream
wrapper/overlay against the new pin; retain a workaround only with a reason,
reproducer/regression and removal condition. Do not remove MinGW lifecycle fixes
merely because an upstream per-object error leak was fixed; they are distinct.

Preserve explicit source ownership, licenses and the bounded FFmpeg feature set.
Do not add codecs, networking, hardware, encoders or SIMD during a security bump.
Prefer released upstream fixes; unavoidable local backports must be separate,
traceable, reproducible and fail closed when their input source changes.

Record findings in `docs/THIRD_PARTY_SECURITY.md` and reconcile `THIRD_PARTY.md`
and `LICENSES.md`. These files must distinguish verified facts from pending items.

## 4. Input hardening and portability

Define legitimate VFX workload limits explicitly: dimensions, decoded bytes,
channels, metadata and frame/subimage counts. Enforce backend limits before
expensive decode where supported, plus overflow-safe framework checks before
allocation. A post-decode check alone does not prevent decoder exhaustion.
Document configurable exceptions; do not impose arbitrary limits silently.
Test truncated/oversized inputs and repeated failures on the calling thread as
well as workers. Do not hide lifecycle or memory-growth failures in short-lived
threads or disabled leak checks.

Audit generated configs, source-selection flags, file APIs, case-sensitive
includes, UTF-8 paths, pthread/Windows shims and x64/ARM64 assumptions.
Windows x64, Linux x64 and macOS ARM64 need separate build/runtime evidence;
macOS x64 is optional unless explicitly adopted. Unsupported configs should
fail clearly, not silently inherit Windows-generated capabilities.
Keep results in `docs/PORTABILITY_MATRIX.md`; missing local hardware is a pending
platform gate, not a reason to fabricate a green result.

Run ASan/UBSan and bounded malformed-input/fuzz regressions in a separate
supported configuration where available. Preserve normal U++ heap/leak-check
validation as well. A clean sanitizer run is evidence for that configuration,
not a blanket security certification. Add harnesses only for real public/native
entry points; do not recreate the removed probe forest.

## 5. Final release evidence

Run focused affected tests while implementing. At the integrated endpoint,
use a clean intermediate root and run the complete retained Debug/Release suite
once. Keep exact totals and exit codes; investigate the first substantive failure
and repair it without weakening assertions. Re-run impacted checks after a fix.
Do not demand the historical total 2,734 when reviewed regressions add checks.

Build and exercise ImagingWorkbench Debug and Release: EXR/PNG load/save,
channels/passes, Fit/zoom/pan/probe, source histogram, exposure/gamma, OCIO
config/display/LUT, invalid-file recovery and normal exit. Publish only the
verified Release binary to bin and record its SHA-256, source SHA, toolchain,
architecture, configuration and dependency manifest outside the launch directory.
Performance claims require comparable measured Release runs; the manual benchmark
is not a replacement for correctness tests.

Refresh current docs, links and package paths; retain historical reports as
historical. Unknown advisory reachability or untested platforms remain explicit
release gates. Never call Windows-only validation a cross-platform release.
Do not publish a release tag/upload an artifact without separate authorization.

## Reporting

Every publish updates ACTIVE_WORK with BASE / TASK / TOUCHED / STATUS / PUBLISHED /
VALIDATION / NEXT ACTION. Review the complete diff, run `git diff --check`, fetch
before a non-forced publish, and fetch/inspect the resulting remote commit.
Report changed paths, exact SHAs, per-platform evidence, remaining security
findings and the next action. Use PASS only for the scope actually completed;
use PARTIAL or FAIL with the specific remaining boundary otherwise.
