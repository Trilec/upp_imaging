# Windows trusted-local Debug delivery completion

Status: **PASS — 8/8 milestones, 100% of the agreed Windows Debug scope.**
Release publication remains **PARTIAL**. Curt explicitly selected trusted
local inputs with enforced restrictions on 2 October 2026; isolated hostile
decoding is outside this milestone. No Linux/macOS, sanitizer or fuzz execution
is claimed. Workbench manually accepted by Curt. The earlier human acceptance
does not establish the identity of the newly rebuilt executable.

## Source and changes

Base: `ed15a3aaf855e6100df87bda2701dc8555b91905`, verified against fetched
origin/main before publication. The commit containing this record is the
coherent completion checkpoint; find its exact SHA with
`git log -1 --format=%H -- docs/DELIVERY_COMPLETION_20261002.md`.

- `third_party/openimageio/openimageio_plugin_jpeg`, `integrations/plugin/imaging_jpeg`,
  `ImagingIO` and Workbench: standard JPEG through repository libjpeg-turbo,
  opt-in U++ raster registration, .jpg/.jpeg I/O policy and Open dialog entries.
- `ImagingCore/TrustedInput.h`, image/color loaders and OCIO preview: shared
  pre-parser checks for local regular files and image/config/LUT size limits.
- TIFF input overlay: native single/cumulative allocation limits on filename
  and IOProxy paths; pinned OIIO source remains unchanged.
- OCIO registry generator and Expat compatibility header: omit XML formats
  and reject native XML parser construction. Pinned OCIO source remains unchanged.
- Focused regressions, expected counts, package usage and security/status records.

Required positive image selection: JPEG, PNG, JPEG XL, EXR, DPX and Radiance HDR.
Additional retained coverage: WebP, TIFF, Cineon, AVIF/HEIC and synthetic linear
DNG. Video: H.264 MP4/MOV frames, timestamps, seeking and Workbench controls.
JPEG XR is distinct from EXR and deferred; arbitrary MOV/MP4 codecs, audio and
HDR video are not included. Full-fidelity EXR/HDR data and display preview are
separate contracts; see [FORMAT_QUICKSTART.md](FORMAT_QUICKSTART.md).

## Actual Windows validation

Final integrated command: `tools/validate.ps1 -Configuration debug`.
**51 targets, 1,512 checks, zero failures, 51 exits at 0.** Counts match the
51-entry acceptance/expected-count manifests. This was incremental Debug
acceptance, not a clean Release rebuild. Final changed targets include:
imaging_raster_test 66, imaging_workbench_ocio_test 168, imaging_color_test 71,
opencolorio_test 20, tiff_oiio_test 17; imaging_video_test retains 24.

Preserved byte-identical ledger:
`build/windows-x64/release/acceptance-debug-finish-20261002-results.txt`
SHA-256: `0e63e53fb618d1d3d09fe09496c719ba0b2143f8465b6772894eb254b8c244c0`.
Per-target build/stdout/stderr logs remain in `build/windows-x64/validation/debug/`.

The ledger header is pre-commit HEAD `ed15a3aaf855e6100df87bda2701dc8555b91905`;
the build used the modified worktree source published in this checkpoint.
Production source was unchanged during the complete successful run; subsequent
edits reconcile documentation. A retained normalized source manifest is under
`build/windows-x64/release/completion-source-20261002.json`; publication verifies
those source bytes against committed blobs without repeating the suite.

Focused checks repaired actual integration issues before acceptance. TIFF
limits use the existing recognized OIIO imagesize attribute; custom attributes
are not recognized by that provider. Oversized optional TIFF descriptions can
be skipped while image opening succeeds; default reads retain OIIO's 64 KiB
description truncation. OCIO policy is applied to its compiled generated
registry, rather than changing an unused upstream copy. The first integrated
attempt stopped at the first target's link, before any test ran; ordering the
Core include before OIIO's LoadImage macro undefinition repaired the ABI.
The OCIO Debug dependency rebuild and generator --check passed. No failed
focused or aborted run is counted as acceptance.

## Runnable artifact and cleanup

The actual GUI Debug application rebuilt successfully with CLANGx64 and +GUI:
`build/windows-x64/apps/debug/ImagingWorkbench.exe`.
The existing launcher path `build/ImagingWorkbench_debug.exe` contains the same
bytes. Both SHA-256:
`7650190c4df68bdb57d1e18514097bf67253f3cbadc4f55997c2a8f792aa9c51`.
Build log: `build/finish-workbench-build.log`. No fresh manual GUI acceptance
or Release identity is inferred from compilation.

Only four temporary executables created during this focused repair were
removed after preserving their size/hash inventory at
`build/windows-x64/release/focused-temp-cleanup-20261002.json`. Final suite
executables/logs, manual fixtures and earlier evidence remain. TIFF generated
archives were rebuilt after the input source replacement; source was untouched.
Get-Process found no running Workbench during the fresh check; WMI enumeration
was denied, so no process was terminated from an old PID. The previously
removed out/ tree and reviewed obsolete remote branches remain retired; no
new branch was created and no shutdown experiment was restored.

## Security disposition and later work

[INPUT_POLICY.md](INPUT_POLICY.md) states enforced restrictions and accepted
native scratch/metadata, config-reference and file-race limitations.
[DEPENDENCY_REVIEW_20261002.md](DEPENDENCY_REVIEW_20261002.md) closes the bounded
family/configuration disposition review, without claiming an exhaustive audit.
The skcms remote lookup was unavailable; independent advisory coverage remains
future work. The raw Expat issue is unresolved upstream, while OCIO XML input
is disabled here. Earlier video-probing, ZIP extraction, PNG, zlib and
OpenEXR/OpenJPH repairs are retained and covered by the final Debug suite.

Next action is ordinary trusted-input use or a separately planned Release
publication checkpoint. `bin/windows-x64/ImagingWorkbench.exe` has not been
populated from this Debug build. Release validation/publication, platform
validation and hostile-input isolation remain separate work.
