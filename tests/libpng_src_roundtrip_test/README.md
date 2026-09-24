# libpng_src_roundtrip_test

Strict memory round-trip test for `libpng_src`.

This test encodes a 2x2 RGBA image to PNG in memory, decodes it back from memory, and verifies the exact pixel values.
