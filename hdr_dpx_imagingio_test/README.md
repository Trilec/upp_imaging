# hdr_dpx_imagingio_test

Focused GUI-free ImagingIO contract for Radiance HDR/RGBE and DPX/Cineon.

The test covers representable Float32 RGB HDR/RGBE roundtrips, UInt8/UInt16 RGB
DPX roundtrips including positive origin preservation, explicit format/sample/
channel/origin refusals, Cineon input-only policy, transactional destination
preservation, verified replacement, and transaction cleanup.

Expected summary: `passed=38 failed=0`.
