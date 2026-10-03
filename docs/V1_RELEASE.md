# U++ Imaging 1.0.0 — Windows release

**PASS — version 1 Windows trusted-local delivery and local Release publication.**
The six version 1 delivery tasks are complete. This scope includes the basic
image formats, silent H.264 MP4/MOV, documented restrictions, the reusable U++
plugins, Workbench and a concrete UiMediaCard provider demo.

## Applications and format support

The authoritative Workbench source is `apps/ImagingWorkbench`; `tools/` contains
build/validation utilities. Staged and published executables are generated
copies of that app, not separate implementations. Windows version metadata,
title and Help identify version 1.0.0. Help is available with F1.

Open has separate choices for JPEG, PNG, JPEG XL, EXR, Radiance HDR, DPX/Cineon,
TIFF, WebP, AVIF/HEIF, camera RAW/DNG and H.264 MP4/MOV. The filename/container
alone does not guarantee a supported encoding: MOV/MP4 requires H.264, and RAW
coverage includes synthetic linear DNG rather than every camera variant.

Save offers EXR, PNG, JPEG, JPEG XL, TIFF, WebP, HDR and DPX. EXR retains original
channels. Other formats export the selected source RGB/RGBA group at full
resolution; display exposure/gamma/OCIO is not baked in. JPEG/WebP output is
8-bit, JPEG is lossy, DPX is 16-bit, and JPEG/HDR/DPX reject alpha explicitly.
Video saving exports the current still frame. Cineon, RAW and HEIF are input-only.

ImagingPluginDemo uses the real UiMediaCard control with six explicit raster
readers, live C++ and Copy C++. Preview and generated code share the selected
format, path and fit. It complements the full UiMediaCard styling demo in
upp_Ui. See [FORMAT_QUICKSTART.md](FORMAT_QUICKSTART.md) for exact package/header
names and [the executable example](../apps/ImagingPluginDemo/README.md).

## Validation and compiled source

- Integrated incremental Debug: **51 targets, 1,527 checks, no failures, 51 exits 0**.
- Focused Release Workbench: **183 checks, no failures, exit 0**, including new exports and H.264 MP4/MOV.
- Both actual Release applications compiled with GitHubOut / CLANGx64 / +GUI.
- Actual generated C++ for JPEG, PNG, JXL, EXR, HDR and DPX compiled unchanged; each decoded its fixture and initialized UiMediaCard through `--check`, exit 0.
- Release Workbench `--smoke` exercises construction, the normal GUI event loop and timed Close; startup/closure passed from the repository and a separate working directory. No full GUI checklist was repeated.

Workbench manually accepted by Curt. That historical human confirmation does
not establish either new executable's exact identity or a new visual review.
The final main.cpp change adds only the bounded `--smoke` startup/closure path;
the final executable was rebuilt and exercised after this change.

The preserved ledger is
`build/windows-x64/release/acceptance-v1-debug-20261003-results.txt`, SHA-256
`0fac52196ff698afc426528c214e51bdcce1abfd31e8a4527615e0c75f808b35`.
Its header identifies pre-commit main
`70acf01d351058a7d0598ec088bac09321cd4bea`; its compiled worktree includes the
changes published with this document. The ledger is not evidence that the
older HEAD alone contains those changes. The normalized source manifest
`build/windows-x64/release/v1-source-manifest.json` is checked against published
Git blobs, preserving the relationship without rerunning after a commit.
The external Ui checkout is `877d60b87d2dbd171c5785019a0c192f95aca0c4`.
A full new Release suite was not run; the efficient final block uses the full
Debug suite plus the focused Release test and actual Release app builds.

## Local artifacts

| Artifact | SHA-256 |
| --- | --- |
| `bin/windows-x64/ImagingWorkbench.exe` | `613411391b84d1e60e21490a27c046441f5f4c22fbb1047c20aef50176e50f0f` |
| `bin/windows-x64/ImagingPluginDemo.exe` | `8f7d7b6f7bff639cecc9915b3c77bb30acc4c92e6fedbf357cf430e38d4a7c92` |

Published executables match the staged candidates byte for byte. The local
Discord-ready archive is
`build/windows-x64/release/upp-imaging-1.0.0-windows-x64.zip`; its accompanying
SHA-256 file identifies the package. It contains both apps, small fixtures,
usage/input-policy docs, build identity, repository/U++/Ui licence notices and
retained third-party notices. This is local publication, not a Discord upload
or a GitHub Release asset. Compiler output and test logs remain under build.

Old root build logs are preserved in `pre-v1-root-logs.zip`. Only explicitly
inventoried obsolete generated executables/symbols were removed; source tests,
compiler caches, current acceptance logs and historical ledgers remain.
`v1-cleanup-inventory.json` records paths and hashes. No obsolete Workbench was
still running when the old staging app was replaced. No branches were created.

## Support boundary

[INPUT_POLICY.md](INPUT_POLICY.md) remains authoritative. OCIO XML readers are
disabled to avoid the known Expat path. Native scratch/metadata and incomplete
independent skcms advisory coverage remain documented residual risks within
the accepted trusted-input scope. No new dependency-security clearance beyond
that scope is claimed. JPEG XR, audio, other video codecs, HDR video, decoder
isolation, Linux/macOS and sanitizer/fuzz validation remain deferred. They do
not count as completed features. See THIRD_PARTY_SECURITY.md for dispositions.
