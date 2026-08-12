# OpenImageIO

Canonical application-facing OpenImageIO package. Include `<OpenImageIO/OIIO.h>`.
Use `UppImaging::LoadImage` and `UppImaging::SaveImage` for the statically
registered OpenEXR, PNG, JPEG XL, Radiance HDR/RGBE, DPX, Cineon and camera RAW
plugins.

JPEG XL uses the repository-pinned libjxl 0.12.0 backend. HDR/RGBE is
self-contained. DPX and Cineon compile from the exact signed OpenImageIO
3.1.15.0 plugin source tree; DPX is read/write and Cineon is input-only, matching
upstream capability. RAW reuses that same OpenImageIO source pin and is backed
by repository-pinned LibRaw 0.22.2. RAW is input-only, and its upstream `.hdr`
extension alias is deliberately excluded so `.hdr` remains the Radiance format.
