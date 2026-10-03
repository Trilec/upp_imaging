# Version 1.0.0 — Windows release

U++ Imaging version 1 supports Windows x64 with trusted local inputs.
The release applications are ImagingWorkbench and ImagingPluginDemo.
See [supported formats](FORMATS.md), [U++ usage](FORMAT_QUICKSTART.md) and
[input restrictions](INPUT_POLICY.md).

## Applications

ImagingWorkbench provides image/channel viewing, exposure/gamma/OCIO preview,
histograms, eight still-image export choices and silent H.264 MP4/MOV playback.
Help is available with F1. ImagingPluginDemo demonstrates six concrete raster
plugins loading into UiMediaCard, with matching copyable C++.

Build source lives under apps; verified local Release applications are published
under bin/windows-x64. The local package is
build/windows-x64/release/upp-imaging-1.0.0-windows-x64.zip, containing both apps,
small samples, usage/input-policy docs, build identity and licence notices.
Pulling Git does not download or populate ignored local binaries.

| Executable | SHA-256 |
| --- | --- |
| ImagingWorkbench.exe | 613411391b84d1e60e21490a27c046441f5f4c22fbb1047c20aef50176e50f0f |
| ImagingPluginDemo.exe | 8f7d7b6f7bff639cecc9915b3c77bb30acc4c92e6fedbf357cf430e38d4a7c92 |

## Windows validation

Release source checkpoint: `04ec3626faadf090c5d2a7b20d6895393b422e12`.
Toolchain: U++ CLANGx64 / GitHubOut / +GUI.

- Complete incremental Debug block: 51 targets, 1,527 checks, no failures, 51 exits 0.
- Focused Release Workbench: 183 checks, no failures, exit 0.
- Both actual Release apps compiled; all six generated plugin examples compiled unchanged and loaded their fixtures.
- Published Workbench bounded startup/event-loop/normal-close checks passed from two working directories.

A full new Release suite was not run. Workbench was manually accepted by Curt;
that earlier confirmation does not identify a newly built executable's exact hash.
Local build identities and detailed ledgers remain under build/windows-x64/release.
The acceptance ledger's pre-commit HEAD is distinct from its tested worktree;
the normalized source manifest was verified against the published Git blobs.

Linux/macOS, sanitizer/fuzz execution and hostile-input isolation remain
unvalidated or deferred. Audio, other video codecs, HDR video and JPEG XR are
not supported in version 1. Native decoder restrictions remain in INPUT_POLICY.md.
