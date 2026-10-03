# Windows build and run

Version 1 is validated on Windows x64 with U++ CLANGx64. Linux/macOS and other
toolchains remain unvalidated; Windows-generated dependency configurations
must not be treated as portable.

## Assembly

Initialize pinned submodules with `git submodule update --init --recursive`.
Copy [GitHubOut.var.example](../GitHubOut.var.example) to local `GitHubOut.var`.
Replace repository, external Ui/Animation and U++ paths with your local absolute
paths. Add the listed nest directories, not individual packages. The repository
root is not a nest. An existing local assembly is not changed by a Git pull.
Set OUTPUT to `<checkout>/build/windows-x64/umk`.

## Applications

Run from the repository root, adjusting the builder path:

```powershell
$umk = 'E:/upp-18468/umk.exe'
New-Item -ItemType Directory -Force build/windows-x64/apps/release | Out-Null
& $umk GitHubOut ImagingWorkbench CLANGx64 -rH8 +GUI build/windows-x64/apps/release/ImagingWorkbench.exe
& $umk GitHubOut ImagingPluginDemo CLANGx64 -rH8 +GUI build/windows-x64/apps/release/ImagingPluginDemo.exe
```

Require successful exit codes before using the outputs. For Debug, omit `-r`
and stage under `build/windows-x64/apps/debug`. Workbench `--smoke` runs the
normal GUI loop and closes after one second for a bounded startup check.
The demo's interactive code pane shows the exact selected plugin API.

## Validation

```powershell
./tools/validate.ps1 -Configuration debug -Package imaging_raster_test,imaging_workbench_ocio_test
./tools/validate.ps1 -Configuration debug
```

The complete Debug command runs the retained manifest in `tests/acceptance.txt`.
Minimum check counts are in `tests/expected_counts.txt`. The runner rejects failed
builds, stale executables, malformed/reduced summaries, failed checks, nonzero
exits and timeouts. It writes logs and results under build/windows-x64/validation.
`-Configuration release` selects Release. Use focused checks for changed paths
and a complete block at a substantive checkpoint; documentation-only changes
do not require rebuilding applications. `-Rebuild` does not guarantee that every
independent package cache is clean; use a separate empty cache for a clean-build claim.

## Output folders and publication

| Folder | Contents |
| --- | --- |
| build/windows-x64/umk | Compiler intermediates |
| build/windows-x64/validation | Test executables, logs and ledger |
| build/windows-x64/apps/debug | Debug applications |
| build/windows-x64/apps/release | Release staging |
| build/windows-x64/release | Local package, hashes and build evidence |
| build/windows-x64/samples | Generated small manual-load fixtures |
| bin/windows-x64 | Verified Release apps, usage docs, samples and licence notices |

Close running applications before replacement. Publish only successfully built
and checked candidates; compare staged/published SHA-256 hashes. Keep test
executables, logs, symbols and intermediates out of bin. User images/configs
are not disposable build output. See [release information](V1_RELEASE.md).
