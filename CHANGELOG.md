# Changelog

## Unreleased

### Current-generation framework and package architecture

* Completed the backend-neutral `Upp::Imaging` framework: `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics`, and the `Imaging` umbrella.
* Preserved Core-only/GUI-independent dependency boundaries for `ImagingCore`, `ImagingAnalysis`, and `ImagingDiagnostics`; OpenImageIO/OpenColorIO remain private behind framework APIs.
* Established `OpenImageIO` and `OpenColorIO` as canonical public packages while retaining strict/source implementation boundaries underneath them.
* Kept `plugin/exr` opt-in and display-oriented rather than making it a full-fidelity EXR API or an automatic `Imaging` dependency.
* Kept FFmpeg as a separate media subsystem rather than expanding `ImagingIO` into a movie API.

### Established Windows framework baseline

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
* Repaired shared static OpenImageIO plugin dependency ownership in `5ca436c3ba6265f6431deaf7348332940051686d`; the complete post-repair later-format accumulation matrix remains Windows validation pending.

### `plugin/exr`

* Implemented the opt-in EXR `StreamRaster` / `Upp::Image` preview bridge.
* Expanded the focused contract through `323c3dc29938de404fc3411b87dcaf6c6aea4559` to cover GrayAlpha, non-finite-to-zero preview behaviour, single-channel masks, named multichannel RGB selection, straight alpha, opaque/alpha reporting, invalid/truncated input and fixture cleanup.
* Current focused contract is 22 checks; Debug/Release Windows acceptance remains pending.

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
* Current expected first-slice gates are headers 8/0, avutil 13/0, avcodec 12/0, avformat 14/0, swscale 13/0, first-frame 27/0. Current-main full Debug/Release and repeatability acceptance remains pending.

### Documentation and recovery

* Reconciled README and architecture documentation with the implemented framework, still-image expansion, `plugin/exr`, and FFmpeg first-slice boundaries.
* Reconciled package catalogue, status/roadmap, package-layout guidance, and this changelog so implemented work is no longer described as planned.
* Formalized three distinct state labels: implemented, Windows-proven, and platform validation pending.
* `docs/ACTIVE_WORK.md` remains the recovery authority and records the exact published checkpoint, validation evidence, and next accumulated validator action.

### Deferred next scope

* FFmpeg SIMD/external assembly, hardware acceleration, broader codecs/containers, audio, seeking/indexing, a backend-neutral media wrapper, and waveform/vectorscope expansion remain explicitly deferred until the current bounded generation is fully accepted.
