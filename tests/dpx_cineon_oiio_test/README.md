# dpx_cineon_oiio_test

Focused GUI-free direct OpenImageIO contract for the pinned DPX/Cineon source
slice. It verifies static registration, DPX UInt8/UInt16 RGB read/write,
non-zero origin preservation, Cineon input-only registration, malformed-input
refusal (including 64 alternating failures on the ordinary caller thread),
and fixture cleanup.

Expected summary: `passed=20 failed=0`.
