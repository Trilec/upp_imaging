# ffmpeg_avformat_src

Pinned FFmpeg 9.0.1 `libavformat` implementation for the first U++ video decode stack.

## Enabled format surface

- MOV/MP4-family demuxer only
- local `file` protocol only
- no muxers
- no network protocols
- no devices, playlist/network helpers or external container libraries

The explicit 43-translation-unit manifest contains the 31 pinned upstream base `libavformat` objects, the Windows MSVCRT file-open helper, ISO-media and RIFF-decode helpers, the seven MOV demuxer objects, the local file protocol, and FFmpeg's MOV-private AC-3 channel-layout table helper. The MOV configure graph selects `ISO_MEDIA` and `RIFFDEC`; `ISO_MEDIA` selects the MPEG-4 audio parser helpers, which remain owned by `ffmpeg_avcodec_src`. Optional IAMF and zlib suggestions remain disabled.

Generated registries expose exactly one demuxer (`ff_mov_demuxer`), no muxers, and one protocol (`ff_file_protocol`). No recursive source globs are used.
