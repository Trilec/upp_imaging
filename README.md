# upp_imaging

## Purpose

`upp_imaging` is a U++ imaging nest that provides pinned third-party packages, strict source validation, a backend-neutral `Upp::Imaging` framework, opt-in U++ raster integration, and a separately bounded FFmpeg media stack.

The repository deliberately distinguishes three states:

- **implemented** — the package/source/test contract exists and has passed source/static review;
- **Windows-proven** — the relevant U++ CLANGx64 Debug/Release acceptance has been recorded;
- **platform validation pending** — implementation exists, but the current accumulated Windows checkpoint has not yet been accepted.

Do not infer platform acceptance from implementation alone. `docs/ACTIVE_WORK.md` is the recovery authority for the exact accepted checkpoint and evidence.

## Supported use cases

### 1. Direct upstream-style API access

Applications that want established native APIs directly use the stable public packages, including:

- `openexr` / `openexr_core`
- `OpenImageIO`
- `OpenColorIO`
- `imath`
- `libpng`
- `libjpeg_turbo`
- `libtiff`
- `libdeflate`
- `openjph`
- `fmt`
- `robinmap`

`oiio` remains a compatibility forwarder to `OpenImageIO`. Strict `_src`, generated-header, source-routing and static-registration packages are implementation details and are not ordinary application dependencies.

### 2. Backend-neutral U++ Imaging framework

Applications that want U++-native image contracts use the implemented `Upp::Imaging` framework:

- `ImagingCore` — backend-neutral image data, metadata, window, result and diagnostic contracts; Core-only.
- `ImagingIO` — full-fidelity typed image loading/saving through private backend adapters.
- `ImagingColor` — backend-neutral colour processing with OpenColorIO private behind the public boundary.
- `ImagingAnalysis` — Core-only statistics, histograms and source probes.
- `ImagingDiagnostics` — Core-only deterministic comparison and structured reporting.
- `Imaging` — convenience umbrella over the five framework packages.

Public framework headers expose only `Upp::Imaging` contracts; OIIO and OCIO types remain private implementation details.

The established Windows framework baseline is accepted at:

- ImagingCore **48/0**
- ImagingIO **79/0**
- ImagingColor **66/0** plus independent OCIO **15/0**
- ImagingAnalysis **41/0**
- ImagingDiagnostics **33/0**
- Imaging umbrella **6/0**

### 3. Optional U++ raster integration

`plugin/exr` is implemented as an opt-in display-oriented `StreamRaster` / `Upp::Image` bridge. It is deliberately separate from the full-fidelity ImagingIO and direct OpenEXR/OpenImageIO paths.

Its current focused contract covers ordinary single-image EXR preview, RGB/RGBA, Gray/GrayAlpha, one-channel masks, named multichannel RGB selection, straight alpha, deterministic finite-value clamp/rounding, non-finite-to-zero preview behaviour, and truthful opaque/alpha reporting. The expanded **22-check** contract is Windows-proven in Debug and Release.

### 4. Separate FFmpeg media subsystem

FFmpeg is not part of ImagingIO or the `Imaging` umbrella. The current first slice is an intentionally small static LGPL scalar decode stack:

- exact signed FFmpeg `n9.0.1` pin;
- `ffmpeg_headers` generated/public configuration boundary;
- `ffmpeg_avutil_src`;
- native H.264-only `ffmpeg_avcodec_src`;
- MOV/MP4 demux + local-file-only `ffmpeg_avformat_src`;
- scalar `ffmpeg_swscale_src`;
- stable direct `FFmpeg` package;
- deterministic one-frame H.264/MP4 decode-to-RGBA acceptance test.

Threads, network protocols, external codecs, filters, devices, audio resampling, CLI/encoding, external/inline assembly and hardware acceleration remain disabled in this first slice.

Implementation/source ownership and accumulated Windows Debug/Release acceptance are closed. All six focused gates are Windows-proven in both configurations, and five additional first-frame runs per configuration passed `27/0` with clean exits. See `docs/FFMPEG_PLAN.md` and `docs/ACTIVE_WORK.md` for the exact evidence.

## Current format line

The repository now contains the completed code-side still-image expansion for:

- OpenEXR and PNG baseline;
- JPEG XL;
- HDR/RGBE;
- DPX/Cineon;
- camera RAW;
- WebP;
- HEIF/AVIF decode-only;
- TIFF expansion.

The complete 16-gate still-image matrix is Windows-proven in Debug and Release with the exact totals in `docs/WINDOWS_ACCEPTANCE.md`. This includes the shared OpenImageIO/ImagingIO boundary and JPEG XL, HDR/RGBE, DPX/Cineon, RAW, WebP, HEIF/AVIF and TIFF gates.

Narrow direct helpers such as `openexr_io`, `png_io`, `jpeg_io` and `tiff_io` remain intentionally narrower than the full framework/backend surfaces.

## Engineering and validation model

- Remote GitHub `main` is authoritative for published state.
- Pinned upstream source and explicit U++ manifests are preferred over system-library assumptions or recursive source globs.
- Public packages own stable application boundaries; `_src` and registration packages stay internal.
- Backend-neutral framework APIs do not leak OIIO/OCIO types.
- Tests are the formal pass/fail authority; `ImagingWorkbench` is supplementary full-stack diagnostics.
- New implementation is grouped into coherent source/dependency/test slices, then validated through accumulated matrices.
- `docs/ACTIVE_WORK.md` records the current base, touched paths, publication state, validation evidence and exact next action so work can recover after a session outage.

## Current closure milestone

The current bounded generation is complete: the 16-target still-image matrix, expanded `plugin/exr` contract, FFmpeg six-gate matrix and FFmpeg repeatability evidence are all Windows-proven in Debug and Release. Provenance-reviewed real-file RAW/DNG, AVIF/HEIC and animated-WebP fixtures remain optional supplementary evidence and are not stored in the repository.

SIMD/hardware acceleration, broader FFmpeg codecs/containers, audio, seeking/indexing, waveform/vectorscope expansion and a possible backend-neutral media wrapper are **next-scope enhancements**, not unfinished requirements of the current closure milestone.

## Choosing a package

| Need | Use |
| --- | --- |
| Direct native EXR/OIIO/OCIO/codec access | the corresponding stable direct package |
| Backend-neutral typed image model | `ImagingCore` |
| Backend-neutral image load/save | `ImagingIO` |
| Backend-neutral colour processing | `ImagingColor` |
| Histograms, statistics and probes | `ImagingAnalysis` |
| Deterministic comparison/reporting | `ImagingDiagnostics` |
| Standard complete imaging framework | `Imaging` |
| EXR preview in ordinary `Upp::Image` workflows | `plugin/exr` |
| Direct bounded FFmpeg API stack | `FFmpeg` |
| Full-stack visual/diagnostic application | `ImagingWorkbench` |

Public framework headers use the package-root convention:

- `<ImagingCore/ImagingCore.h>`
- `<ImagingIO/ImagingIO.h>`
- `<ImagingColor/ImagingColor.h>`
- `<ImagingAnalysis/ImagingAnalysis.h>`
- `<ImagingDiagnostics/ImagingDiagnostics.h>`
- `<Imaging/Imaging.h>`

All public framework types remain in `Upp::Imaging`.

## LumaPix disposition

`upp_lumapix` remains paused after its OpenImageIO reader proof. Its useful data-model and backend-isolation lessons have been carried into `upp_imaging`; this repository does not depend on `upp_lumapix`.

## Documentation

- `docs/ACTIVE_WORK.md` — current recovery/validation authority
- `docs/ARCHITECTURE.md` — dependency and ownership rules
- `docs/PACKAGE_CATALOGUE.md` — package roles and current state
- `docs/STATUS_AND_ROADMAP.md` — accepted scope, pending gates and next scope
- `docs/FFMPEG_PLAN.md` — FFmpeg first-slice contract and validation
- `docs/package_layout.md` — package-layout guidance
