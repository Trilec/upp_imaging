# Changelog

## Unreleased

### Local release preparation in progress

* Completed a clean intermediate-root Windows run of all 49 retained tests
  in Debug and Release: 2,746 checks and 98 clean exits. Workbench was
  manually accepted by Curt at that source checkpoint. The later security
  changes have focused tests; final clean acceptance and publication remain open.
* Exposed the registered WebP, TIFF, HDR, DPX, JPEG XL, AVIF, HEIC and other
  supported input families in Workbench. Small generated fixtures and retained
  upstream HEIF examples validate the expanded Open path; saving remains EXR/PNG.
  Limited Workbench's subimage inspection and Layers tree to 256 entries,
  showing when a file has more subimages.
* Updated the linked OpenEXR and OpenEXRCore source slices from 3.4.13 to
  the 3.4.14 security release and standalone OpenJPH from 0.26.3 to
  0.27.1, retaining explicit generated configuration.
  Limited OIIO read dimensions and decoded size and redirected the Windows
  U++ `plugin/z` provider to pinned zlib 1.3.2. Expat XML and remaining
  dependency/input-boundary review still block release clearance.
* Refreshed the bounded FFmpeg source and generated header family from
  9.0.1 to 9.0.2, including a reachable MOV metadata allocation bound and
  native H.264 corrections. Kept the scalar configuration and the required
  `chroma_pos_compat.c` integration function; the FFmpeg-focused matrix and
  ten additional first-frame runs passed.
* Verified the new U++ intermediate path and focused Debug/Release tests;
  added exact per-target check minima and stopped EXR tests writing to
  the legacy `out/` directory.
* Updated the linked Expat family from 2.7.2 to 2.8.5 with matching
  public/source headers, Windows configuration and a malformed UTF-16
  regression. Upstream still reports an unfixed denial of service, so
  security clearance remains pending.
* Updated the linked decode-only libheif family from 1.23.1 to the 1.23.5
  security release, retaining the verified dav1d/libde265 source slice and
  registry-lifetime overlay. HEIF/AVIF validation and the full suite remain
  release gates.
* Refreshed the OpenImageIO main, utility, public-header and plugin family
  from 3.1.15.0 to 3.1.17.0, including the Cineon/EXR and shared metadata
  fixes. Retained the separate MinGW main-thread error-storage wrappers and
  added repeated caller-thread malformed DPX/Cineon coverage. Full clean
  validation and Workbench acceptance remain release gates.
* Retired four reviewed recovery branches and the obsolete shutdown
  experiment branch; removed the inventoried generated `out/` tree after
  preserving validation evidence.

### Build-output convention and release preparation

* Redirected the assembly example to `build/windows-x64/umk` and the Windows validation runner to configuration-specific directories under `build/windows-x64/validation`.
* Reserved `bin/<platform>` for verified runnable Release applications; updated build, usage and Workbench documentation and ignored generated build/bin trees.
* Preserved the accepted migration evidence at `17bdbb3e326a07e71f159cb034c34534fb6f6bff`: 49 tests in each configuration, 2,734 checks and 98 clean exits. This is historical full-suite evidence.
* Defined release-preparation gates for safe local/branch cleanup, verified dependency-security refresh, Workbench checks and separately evidenced Linux/macOS portability. That documentation checkpoint did not change a dependency pin or C++ behavior.

### Repository structure simplification

* Reduced 177 source-root directories to eight logical areas and 174 packages to 124.
* Retained 49 deterministic tests and one manual benchmark; removed 41 obsolete or duplicate test/probe packages, compatibility aliases, the superseded viewer/helper route and a generated design executable.
* Migrated U++ nests, explicit source paths, submodules, fixtures, generators and documentation while preserving package names and upstream pins.
* Repaired workbench dependency/API/harness defects and clean-shutdown ownership for OCIO's LUT registry and MinGW OIIO error storage exposed by the retained tests.
* Added the authoritative acceptance manifest and reproducible Debug/Release runner under tests/ and tools/.

### Current-generation framework and package architecture

* Completed the backend-neutral `Upp::Imaging` framework: `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics`, and the `Imaging` umbrella.
* Preserved Core-only/GUI-independent dependency boundaries for `ImagingCore`, `ImagingAnalysis`, and `ImagingDiagnostics`; OpenImageIO/OpenColorIO remain private behind framework APIs.
* Established `OpenImageIO` and `OpenColorIO` as canonical public packages while retaining strict/source implementation boundaries underneath them.
* Kept `plugin/exr` opt-in and display-oriented rather than making it a full-fidelity EXR API or an automatic `Imaging` dependency.
* Kept FFmpeg as a separate media subsystem rather than expanding `ImagingIO` into a movie API.

### Original Windows framework baseline (historical)

These are the original bring-up totals, not the current acceptance requirements.
After hardening and migration, Core is 52/0, ImagingIO 89/0 and ImagingColor 69/0;
`docs/WINDOWS_ACCEPTANCE.md` records the retained 49-test matrix.

* ImagingCore accepted at 48/0.
* ImagingIO EXR/PNG baseline accepted at 79/0.
* ImagingColor accepted at 66/0 with independent OCIO 15/0.
* ImagingAnalysis accepted at 41/0.
* ImagingDiagnostics accepted at 33/0.
* Imaging umbrella accepted at 6/0.

### Still-image expansion

* Added code-side JPEG XL support with pinned libjxl and static OpenImageIO registration; prerequisite/backend acceptance is Windows-proven 9/0 Debug and 9/0 Release after the skcms linker repair `a66e1192025032823e93a890e16cc3874034a8a4`.
* Added Radiance HDR/RGBE support and DPX/Cineon format policy to ImagingIO.
* Added camera RAW input through pinned LibRaw/OpenImageIO integration.
* Added exact-lossless WebP support for the documented subset.
* Added decode-only HEIF/AVIF support with pinned decoder dependencies.
* Expanded TIFF/OpenImageIO support.
* Repaired shared static OpenImageIO plugin dependency ownership in `5ca436c3ba6265f6431deaf7348332940051686d`; the accumulated boundary subsequently passed Debug and Release, including the retained migration suite.
* Aligned `ImagingIO` package documentation with the implemented HEIF/AVIF and TIFF policies and replaced later-format roadmap shorthand with the exact 16-target repository-owned Debug/Release matrix and expected totals.
* Kept positive real-camera RAW decode, real 8/10-bit AVIF/HEIC decode and animated-WebP rejection as separately reported supplementary interoperability evidence because provenance-reviewed fixtures are not stored in the repository.

### `plugin/exr`

* Implemented the opt-in EXR `StreamRaster` / `Upp::Image` preview bridge.
* Expanded the focused contract through `323c3dc29938de404fc3411b87dcaf6c6aea4559` to cover GrayAlpha, non-finite-to-zero preview behaviour, single-channel masks, named multichannel RGB selection, straight alpha, opaque/alpha reporting, invalid/truncated input and fixture cleanup.
* The 22-check focused contract passed Debug and Release and remains in the accepted migration suite.

### FFmpeg first slice

* Pinned signed FFmpeg `n9.0.1` at exact commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
* Added explicit static source boundaries for `ffmpeg_headers`, scalar `ffmpeg_avutil_src`, native-H.264-only `ffmpeg_avcodec_src`, MOV/MP4 + local-file-only `ffmpeg_avformat_src`, scalar `ffmpeg_swscale_src`, and the stable direct `FFmpeg` package.
* Added deterministic one-frame 16x16 Constrained Baseline H.264 MP4 decode-to-RGBA acceptance coverage with fixed fixture and logical YUV evidence.
* Removed generated codec/parser/BSF/demuxer/muxer/protocol registry fragments from standalone header-package compilation while keeping them checked in as generated registries.
* Completed the Windows UCRT FFmpeg `MATH_FUNCS` capability set; `ffmpeg_avutil_test` Debug 13/0 is recorded green.
* Completed the generated `CONFIG_EXTRA` helper namespace and explicit disabled H.264 hardware-acceleration component family.
* Retained `HAVE_DIRECT_H 1` after Windows validation proved the direct.h path required by the selected avformat source.
* Added an eighth `ffmpeg_headers_test` preflight check that audits the four production source manifests and recursively reached pinned headers for missing `CONFIG_*`, `HAVE_*`, `ARCH_*`, and `AV_HAVE_*` definitions.
* Recorded `ffmpeg_avcodec_test` Debug 12/0 as green after the generated config repair.
* Closed the avformat linker ownership defect by adding pinned upstream `libavformat/to_upper4.c` (`ff_toupper4`) and `libavformat/mpegaudiotabs.c` (`ff_mpa_freq_tab`) to the explicit avformat manifest.
* Audited the selected avformat and swscale Makefile-to-manifest ownership closures; no further live first-slice materializer gap was found statically.
* Current expected first-slice gates are headers 8/0, avutil 13/0, avcodec 12/0, avformat 14/0, swscale 13/0, first-frame 27/0. The bounded Debug/Release and five-additional-runs-per-configuration repeatability gates passed at `f67e9a53b70be84cb5dd40586f59e1009c1724ab`; all six focused gates also passed the later migration suite.

### Documentation and recovery

* Reconciled README and architecture documentation with the implemented framework, still-image expansion, `plugin/exr`, and FFmpeg first-slice boundaries.
* Reconciled package catalogue, status/roadmap, package-layout guidance, and this changelog so implemented work is no longer described as planned.
* Formalized three distinct state labels: implemented, Windows-proven, and platform validation pending.
* Made repository test package names and expected totals the validation authority instead of format-name roadmap shorthand.
* `docs/ACTIVE_WORK.md` remains the recovery authority and records the exact published checkpoint, validation evidence, and next accumulated validator action.

### Deferred next scope

* FFmpeg SIMD/external assembly, hardware acceleration, broader codecs/containers, audio, seeking/indexing, a backend-neutral media wrapper, and waveform/vectorscope expansion remain separate enhancements, not unfinished requirements of the accepted bounded generation. Current release preparation prioritizes verified dependency security and accurately reported platform support.
