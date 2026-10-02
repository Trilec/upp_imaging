# U++ image and video quick start

The Windows trusted-local delivery covers JPEG, PNG, JPEG XL, EXR, DPX and
Radiance HDR, plus H.264 MP4/MOV. Workbench also retains WebP, TIFF, Cineon,
AVIF/HEIC and synthetic linear DNG coverage. Restrictions and native-library
limits are in [INPUT_POLICY.md](INPUT_POLICY.md).

For a U++ display preview, add the opt-in package to your .upp `uses` list,
include its header and use normal StreamRaster loading:

```cpp
#include <plugin/imaging_jpeg/imaging_jpeg.h>
Image preview = StreamRaster::LoadFileAny(local_jpeg_path);
```

| Format | U++ opt-in package |
| --- | --- |
| JPEG (.jpg/.jpeg) | plugin/imaging_jpeg |
| PNG | plugin/imaging_png |
| JPEG XL | plugin/imaging_jxl |
| EXR | plugin/exr |
| DPX | plugin/imaging_dpx |
| Radiance HDR | plugin/imaging_hdr |

For full-fidelity images, add `ImagingIO` and call
`Upp::Imaging::LoadImageFile(path, image_data)`. EXR/HDR floating-point data
keeps values outside the display range; StreamRaster preview is RGBA8 and
clamps to its display range. Workbench offers exposure and OCIO view controls.
JPEG core saving accepts UInt8 Gray/RGB; alpha is rejected explicitly.
Workbench saving remains EXR/PNG.

For video, add `ImagingVideo`, include `<ImagingVideo/ImagingVideo.h>`, open a
local MP4/MOV with `Upp::Imaging::VideoReader`, then call `ReadNext` for U++
images/timestamps or `Seek` to select a time. The current backend is H.264,
not arbitrary MOV/MP4 codecs, audio or HDR video.

Generated small image/video samples for manual loading are retained under
`build/windows-x64/samples/`. JPEG XR is a different format from EXR and is
deferred. XML colour files (CTF/CLF/CDL/CC/CCC/look) are disabled; YAML configs,
non-XML LUTs and programmatic colour transforms remain supported.

See [PACKAGE_CATALOGUE.md](PACKAGE_CATALOGUE.md) for physical package paths and
[BUILD_AND_RUN.md](BUILD_AND_RUN.md) for the assembly and output directories.
