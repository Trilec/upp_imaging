# heif_imagingio_test

Focused backend-neutral input-only policy test for AVIF and HEIC/HEIF.

The deterministic local contract verifies explicit side-effect-free output refusal with `IMGIO_FORMAT`, malformed AVIF routing through the HEIF reader, transactional preservation of prior `ImageData`, and cleanup. Positive 8/10-bit AVIF and HEIC decode evidence is owned by the Windows acceptance matrix using real provenance-reviewed fixtures.

Expected summary: `passed=10 failed=0`.
