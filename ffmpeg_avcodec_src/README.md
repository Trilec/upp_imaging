# ffmpeg_avcodec_src

Pinned FFmpeg 9.0.1 `libavcodec` implementation for the first U++ video decode stack.

## Enabled codec surface

- native H.264 decoder only
- no encoders
- no public parser registration
- no bitstream filters
- no hardware acceleration
- no frame/slice threading
- scalar C only; x86 assembly/inline-assembly dispatch is disabled by the shared generated configuration

The production manifest now contains 71 explicit translation units: the pinned upstream base `libavcodec` objects, native H.264 decoder, recursively selected CABAC/Golomb/H.264 chroma-DSP/parse-pred-qpel-SEI/VideoDSP/startcode/ITU-T T.35 helpers, ATSC A/53 and Dolby Vision RPU decode helpers, plus the MPEG-4 audio helpers selected by the MOV demuxer's ISO-media dependency. It is reconstructed from the exact upstream `libavcodec/Makefile` and `configure` graph; no unrelated decoder, encoder, parser, BSF, test or tool sources are globbed into production.

Generated registry files in `ffmpeg_headers/generated/libavcodec` expose exactly one codec (`ff_h264_decoder`) and empty parser/BSF lists. Internal H.264/MPEG-4 parsing helpers are implementation details and do not silently enable additional public codec/parser components.
