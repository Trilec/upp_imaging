# libraw_prereq_test

Focused backend test for the repository-pinned LibRaw 0.22.2 source package.

The test verifies the exact compile/runtime version, populated camera table, processor construction, deterministic malformed-buffer rejection and error reporting. It deliberately does not require a camera RAW fixture; positive camera-file decoding is owned by the OpenImageIO/ImagingIO acceptance layer.

Expected summary: `passed=8 failed=0`.
