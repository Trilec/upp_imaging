# openimageio_plugins_src

Neutral source-holder package for OpenImageIO 3.1.15.0 built-in image I/O plugins.

The `upstream` git submodule is pinned to the signed OpenImageIO 3.1.15.0 release commit `cbe57bc005678ca310835473568121719861734c`. Format packages compile only the plugin sources they require from this one tree. This avoids duplicate OpenImageIO checkouts and prevents one format package from owning source used by unrelated siblings.

This package compiles no plugin by itself and exposes no application API. Static registration remains the responsibility of the individual `openimageio_plugin_*` packages and the stable `OpenImageIO` facade.
