# openimageio_plugin_dpxcineon

Pinned OpenImageIO 3.1.15.0 DPX and Cineon source slice for static U++ registration. The package compiles from the neutral `openimageio_plugins_src/upstream` source holder pinned to exact OIIO release commit `cbe57bc005678ca310835473568121719861734c`.

The package compiles only the source files named by the upstream DPX and Cineon CMake plugin definitions. DPX provides input and output; Cineon is input-only, matching OpenImageIO 3.1.15.0 rather than inventing a framework-side encoder.

No system DPX/Cineon library is used. The DPX plugin's bundled `libdpx` and the Cineon plugin's bundled `libcineon` are compiled directly from the shared pinned OIIO source tree.
