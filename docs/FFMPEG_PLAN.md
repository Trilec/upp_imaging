# FFmpeg Integration Plan

## Purpose

FFmpeg is a separate media subsystem. It does not enlarge `ImagingIO` into a movie API and is not added to the `Imaging` umbrella merely because both domains contain pixels.

The first objective is deterministic **video-frame decode** on Windows CLANGx64 with a small, reviewable, LGPL-only source configuration. Audio, capture devices, filters, network protocols, command-line tools and encoding remain outside the first slice.

## Upstream pin

- FFmpeg release: `n9.0.1`
- exact commit: `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`
- signed tag date: 2026-08-12
- licensing baseline: LGPL-2.1-or-later
- `--enable-gpl`, `--enable-version3`, `--enable-nonfree` and GPL/external codec integrations are not part of the first source configuration

## Implemented package boundaries

### `ffmpeg_headers`

Internal strict public-header route over the pinned upstream tree. It owns the checked-in generated-equivalent configuration/header files required by the supported Windows CLANGx64 source build and compiles no implementation source.

### Source implementation packages

The source build is split by upstream library boundary rather than one recursive source glob:

- `ffmpeg_avutil_src` — scalar libavutil foundation;
- `ffmpeg_avcodec_src` — native H.264 decoder closure plus MOV-selected MPEG-4 audio helpers;
- `ffmpeg_avformat_src` — MOV/MP4 demuxing and local `file` protocol only;
- `ffmpeg_swscale_src` — scalar libswscale conversion boundary.

Production source manifests are explicit and reconstructed from the pinned upstream Makefiles/configure selections. Tests, command-line tools and unrelated codecs/formats are not imported into production packages.

### `FFmpeg`

`FFmpeg` is the stable application-facing direct native API package. It links the four source implementation packages and forwards the standard FFmpeg C headers/types through `#include <FFmpeg/FFmpeg.h>` without inventing replacement AV types.

A backend-neutral U++ media wrapper may be added later if it provides a clear contract. If created, it will be a separate package (for example `ImagingMedia`), not an extension of `ImagingIO` and not automatically pulled in by `Imaging`.

## First implementation configuration

The first code path is intentionally scalar C. x86 architecture identity is retained, while external/inline assembly dispatch and hardware acceleration are disabled until the source boundary is Windows-proven and measurable.

Implemented first decode closure:

- libraries: libavutil, libavcodec, libavformat, libswscale;
- demux: MOV/MP4 family;
- decoder: native H.264;
- input protocol: local file only;
- output conversion: decoded video frame to packed RGBA8 through libswscale;
- no encoders or muxers;
- no audio decode/resample;
- no libavfilter/libavdevice;
- no network protocols;
- no external codec libraries;
- no dynamic plugin search.

The pinned configure dependency graph was followed through H.264 CABAC/Golomb/chroma/DSP/parse/pred/qpel/SEI/VideoDSP helpers, ITU-T T.35 -> ATSC A/53 + Dolby Vision RPU, MOV -> ISO-media + RIFF-decode, and ISO-media -> MPEG-4 audio helpers. Generated codec/parser/BSF/demuxer/muxer/protocol registries are checked in rather than relying on an implicit POSIX configure run.

## Generated configuration policy

FFmpeg normally generates `config.h`, `config_components.h`, `libavutil/avconfig.h` and related build metadata through its POSIX `configure` script. U++ builds do not depend on running that generator implicitly.

Repository-owned generated-equivalent headers are checked in for the supported Windows CLANGx64 configuration. They are reviewed together with source manifests whenever the FFmpeg pin or enabled component set changes.

## Validation progression

Code-side implementation now contains all first-slice checkpoints:

1. `ffmpeg_headers_test` — expected 7/0;
2. `ffmpeg_avutil_test` — expected 13/0;
3. `ffmpeg_avcodec_test` — expected 12/0;
4. `ffmpeg_avformat_test` — expected 14/0;
5. `ffmpeg_swscale_test` — expected 13/0;
6. `ffmpeg_first_frame_test` — expected 27/0.

The end-to-end test embeds a 1,463-byte one-frame 16x16 MP4 containing Constrained Baseline H.264. It verifies exact fixture identity, MOV demux, native H.264 decode, exact logical YUV420P evidence, scalar swscale conversion to RGBA8, and deterministic cleanup without an external runtime fixture.

The remaining gate for this first slice is Windows Debug/Release acceptance and repeatability. Only after that should SIMD, hardware acceleration, broader containers/codecs, audio, seeking/index behavior, or a backend-neutral U++ media API be considered.

The still-image OpenImageIO accumulation pass remains a separate validator lane and must not be blocked by FFmpeg work.
