# ImagingIO

`ImagingIO` is the first backend-neutral file-I/O slice. Its public API uses
only `Upp::Imaging` and depends on `ImagingCore`; OpenImageIO is private to the
implementation.

The initial slice supports ordinary, single-image, non-deep EXR Float16 and
Float32 files, including non-zero and negative data-window origins, and PNG
UInt8/UInt16 canonical Gray, GrayAlpha, RGB, and RGBA files at zero origin.
Supported OpenImageIO scalar and array metadata is translated to Core
`Value`; unsupported metadata is omitted with a warning diagnostic.

Deep, multipart, mip, volume, mixed-format, integer EXR, floating PNG, and
arbitrary PNG multichannel images are explicitly unsupported.
