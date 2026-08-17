# hdr_oiio_test

Focused GUI-free OpenImageIO Radiance HDR/RGBE contract test. It verifies
static input/output registration, both `.hdr` and `.rgbe` extensions, native
Float32 RGB structure, representable RGBE values across SDR/HDR ranges,
malformed-input refusal, and fixture cleanup.

Expected summary: `passed=12 failed=0`.
