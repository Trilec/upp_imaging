# webp_oiio_test

Focused OpenImageIO WebP registration and fidelity contract.

It verifies static input/output registration, `.webp` routing, non-empty lossless RGB/RGBA writes, byte-exact RGB readback, and exact RGBA readback including hidden RGB values beneath a fully transparent pixel. Malformed input must fail with an error and all fixtures are removed.

Expected summary: `passed=13 failed=0`.
