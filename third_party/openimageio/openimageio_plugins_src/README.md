# openimageio_plugins_src

Neutral source-holder package for OpenImageIO 3.1.17.0 built-in image I/O plugins.

The `upstream` git submodule is pinned to the signed OpenImageIO 3.1.17.0 release commit `73bc189f7d8469a9760ce9c5099b686c77695074`. Format packages compile only the plugin sources they require from this one tree. This avoids duplicate OpenImageIO checkouts and prevents one format package from owning source used by unrelated siblings.

This package compiles no plugin by itself and exposes no application API. Static registration remains the responsibility of the individual `openimageio_plugin_*` packages and the stable `OpenImageIO` facade.
