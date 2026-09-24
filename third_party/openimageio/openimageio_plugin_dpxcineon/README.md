# openimageio_plugin_dpxcineon

Pinned OpenImageIO 3.1.17.0 DPX and Cineon source slice for static U++ registration. The package compiles from the neutral `third_party/openimageio/openimageio_plugins_src/upstream` source holder pinned to exact OIIO release commit `73bc189f7d8469a9760ce9c5099b686c77695074`.

The package compiles only the source files named by the upstream DPX and Cineon CMake plugin definitions. DPX provides input and output; Cineon is input-only, matching OpenImageIO 3.1.17.0 rather than inventing a framework-side encoder.

The package is intentionally `noblitz`. Upstream builds the DPX and Cineon plugins as separate translation units; DPX's bundled `libdpx` declares a global `InStream`, while Cineon's bundled `libcineon` declares `cineon::InStream` and `cineoninput.cpp` uses `using namespace cineon`. Combining those sources in one U++ Blitz translation unit makes the otherwise-valid Cineon `InStream` references ambiguous.

No system DPX/Cineon library is used. The DPX plugin's bundled `libdpx` and the Cineon plugin's bundled `libcineon` are compiled directly from the shared pinned OIIO source tree.
