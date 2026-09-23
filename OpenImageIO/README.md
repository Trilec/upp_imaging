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

## Pinned error-storage debt

OpenImageIO 3.1.15.0 stores per-ImageInput/ImageOutput errors in thread-local
maps keyed by object ID. Draining geterror() erases entries but retains map
capacity; destructors do not erase undrained errors. This is the issue addressed upstream by
[ce1be9749586ee7ef3f2b9cb82aee8b199991f03](https://github.com/AcademySoftwareFoundation/OpenImageIO/commit/ce1be9749586ee7ef3f2b9cb82aee8b199991f03),
"fix: avoid leaks in the per-ImageInput/Output error message".

Joined worker scopes in hdr_oiio_test, dpx_cineon_oiio_test, raw_oiio_test,
webp_oiio_test, heif_oiio_test, tiff_oiio_test, raw_imagingio_test,
webp_imagingio_test, heif_imagingio_test, tiff_imagingio_test and plugin_exr_test
exist for this pinned dependency: they retire TLS before main-thread U++
HEAPDBG. The WebP/TIFF roundtrip scopes also encounter backend probe/diagnostic
storage. They are not a production threading requirement or concurrency test.
Re-evaluate/remove these scopes when upgrading OIIO; test negative paths and
clean Debug process exit, not just passing check counts. Do not patch the pin
or add production worker threads to hide the issue.

ImagingIO continues to use generic input probing to preserve existing
content-sniffing/fallback semantics; extension policy alone does not prove the
file's actual encoding. Selecting factories for every load would change which
mislabeled files are accepted. The EXR preview bridge already validates EXR
magic and correctly selects the openexr factory. Production malformed-input
error-storage debt therefore remains until the dependency is upgraded.
