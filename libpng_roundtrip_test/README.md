# libpng_roundtrip_test

User-facing memory round-trip test for `libpng`.

This test encodes a 2x2 RGBA image to PNG in memory, decodes it back from memory, and verifies the exact pixel values.
