# Consolidated U++ image-format integration

Positive EXR/PNG/JXL/HDR/DPX/WebP/TIFF/Cineon/DNG fixtures are generated from
original synthetic data. AVIF and single-image HEIC use pinned libheif upstream
examples/test data. The multi-image HEIC example is verified and rejected.
Tests cover direct adapters, registered memory/file loading, decoded pixels,
straight alpha and encoded/dimension/pixel/combined-allocation limits.

Manual samples remain under `build/windows-x64/samples/` and can be regenerated
by this Debug test. DNG is a synthetic 64x64 linear RAW example, not a coverage
claim for every camera manufacturer. Cineon is an 8-bit Rec.709 RGB example.
