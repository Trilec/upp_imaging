# raw_imagingio_test

Focused backend-neutral policy test for camera RAW integration in `ImagingIO`.

The deterministic local contract verifies that representative RAW extensions are recognized as input-only, save refusal is side-effect free and uses `IMGIO_FORMAT`, malformed DNG reaches the registered RAW reader and preserves the caller's prior `ImageData`, and Radiance `.hdr` routing remains intact beside LibRaw.

A positive camera-file decode is intentionally not synthesized. Windows acceptance must use a pinned, provenance-reviewed real RAW/DNG fixture and additionally verify the supported processed output contract: zero-origin UInt16 RGB, camera white balance, camera matrix, no auto-bright, and `srgb_rec709_scene` output colour-space selection.

Expected summary: `passed=10 failed=0`.
