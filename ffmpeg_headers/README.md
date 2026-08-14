# ffmpeg_headers

Strict public-header boundary for the repository's pinned FFmpeg release.

## Provenance

- upstream: FFmpeg/FFmpeg
- release tag: `n9.0.1`
- exact commit: `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`
- signed tag date: 2026-08-12
- base license: LGPL-2.1-or-later when GPL/nonfree components are not enabled

The upstream tree is a git submodule. This package intentionally compiles no FFmpeg implementation source. It exports the ordinary FFmpeg public include routes such as `<libavutil/avutil.h>`, `<libavcodec/avcodec.h>`, `<libavformat/avformat.h>` and `<libswscale/swscale.h>`.

`generated/libavutil/avconfig.h` is repository-owned because FFmpeg normally creates it through `configure`. For the supported Windows CLANGx64 little-endian target it records only the two public ABI/configuration facts used by installed headers: little endian and fast unaligned access.

Implementation libraries are deliberately separate follow-on `_src` packages. Keeping this package header-only prevents a partial or generator-dependent FFmpeg build from being mistaken for a validated media backend.
