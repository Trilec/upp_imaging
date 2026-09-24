# ffmpeg_avformat_test

Focused linked-runtime contract for the pinned FFmpeg 9.0.1 MOV/file `libavformat` package.

It verifies version/license/configuration identity, exactly one demuxer, no muxers, exactly one local-file protocol for input/output, exact local AVIO file reading and deterministic cleanup, plus clean rejection of malformed MOV input.

Expected result: `SUMMARY passed=14 failed=0`.
