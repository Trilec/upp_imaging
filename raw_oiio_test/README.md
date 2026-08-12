# raw_oiio_test

Focused static-registration contract for OpenImageIO RAW input support.

The test verifies that RAW is input-only, `.dng` resolves to the RAW reader, `.hdr` remains owned by the Radiance HDR plugin despite LibRaw's upstream extension list, malformed DNG input fails with an error, and fixture cleanup is complete.

A positive real camera-file decode is intentionally not fabricated here; it belongs to the Windows acceptance layer using a pinned, provenance-reviewed RAW fixture.

Expected summary: `passed=9 failed=0`.
