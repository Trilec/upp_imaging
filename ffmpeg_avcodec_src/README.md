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

The explicit 69-translation-unit production manifest consists of the pinned upstream base `libavcodec` objects, the H.264 decoder objects, and the recursively selected CABAC/Golomb/H.264 chroma-DSP/parse-pred-qpel-SEI/VideoDSP/startcode/ITU-T T.35 helpers. The ITU-T T.35 selection is followed to its own configure dependencies, adding ATSC A/53 caption parsing and the Dolby Vision RPU decoder helpers. It is reconstructed from the exact upstream `libavcodec/Makefile` and `configure` selection graph; no unrelated decoder, encoder, parser, BSF, test or tool sources are globbed into production.

Generated registry files in `ffmpeg_headers/generated/libavcodec` expose exactly one codec (`ff_h264_decoder`) and empty parser/BSF lists. Internal H.264 parse helpers are implementation details and do not silently enable the public H.264 parser component.
