# Maintaining Imaging

This is the single continuation guide for contributors and future agents.
Current user support is in [FORMATS.md](FORMATS.md); version 1 targets Windows
x64 and trusted stable local inputs. Keep capability claims within that scope.

## Repository layout

| Folder | Purpose |
| --- | --- |
| imaging | ImagingCore, IO, Color, Analysis, Diagnostics, umbrella and separate Video packages |
| integrations | ImagingRaster, opt-in plugin packages, preview/geometry/histogram helpers and fixtures |
| third_party/openimageio, opencolorio, openexr, imath | Native API/header/source owners |
| third_party/codecs, support, ffmpeg | Codecs/compression, utilities and the separate bounded video stack |
| apps | Authoritative Workbench and PluginDemo application source |
| tests | Deterministic contract packages, acceptance manifest and minimum counts |
| examples | Small usage examples |
| tools | Validation runner, config generator and manual benchmark |
| docs | Current usage, support, architecture and this guide; images contains screenshots |
| build / bin | Ignored generated evidence/staging and verified Release payload, respectively |

Package names are independent of nests. [GitHubOut.var.example](../GitHubOut.var.example)
lists assembly roots; local GitHubOut.var is machine-specific. `apps` is the source
home for applications. Copies under build/bin are outputs, not second implementations.

## Adding an image format

1. Establish upstream provenance, exact source version/hash and retained licences.
   Use a public native package backed by explicit source/header owners; do not
   invent a substitute upstream API or add every source file through a glob.
2. Add the OIIO format registration and declared ImagingIO sample/channel policy.
   Decide separately whether reading, writing and U++ preview are supported.
3. Add an opt-in plugin under integrations/plugin using the shared ImagingRaster
   limits and explicit reader. Keep RGBA8 display conversion separate from typed
   HDR data and preserve existing failure/ownership semantics.
4. Add a small positive fixture and appropriate rejection/round-trip coverage to
   existing family blocks. Register a new test package/count only when needed.
5. Update WorkbenchFormats.h and export validation when the app supports it;
   keep alpha/quantization restrictions explicit. Update the demo/generator when
   adding a demonstrated plugin. Compile its generated C++ unchanged.
6. Update FORMATS.md, usage, package README and dependency provenance. Use focused
   regressions first, then one integrated block at the release checkpoint.

## Dependencies and safety

Keep upstream changes separate from repository-owned wrappers/configuration.
Record required local adaptations in the package README and THIRD_PARTY.md.
Preserve copyright/licence notices and release provenance. Refresh generated
version/capability headers against the actual source; stale macro values are
not a substitute for inspecting what compiles. FFmpeg changes also require its
source/config parity test and the affected decode/timing/seek block.

Windows Core's compression provider and standalone zlib/libpng source routes are
intentional. Preserve OIIO error-lifetime and HEIF/OCIO teardown adaptations until
the pinned upstream implementation genuinely supersedes them. Preserve test
process exit checks: printed passing counts do not excuse a shutdown crash.

OCIO XML readers are disabled to avoid the known Expat parsing risk. Do not
re-enable CTF/CLF/CDL/CC/CCC/look merely because Expat builds. Native scratch,
metadata, ICC processing and config-referenced resources remain within the
trusted-input boundary. Independent skcms advisory coverage was incomplete;
do not turn that into a clean bill of health. Exact pins remain in THIRD_PARTY.md
and package source manifests; enforced policies remain in INPUT_POLICY.md.

## Deferred work

| Area | What must be resolved before claiming support |
| --- | --- |
| Linux/macOS | Target-generated dependency configs, toolchain/assembly, case-sensitive includes, actual build/runtime and shutdown checks |
| Audio / more video codecs / HDR video | Explicit feature/config, buffering/timing/playback contract and representative tests |
| JPEG XR | A justified provider, format policy, plugin/fixture and integration coverage |
| OCIO XML | Applicability/fix or isolation decision plus bounded parsing tests |
| Hostile files | Decoder/process isolation and stronger whole-process resource guarantees |
| Sanitizers/fuzzing | Actual executions and reproducible harness/results, rather than source-review claims |
| Performance | Profile first; possible prepared OCIO processor caching and bounded EXR tile/chunk previews must preserve current semantics |

Seeking already exists in ImagingVideo; it is not an unfinished feature.
For rebuild/publication use [BUILD_AND_RUN.md](BUILD_AND_RUN.md). Keep the public
docs current; routine work diaries, broad programming guides and review reports
do not belong here. Local diagnostic evidence belongs under ignored build.
