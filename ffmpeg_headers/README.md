# ffmpeg_headers

Strict public-header and generated-configuration boundary for the repository's pinned FFmpeg release.

## Provenance

- upstream: FFmpeg/FFmpeg
- release tag: `n9.0.1`
- exact commit: `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`
- signed tag date: 2026-08-12
- base license: LGPL-2.1-or-later when GPL/nonfree components are not enabled

The upstream tree is a git submodule. This package compiles no FFmpeg implementation source. It exports the ordinary FFmpeg public include routes such as `<libavutil/avutil.h>`, `<libavcodec/avcodec.h>`, `<libavformat/avformat.h>` and `<libswscale/swscale.h>`.

## Generated configuration

FFmpeg normally creates build headers with its POSIX `configure` script. For the repository's fixed Windows x86_64 / U++ CLANGx64 decode-first configuration, generated-equivalent headers are checked in under `generated/` so TheIDE builds do not depend on a configure step:

- `config.h` — private library feature/platform configuration
- `config_components.h` — enabled component registry (H.264 decoder, MOV demuxer, file protocol)
- `libavutil/avconfig.h` — public ABI/header configuration
- `libavutil/ffversion.h` — pinned release version string

The configuration is static, scalar, LGPL-only and deliberately disables external/inline assembly, threads, network, external codec libraries, filters, devices, audio resampling, CLI programs and encoding for the first slice.

Implementation libraries live in separate `_src` packages. Keeping this package free of implementation objects prevents a partial FFmpeg bring-up from being mistaken for a validated media backend.
