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
2. Copy [GitHubOut.var.example](GitHubOut.var.example) to `GitHubOut.var` and set local absolute nest paths, including your U++ installation and `upp_Ui` for the workbench.
3. Build, for example, `umk GitHubOut imaging_io_test CLANGx64 -H8 out/imaging_io_test.exe`.
4. Run the current Debug/Release acceptance set with `./tools/validate.ps1 -Umk /path/to/umk.exe`.

See [layout](docs/package_layout.md), [package catalogue](docs/PACKAGE_CATALOGUE.md), [usage](docs/USAGE.md), and [acceptance](docs/WINDOWS_ACCEPTANCE.md). The workbench is supplementary diagnostics; deterministic tests are the correctness authority.

The FFmpeg stack remains pinned to `n9.0.1`, scalar/static, native H.264 decode, MOV/MP4 demux, local-file protocol and swscale. Networking, external codecs, hardware acceleration, encoding and other media subsystems remain disabled.

[THIRD_PARTY.md](THIRD_PARTY.md) records provenance and pins; [LICENSES.md](LICENSES.md) lists licenses. [STRUCTURE_MIGRATION.md](docs/STRUCTURE_MIGRATION.md) records every test cleanup decision.
