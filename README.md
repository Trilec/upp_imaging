# upp_imaging

A U++ imaging framework with pinned native image libraries, an optional EXR raster integration, an interactive workbench, and a separate bounded FFmpeg decoder.

## Choose an API

| Need | Package |
| --- | --- |
| Typed image data and metadata | `ImagingCore` |
| Full-fidelity image loading and transactional saving | `ImagingIO` |
| Color configuration and transforms | `ImagingColor` |
| Statistics and histograms | `ImagingAnalysis` |
| Comparisons and structured diagnostics | `ImagingDiagnostics` |
| Framework umbrella | `Imaging` |
| Direct native APIs | `OpenImageIO`, `OpenColorIO`, `openexr`, `openexr_core`, and the public codec packages |
| Display-oriented U++ StreamRaster bridge | `plugin/exr` |
| Bounded H.264/MP4 decode-to-RGBA | `FFmpeg` |

ImagingIO supports EXR, PNG, JPEG XL, HDR/RGBE, DPX/Cineon, camera RAW, WebP, HEIF/AVIF and TIFF within each format's declared policy. RAW, HEIF/AVIF and Cineon are input-only. JPEG remains available through the direct codec API. Framework public headers do not expose OpenImageIO or OpenColorIO types.

## Browse and build

Our framework is in [imaging](imaging/), native dependencies in [third_party](third_party/), U++ adapters in [integrations](integrations/), and ImagingWorkbench in [apps](apps/). Tests, examples, tools and documentation have their own directories.

1. Initialize the pinned submodules with `git submodule update --init --recursive`.
2. Configure the active `GitHubOut` assembly from [GitHubOut.var.example](GitHubOut.var.example), using local absolute nest paths and an absolute `OUTPUT` path ending in `build/windows-x64/umk`. Existing local assembly files are not updated by pulling Git.
3. On Windows, build/run a focused test with `powershell -ExecutionPolicy Bypass -File tools/validate.ps1 -Package imaging_io_test`. Omit `-Package` for the retained Debug/Release suite; use `-Umk` to override the builder path.
4. Follow [build and run](docs/BUILD_AND_RUN.md) to stage ImagingWorkbench under `build/` and publish a verified Release executable to `bin/windows-x64/ImagingWorkbench.exe`.

See [layout](docs/package_layout.md), [package catalogue](docs/PACKAGE_CATALOGUE.md), [usage](docs/USAGE.md), and [acceptance](docs/WINDOWS_ACCEPTANCE.md). ImagingWorkbench is the primary interactive integration check; deterministic tests remain the correctness authority. Compiler/test/log output belongs under `build/<platform>/`; `bin/<platform>/` contains only verified runnable Release applications and required runtime payloads.

The FFmpeg stack remains pinned to `n9.0.2`, scalar/static, native H.264 decode, MOV/MP4 demux, local-file protocol and swscale. Networking, external codecs, hardware acceleration, encoding and other media subsystems remain disabled.

[THIRD_PARTY.md](THIRD_PARTY.md) records provenance and pins; [LICENSES.md](LICENSES.md) lists licenses. The current [security review](docs/THIRD_PARTY_SECURITY.md) and [portability matrix](docs/PORTABILITY_MATRIX.md) record unresolved release gates. [STRUCTURE_MIGRATION.md](docs/STRUCTURE_MIGRATION.md) records every test cleanup decision.

## Release readiness

The last recorded full Windows acceptance is the structure-migration checkpoint `17bdbb3e326a07e71f159cb034c34534fb6f6bff`: 49 tests per configuration, 2,734 checks across Debug/Release, all exits 0. Focused tests now verify the new build output routing. A clean full run, Workbench manual checks, dependency-security clearance and Linux/macOS runtime acceptance remain pending. See [release preparation](docs/RELEASE_PREPARATION.md) and [active work](docs/ACTIVE_WORK.md) for the current evidence.
