# Build output and runnable applications

## Status

The assembly example and Windows validation runner now target `build/` instead
of `out/`. Their local Windows verification is pending. The last accepted run
is recorded in [WINDOWS_ACCEPTANCE.md](WINDOWS_ACCEPTANCE.md) at `17bdbb3`.
A Git pull does not edit an existing local `.var` file, relocate old artifacts,
redirect application runtime logs, or populate `bin/`.

## Directory contract

```text
build/
  windows-x64/
    umk/                    # U++ package/method/configuration intermediates
    validation/
      debug/                # Debug test executables and their logs
      release/              # Release test executables and their logs
      results.txt
    apps/
      debug/                # Debug applications, never the release bin
      release/              # Release staging before validation/publication
    runtime/                # development smoke logs/scratch, not user images
    release/                # build manifest, hashes and packaged-release evidence
bin/
  windows-x64/
    ImagingWorkbench.exe    # verified Release build only
```

`build/` is reproducible generated output, not source. `bin/` is the launch
location, not a dump of every linked executable: no tests, object/static library
files, compiler logs, temporary fixtures or old application versions. Necessary
runtime libraries/resources may accompany an application; do not remove them
merely to obtain an exe-only listing. Keep debug symbols in `build/` and record
their relationship to the released binary.

Use `linux-x64`, `macos-arm64` and, when actually supported, `macos-x64` as other
platform directory names. Naming a directory is not evidence of platform support.
A macOS GUI release may be an `.app` bundle rather than a bare executable.

## Active U++ assembly

Retain the `GitHubOut` assembly name. Configure its actual local `.var` file
(the one TheIDE/umk resolves) with the nests in `GitHubOut.var.example`.
Use local absolute paths. For the current Windows checkout:

```text
OUTPUT = "E:/apps/github/upp_imaging/build/windows-x64/umk";
```

The repository example is a template, not a portable machine configuration.
U++ retains its own package/build-method/flag subdirectories under `OUTPUT`;
do not flatten these or reuse objects across incompatible toolchains. If another
compiler/ABI is added, isolate its build root as well.
The explicit final target passed to umk is separate from this intermediate root.

## Windows tests

Run from the repository root with the configured local assembly:

```powershell
powershell -ExecutionPolicy Bypass -File tools/validate.ps1 -Umk E:/upp-18468/umk.exe -Package imaging_core_test
```

For an array of package names, invoke the script directly from PowerShell instead:

```powershell
./tools/validate.ps1 -Umk E:/upp-18468/umk.exe -Package imaging_core_test,imaging_io_test
./tools/validate.ps1 -Umk E:/upp-18468/umk.exe -Rebuild
```

The second direct invocation runs the manifest suite in Debug and Release.
`-Rebuild` cleans the first selected target and its dependency closure in each
configuration; it is not a guarantee that every independent package cache was
cleaned. For final release evidence, use a new empty intermediate root or a
reviewed cleanup of the entire relevant cache. Preserve the existing source
manifests and required tests; no fresh matrix is needed after every small edit.

## Workbench staging and publication

Example Windows staging commands, from a PowerShell session at repository root:

```powershell
$umk = 'E:/upp-18468/umk.exe'
New-Item -ItemType Directory -Force build/windows-x64/apps/debug,build/windows-x64/apps/release | Out-Null
& $umk GitHubOut ImagingWorkbench CLANGx64 -H8 build/windows-x64/apps/debug/ImagingWorkbench.exe
if ($LASTEXITCODE -ne 0) { throw 'Workbench Debug build failed' }
& $umk GitHubOut ImagingWorkbench CLANGx64 -rH8 build/windows-x64/apps/release/ImagingWorkbench.exe
if ($LASTEXITCODE -ne 0) { throw 'Workbench Release build failed' }
```

Smoke-test the newly built staging executable, then publish only after the
required tests and clean shutdown pass:

```powershell
New-Item -ItemType Directory -Force bin/windows-x64 | Out-Null
Copy-Item build/windows-x64/apps/release/ImagingWorkbench.exe bin/windows-x64/ImagingWorkbench.exe -Force
Get-FileHash bin/windows-x64/ImagingWorkbench.exe -Algorithm SHA256
```

Close running Workbench instances before replacement. A failed build must not
publish or validate an older executable. Verify the staged and published hashes,
record source SHA/toolchain/configuration under `build/windows-x64/release/`,
and launch the published executable from both the repository and another working
directory. Do not assume an empty stderr stream means every check passed.

The local release pass must verify that Workbench logging, settings and generated
images do not pollute `bin/`. Use documented development runtime paths or the
platform's user-data locations; do not redirect genuine user files into disposable
build output. No C++ runtime-path change is included in this documentation checkpoint.

## Legacy artifact cleanup

Inventory `out/`, old executable copies and known generated directories first.
Preserve any useful diagnostic evidence before removing only reproducible
artifacts. Do not follow junctions/symlinks outside the repository, delete user
images/configs, clean submodule source trees, or run `git clean -xfd` across the
checkout. Retained test source is not obsolete just because its last executable
is old. Historical reports keep their original log paths clearly labelled.

`out/` remains ignored during migration. Once local commands, IDE settings and
runtime paths no longer recreate it, remove its known generated contents.
