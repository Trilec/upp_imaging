# Package Catalogue

Primary navigation for `upp_imaging`.

## State labels

This catalogue distinguishes three states:

- **implemented** — package/source/test contract exists and has passed source/static review;
- **Windows-proven** — the relevant U++ CLANGx64 acceptance has been recorded;
- **platform validation pending** — implementation exists but the current accumulated Windows checkpoint is not yet accepted.

`docs/ACTIVE_WORK.md` is the authority for the exact in-flight validation boundary.

## Package model

- direct packages expose established upstream-style APIs to applications;
- `_src`, generated-header and static-registration packages own pinned implementation details and are not ordinary application dependencies;
- narrow helpers expose deliberately limited U++-friendly file subsets;
- `Upp::Imaging` framework packages expose backend-neutral U++ contracts;
- raster plugins under `plugin/*` are opt-in display integrations;
- FFmpeg is a separate media subsystem, not an `ImagingIO` backend;
- tests/probes are the pass/fail authority; `ImagingWorkbench` is supplementary diagnostics.

## Stable direct application packages

### `openexr`
- Stable high-level OpenEXR API.
- Pinned OpenEXR 3.4.13 implementation is owned by `openexr_src`.
- Primary validation: `openexr_test`.

### `openexr_core`
- Stable OpenEXRCore C API.
- Implementation is owned by `openexr_core_src`.
- Primary validation: `openexr_core_write_probe`, `openexr_core_roundtrip_test`.

### `OpenColorIO`
- Canonical application-facing OpenColorIO package over `opencolorio_src`.
- Pinned version: 2.5.2.
- The old lowercase public package name is intentionally not provided on Windows because package paths cannot differ only by case.
- Primary validation: `opencolorio_test`, `opencolorio_gui_link_test`.

### `OpenImageIO`
- Canonical application-facing OpenImageIO package over `openimageio_headers`, `openimageio_src`, `openimageio_util_src` and statically registered format packages.
- Pinned version: 3.1.15.0.
- `oiio` remains a compatibility forwarder; it is not a second implementation.
- The original Windows-proven OpenEXR/PNG route remains the accepted baseline.
- Code-side static format expansion now includes JPEG XL, Radiance HDR/RGBE, DPX/Cineon, camera RAW, WebP, decode-only HEIF/AVIF and TIFF support required by the current `ImagingIO` format line.
- The shared static dependency repair is `5ca436c3ba6265f6431deaf7348332940051686d`; the complete post-repair accumulation matrix is still platform-validation pending.

### Other stable direct packages

The repository also retains independently usable stable packages including:

- `imath`
- `zlib`
- `libpng`
- `libjpeg_turbo`
- `libtiff`
- `libdeflate`
- `openjph`
- `fmt`
- `robinmap`

Each stable package owns the application-facing boundary; corresponding strict/source packages own pinned build details.

## `Upp::Imaging` framework

All framework public types live under `Upp::Imaging`. Public framework headers do not expose OIIO, OCIO, strict-source filesystem paths or application GUI types.

### `ImagingCore`
- Backend-neutral image data model, metadata, window, result and diagnostic contracts.
- Core-only dependency boundary.
- Public concepts include `ImageSpec`, `ImageBuffer`, `ImageData`, `Metadata`, `DataWindow`, `SampleType`, `ChannelLayout`, `Result` and `Diagnostics`.
- **Implemented and Windows-proven**: established baseline `imaging_core_test` 48/0.

### `ImagingIO`
- Backend-neutral typed image load/save API using OpenImageIO privately.
- Public headers expose only `Upp::Imaging` types.
- Accepted baseline: EXR/PNG.
- Code-side format line: JPEG XL, HDR/RGBE, DPX/Cineon, RAW, WebP, decode-only HEIF/AVIF and TIFF expansion.
- Preserves transactional load/save and stable framework diagnostics.
- **Implemented**; baseline Windows-proven at 79/0, later-format accumulation remains pending after `5ca436c3`.

### `ImagingColor`
- Backend-neutral colour-processing API using OpenColorIO privately.
- Supports the documented RGB/RGBA and unambiguous named multichannel transform subset while preserving alpha and non-RGB channels.
- **Implemented and Windows-proven**: 66/0 plus independent OCIO 15/0.

### `ImagingAnalysis`
- Core-only numerical analysis layer.
- Provides per-channel statistics, normalized histograms, finite/non-finite accounting and source-coordinate probes.
- Waveform/vectorscope algorithms are deferred next-scope work.
- **Implemented and Windows-proven**: 41/0.

### `ImagingDiagnostics`
- Core-only deterministic numerical comparison and reporting layer.
- Formats/compares existing `ImagingCore` contracts rather than duplicating state.
- GUI-independent.
- **Implemented and Windows-proven**: 33/0.

### `Imaging`
- Convenience umbrella over `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis` and `ImagingDiagnostics`.
- Does not automatically include `plugin/exr` or FFmpeg.
- **Implemented and Windows-proven**: 6/0.

## U++ raster integration

### `plugin/exr`
- Opt-in display-oriented `StreamRaster` / `Upp::Image` bridge.
- Not a full-fidelity EXR API and not part of the `Imaging` umbrella.
- Current contract covers ordinary single-image EXR preview, RGB/RGBA, Gray/GrayAlpha, one-channel masks, named multichannel RGB selection, straight alpha, deterministic finite-value clamp/rounding, non-finite-to-zero preview behaviour, and truthful opaque/alpha reporting.
- Does not claim arbitrary-channel preservation, source floating-point preservation, full metadata/window semantics, multipart/deep/mip support or unclamped HDR fidelity.
- **Implemented** through `323c3dc29938de404fc3411b87dcaf6c6aea4559`; expanded `plugin_exr_test` contract is 22 checks and remains Windows Debug/Release pending.

## Narrow format helpers

These remain intentionally narrower than the full framework/backends:

### `openexr_io`
- Scanline RGBA HALF/FLOAT subset over `openexr_core`.
- No tiled, multipart, deep, arbitrary-channel or general metadata claim.

### `png_io`
- RGBA8 ordinary PNG helper over `libpng`.
- Does not claim metadata/ICC/gamma/source-depth preservation.

### `jpeg_io`
- RGB8 baseline JPEG helper over `libjpeg_turbo`.
- Lossy by design; not a metadata-preserving wrapper.

### `tiff_io`
- Typed RGBA classic single-directory scanline TIFF helper over `libtiff`.
- No tiled, planar-separate, palette, CMYK, general metadata/ICC or BigTIFF claim.

## FFmpeg media subsystem

### `FFmpeg`
- Stable direct application package forwarding standard FFmpeg C headers/types.
- Separate from `ImagingIO` and the `Imaging` umbrella.
- First slice is static LGPL scalar decode only.

Implementation packages:

- `ffmpeg_headers` — public/generated configuration boundary; no implementation source.
- `ffmpeg_avutil_src` — scalar libavutil foundation.
- `ffmpeg_avcodec_src` — native H.264 decoder closure only.
- `ffmpeg_avformat_src` — MOV/MP4 demux + local `file` protocol only.
- `ffmpeg_swscale_src` — scalar YUV-to-RGBA conversion boundary.

Exact upstream pin: signed FFmpeg `n9.0.1`, commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.

The first slice deliberately disables threads, network, external codecs, filters, devices, audio resampling, CLI/encoding, external/inline assembly and hardware acceleration.

Current expected focused gates:

- `ffmpeg_headers_test` 8/0 (includes generated-config parity audit)
- `ffmpeg_avutil_test` 13/0
- `ffmpeg_avcodec_test` 12/0
- `ffmpeg_avformat_test` 14/0
- `ffmpeg_swscale_test` 13/0
- `ffmpeg_first_frame_test` 27/0

The avformat manifest explicitly owns the two pinned FFmpeg materializers `libavformat/to_upper4.c` and `libavformat/mpegaudiotabs.c`; this closes the current `ff_toupper4` / `ff_mpa_freq_tab` linker defect without enabling any new component.

**Implementation/source ownership is closed; current-main Debug/Release and repeatability acceptance remains pending.**

## Strict imported-source/package boundaries

Ordinary applications must not depend directly on strict implementation packages. Established strict boundaries include:

| Strict package | Upstream role | Stable/public route |
| --- | --- | --- |
| `zlib_src` | pinned zlib source proof | `zlib` |
| `libpng_src` | pinned libpng source proof | `libpng` |
| `imath_src` | pinned Imath source proof | `imath` |
| `libdeflate_src` | pinned libdeflate source proof | `libdeflate` |
| `openjph_src` | pinned OpenJPH source proof | `openjph` |
| `iex_src`, `ilmthread_src`, `openexr_core_src`, `openexr_src` | OpenEXR stack ownership | `openexr_core`, `openexr` |
| `libjpeg_turbo_src` | pinned libjpeg-turbo source proof | `libjpeg_turbo` |
| `libtiff_src` | pinned libtiff source proof | `libtiff` |
| `opencolorio_src` | pinned OpenColorIO implementation | `OpenColorIO` |
| `openimageio_headers`, `openimageio_src`, `openimageio_util_src`, static plugin packages | pinned OpenImageIO implementation/registration | `OpenImageIO` |
| `fmt_src` | pinned fmt headers | `fmt` |
| `robinmap_src` | pinned robin-map headers | `robinmap` |
| `ffmpeg_headers`, `ffmpeg_*_src` | pinned bounded FFmpeg implementation | `FFmpeg` |

Conflict rule: do not link strict and stable implementations of the same underlying library into one ordinary executable.

## Validation and diagnostics

- Automated tests determine PASS/FAIL.
- `ImagingWorkbench` and round-trip viewers are supplementary diagnostics only.
- Generated executables/images belong in ignored output directories.
- Machine-specific U++ nest configuration is not committed.
- Source manifests remain explicit; recursive globs must not be used to hide missing ownership/dependency closure.

## Dependency direction

```text
pinned/strict source packages
        ↓
stable direct packages
        ↓
Upp::Imaging framework
        ↓
application / diagnostic integration
```

FFmpeg remains a parallel stable-direct media stack, not an Imaging framework dependency.

`ImagingCore`, `ImagingAnalysis` and `ImagingDiagnostics` remain Core-only/GUI-independent according to their documented boundaries. `plugin/exr` remains opt-in.

## Current closure boundary

The bounded current-generation implementation is complete enough for accumulated platform acceptance. Remaining closure gates are:

1. repaired OpenImageIO/later-format accumulation after `5ca436c3` in Debug and Release;
2. `plugin_exr_test` 22/0 in Debug and Release;
3. all six FFmpeg gates in Debug and Release plus first-frame repeatability;
4. coherent root-cause repairs only if current-main Windows validation exposes a substantive defect.

SIMD/hardware FFmpeg paths, broader codecs/containers, audio, seeking/indexing, waveform/vectorscope expansion and a possible backend-neutral media wrapper are deferred next scope, not incomplete requirements of this milestone.
