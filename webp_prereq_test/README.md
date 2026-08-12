# webp_prereq_test

Focused backend contract for repository-pinned libwebp 1.6.0.

The test verifies exact encoder, decoder, mux and demux runtime versions, lossless RGB encode/decode, and an exact RGBA path using `WebPConfig::exact=1` that preserves hidden RGB values beneath a fully transparent pixel as well as alpha.

Expected summary: `passed=11 failed=0`.
