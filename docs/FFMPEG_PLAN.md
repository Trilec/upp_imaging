# FFmpeg Integration Plan

## Purpose

FFmpeg is a separate media subsystem. It does not enlarge `ImagingIO` into a movie API and is not added to the `Imaging` umbrella merely because both domains contain pixels.

The first objective is deterministic **video-frame decode** on Windows CLANGx64 with a small, reviewable, LGPL-only source configuration. Audio, capture devices, filters, network protocols, command-line tools and encoding are outside the first slice.

## Upstream pin

- FFmpeg release: `n9.0.1`
- exact commit: `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`
- signed tag date: 2026-08-12
- licensing baseline: LGPL-2.1-or-later
- `--enable-gpl`, `--enable-version3`, `--enable-nonfree` and GPL/external codec integrations are not part of the first source configuration

## Package boundaries

### `ffmpeg_headers`

Internal strict public-header route over the pinned upstream tree. It owns the checked-in public `libavutil/avconfig.h` generated-equivalent header and compiles no implementation source.

### Planned implementation packages

The generated/source build will stay split by upstream library boundary rather than one recursive source glob:

- `ffmpeg_avutil_src`
- `ffmpeg_avcodec_src`
- `ffmpeg_avformat_src`
- `ffmpeg_swscale_src`

Each package will use explicit translation-unit manifests reconstructed from the pinned upstream Makefiles and the checked-in configuration. Tests and CLI/tool sources are never imported into production packages.

### Planned public direct package

`FFmpeg` will be the application-facing direct native API package after the four implementation packages link together. It will forward the standard FFmpeg headers without inventing replacement FFmpeg types.

A backend-neutral U++ media wrapper may be added later if it provides a clear contract. If created, it will be a separate package (for example `ImagingMedia`), not an extension of `ImagingIO` and not automatically pulled in by `Imaging`.

## First implementation configuration

The first code path is intentionally scalar/portable C. x86 assembly and hardware acceleration are deferred until the source boundary is green and measurable.

Target first decode closure:

- libraries: libavutil, libavcodec, libavformat, libswscale
- demux: MOV/MP4 family
- decoder: native H.264
- input protocol: local file only
- output conversion: decoded video frame to packed RGBA8 through libswscale
- threading: Win32 threads only if required by the selected decoder closure; otherwise single-process scalar operation first
- no encoders or muxers
- no audio decode/resample
- no libavfilter/libavdevice
- no network protocols
- no external codec libraries
- no dynamic plugin search

The pinned configure dependency graph shows H.264 selecting CABAC, Golomb, H264 chroma/DSP/parse/pred/qpel/SEI and VideoDSP helpers. MOV demuxing selects ISO-media and RIFF-decode helpers. Those selected helper objects must be represented explicitly in the source manifests rather than discovered through broad recursive globs.

## Generated configuration policy

FFmpeg normally generates `config.h`, `config_components.h`, `libavutil/avconfig.h` and related build metadata through its POSIX `configure` script. U++ builds must not depend on running that generator implicitly.

Repository-owned generated-equivalent headers will therefore be checked in for the supported Windows CLANGx64 configuration. They must be derived from the exact pin and documented with the configure-equivalent feature set. If the selected FFmpeg release changes, generated configuration and source manifests are reviewed together.

## Validation progression

1. public-header compile contract;
2. libavutil source/link/version test;
3. libavcodec H.264 decoder registration test;
4. libavformat MOV/file registration test;
5. libswscale YUV-to-RGBA conversion test;
6. deterministic embedded/generated video fixture decode to known RGBA frame evidence;
7. Debug/Release repeatability and clean shutdown;
8. only then consider SIMD, broader containers/codecs, audio, seek/index behavior, or a backend-neutral U++ media API.

The still-image OpenImageIO accumulation pass remains a separate validator lane and must not be blocked by this FFmpeg implementation work.
