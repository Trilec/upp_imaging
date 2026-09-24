# ImagingCore

`ImagingCore` is the first implemented backend-neutral U++ Imaging package. It
depends only on U++ Core and deliberately exposes no OpenImageIO or
OpenColorIO types.

The package defines image specifications, non-zero-origin inclusive data
windows, checked geometry and storage sizing, owned sample storage, typed
metadata, operation results, and structured diagnostics. Supported samples are
`UInt8`, `UInt16`, `Float16`, and `Float32`. Canonical layouts enforce their
channel counts and alpha positions; `MultiChannel` requires non-empty explicit
channel names.

`ImageBuffer` owns its bytes and clears itself when allocation from an invalid
or unrepresentable specification fails. `Metadata` supports Core `Value`
values including strings, booleans, integers, 64-bit integers, doubles, and
`ValueArray`. ImagingIO will translate supported backend metadata into this
representation and report unsupported complex values through `Result` and
`Diagnostics` rather than flattening them.

File loading, colour processing, codecs, and GUI integration belong in higher
layers. ImagingCore remains Core-only and exposes no backend types.
