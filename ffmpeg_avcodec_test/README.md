# ffmpeg_avcodec_test

Focused linked-runtime contract for the pinned FFmpeg 9.0.1 native H.264 `libavcodec` package.

It verifies runtime/header version identity, LGPL/configuration reporting, exactly one registered codec, native H.264 decoder availability, absence of H.264 encoding, deliberately empty public parser/BSF registries, decoder context allocation/open and deterministic cleanup.

Expected result: `SUMMARY passed=12 failed=0`.
