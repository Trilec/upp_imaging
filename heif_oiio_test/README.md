# heif_oiio_test

Focused decode-only OpenImageIO registration contract for HEIF-family files.

The test verifies `heif` input registration, deliberate absence of output, AVIF and HEIC/HEIF extension routing, factory resolution to the HEIF input plugin, malformed AVIF rejection with an error, and fixture cleanup.

Positive AVIF/HEIC decoding is intentionally validated with provenance-reviewed real files in the Windows acceptance matrix.

Expected summary: `passed=11 failed=0`.
