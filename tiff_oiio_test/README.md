# tiff_oiio_test

Focused OpenImageIO/libtiff integration contract. It verifies static TIFF registration, `.tif/.tiff` routing, exact ZIP roundtrips for UInt8 RGB/RGBA (including hidden transparent RGB), UInt16 RGB and Float32 Gray, malformed-input refusal and cleanup.

Expected summary: `passed=13 failed=0`.
