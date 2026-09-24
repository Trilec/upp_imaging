# Active Work

## BASE

`17bdbb3e326a07e71f159cb034c34534fb6f6bff` / main, freshly read before this work.
The structure migration remains the last recorded Windows-accepted source:
eight source directories, 124 packages, 49 tests per configuration, 2,734 checks
and 98 clean exits. See WINDOWS_ACCEPTANCE.md and STRUCTURE_MIGRATION.md.

## TASK

IMG-REL-001: normalize build/bin output and documentation, then complete local
release preparation under RELEASE_PREPARATION.md. Dependency security refresh,
branch/artifact cleanup and Linux/macOS validation are not yet completed.

## TOUCHED

- `.gitignore`, `GitHubOut.var.example`, `tools/validate.ps1`: ignore build/bin,
  redirect compiler template and validation outputs away from legacy out.
- `README.md`, `CHANGELOG.md`, `apps/ImagingWorkbench/README.md`.
- `docs/BUILD_AND_RUN.md`, `docs/RELEASE_PREPARATION.md` (new contracts).
- `docs/USAGE.md`, `docs/package_layout.md`, `docs/WINDOWS_ACCEPTANCE.md`, this file.

## STATUS

IMPLEMENTATION COMPLETE — PLATFORM VALIDATION PENDING for output routing.
Documentation/source review only in this checkpoint. No C++ source, upstream pin,
package topology or test assertions changed. No local Windows files or remote
branches were deleted. The previous acceptance is not a new-layout or security pass.

## PUBLISHED

This coherent checkpoint; resolve with `git log -1 -- docs/ACTIVE_WORK.md`.
No follow-up commit solely to insert this commit's own SHA is required.

## VALIDATION

Complete touched files were fetched at BASE; locally reconstructed originals
match their Git blob SHAs. Scoped diff/whitespace and path-consistency checks
were performed. PowerShell/umk and Windows GUI execution are unavailable here;
no new runtime result is claimed. Prior migration evidence remains unchanged.

## NEXT ACTION

Gary: refresh main, read BUILD_AND_RUN.md and RELEASE_PREPARATION.md, update the
actual local assembly, validate new paths and Workbench, inventory safe cleanup,
then verify/update security-relevant dependency families with focused tests.
Finish with the retained Windows suite and explicit per-platform status.
Keep the optional Debug benchmark timeout and external-fixture gaps separate.
