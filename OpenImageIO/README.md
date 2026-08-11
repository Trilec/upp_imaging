# OpenImageIO

Canonical application-facing OpenImageIO package. Include `<OpenImageIO/OIIO.h>`.
Use `UppImaging::LoadImage` and `UppImaging::SaveImage` for the statically
registered OpenEXR, PNG, JPEG XL and Radiance HDR/RGBE plugins.

JPEG XL uses the repository-pinned libjxl 0.12.0 backend. Radiance HDR/RGBE is
self-contained in OpenImageIO and introduces no additional external codec
package.
