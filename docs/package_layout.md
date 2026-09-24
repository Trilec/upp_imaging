# Repository layout

The repository contains several U++ nests. Package identities are independent of their physical nest: applications still use `ImagingIO`, `OpenImageIO`, `OpenColorIO`, and `plugin/exr`.

| Directory | Ownership |
| --- | --- |
| `imaging/` | Backend-neutral Imaging, Core, IO, Color, Analysis and Diagnostics packages. |
| `third_party/openimageio/` | OpenImageIO API, headers, source owners and format registration. |
| `third_party/opencolorio/` | Canonical OpenColorIO API and implementation. |
| `third_party/openexr/` | High-level OpenEXR, EXRCore, Iex and IlmThread. |
| `third_party/imath/` | Imath public headers and implementation. |
| `third_party/ffmpeg/` | Pinned bounded FFmpeg decode stack and generated configuration. |
| `third_party/codecs/` | Image codecs and compression dependencies. |
| `third_party/support/` | Formatting, maps, XML/YAML, strings and ZIP dependencies. |
| `integrations/` | U++ EXR raster plugin and workbench preview, histogram, geometry and fixture support. |
| `tests/` | Deterministic current-contract regressions; `acceptance.txt` is authoritative. |
| `apps/` | ImagingWorkbench interactive application. |
| `examples/` | Small usage examples; no additional U++ nest is required. |
| `tools/` | Validation runner, OCIO generator and manual workbench benchmark package. |
| `docs/` | Architecture, provenance, guides, design assets and acceptance evidence. |

Copy `GitHubOut.var.example` to `GitHubOut.var` and replace its repository and external U++ paths with local absolute paths. Add nest directories, never each individual package. The repository root is not a nest. `examples/` and `docs/` contain no build packages and are omitted from the assembly.

Explicit source manifests and import lists remain authoritative. `_src` packages provide source ownership and build isolation; public packages provide include and link policy. In particular, public zlib/libpng routes avoid duplicate zlib symbols when U++ Core links plugin/z. They are functional build boundaries.

The Imath and OpenEXR public packages export their own include directories for bare upstream header names. No root-level implementation header is required. Submodules live with their source owners; `.gitmodules` records their current physical paths while retaining their original module identities.

Disposable compiler output, test executables, validation logs and staging belong under ignored `build/<platform>/`. Only verified runnable Release applications and required runtime payloads belong under ignored `bin/<platform>/`. Neither directory is a U++ source nest. The old `out/` tree is legacy local output, not an active build destination. See [build and run](BUILD_AND_RUN.md) for assembly setup, exact paths and migration status.
