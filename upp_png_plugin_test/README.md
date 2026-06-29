# upp_png_plugin_test

Round-trip comparison test for U++ `plugin/png`.

This test creates a small premultiplied-alpha `Image`, encodes it with `PNGEncoder`, decodes it with `PNGRaster`, and verifies the decoded premultiplied pixel values.
