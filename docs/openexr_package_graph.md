# OpenEXR package graph

`openexr` exposes the high-level C++ API through `openexr_src`, which depends on `imath_src`, `iex_src`, `ilmthread_src` and `openexr_core_src`.

`openexr_core` exposes the lower-level API through `openexr_core_src`. That source owner depends on `imath_src`, `ilmthread_src`, `libdeflate_src` and `openjph_src`. Compression sources remain owned by their codec packages, not duplicated inside OpenEXR.

The OpenEXR packages live under `third_party/openexr/`, Imath under `third_party/imath/`, and the compression packages under `third_party/codecs/`. Public package names and include identities are unchanged.

`openexr_test` validates the high-level RGBA HALF ZIP roundtrip and malformed input. `openexr_core_rgba_zip_test` validates the lower-level RGBA ZIP pipeline. ImagingIO and plugin/exr test their respective typed-data and display boundaries separately.
