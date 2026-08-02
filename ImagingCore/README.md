# ImagingCore

`ImagingCore` is the first implemented backend-neutral U++ Imaging package. It
depends only on U++ Core and deliberately exposes no OpenImageIO or
OpenColorIO types.

The package defines image specifications, non-zero-origin data windows, typed
sample storage, metadata, operation results, and diagnostics. File loading,
colour processing, codecs, and GUI integration belong in higher layers.
