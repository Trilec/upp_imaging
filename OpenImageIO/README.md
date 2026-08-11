# OpenImageIO

Canonical application-facing OpenImageIO package. Include `<OpenImageIO/OIIO.h>`.
Use `UppImaging::LoadImage` and `UppImaging::SaveImage` for the statically
registered OpenEXR, PNG and JPEG XL plugins.

JPEG XL uses the repository-pinned libjxl 0.12.0 backend; callers do not need a
system libjxl installation.
