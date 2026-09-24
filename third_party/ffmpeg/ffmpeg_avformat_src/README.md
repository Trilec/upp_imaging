# ffmpeg_avformat_src

Pinned FFmpeg 9.0.1 `libavformat` implementation for the first U++ video decode stack.

## Enabled format surface

- MOV/MP4-family demuxer only
- local `file` protocol only
- no muxers
- no network protocols
- no devices, playlist/network helpers or external container libraries

The explicit 45-translation-unit manifest contains the 31 pinned upstream base `libavformat` objects, the Windows MSVCRT file-open helper, ISO-media and RIFF-decode helpers, the seven MOV demuxer objects, the local file protocol, and the selected shared-build materializers required by this bounded source configuration.

The pinned `libavformat/Makefile` duplicates several symbols from other FFmpeg libraries when building libavformat. In this U++ source split, `log2_tab` is already materialized by `ffmpeg_avutil_src`, `h2645_parse` is already materialized by the selected H.264 `ffmpeg_avcodec_src` closure, and the MOV-private AC-3 channel-layout table is compiled here from `libavcodec/ac3_channel_layout_tab.c`. `libavformat/to_upper4.c` is required to materialize `ff_toupper4`, and `libavformat/mpegaudiotabs.c` is required by selected `CONFIG_ISO_MEDIA` to materialize the MPEG-audio tables used by ISO-media parsing. These are implementation/link ownership objects only; they do not enable additional codecs, muxers, protocols or public components.

The MOV configure graph selects `ISO_MEDIA` and `RIFFDEC`; `ISO_MEDIA` selects the MPEG-4 audio parser helpers, which remain owned by `ffmpeg_avcodec_src`. Optional IAMF and zlib suggestions remain disabled.

Generated registries expose exactly one demuxer (`ff_mov_demuxer`), no muxers, and one protocol (`ff_file_protocol`). No recursive source globs are used.
