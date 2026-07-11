# openexr_src_test

Deterministic strict high-level OpenEXR test.

- writes a 4 x 3 scanline RGBA HALF EXR with ZIP compression
- reads it back through the high-level API
- verifies exact pixels and file settings
- rejects a malformed non-EXR file
