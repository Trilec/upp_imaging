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

## Executable plugin / media-card example

Build `apps/ImagingPluginDemo` with GitHubOut / CLANGx64 / +GUI. Choose a format,
load a small local image, switch Contain/Cover and use Copy C++. The code pane
shows a standalone program with the exact package/header, concrete raster class,
FileIn loading and UiMediaCard::SetImage. It uses the same format/path/fit as the
preview. UiMediaCard is the display control; the raster plugin does decoding.

```cpp
#include <Ui/Ui.h>
#include <plugin/imaging_jpeg/imaging_jpeg.h>
// .upp uses: Ui, plugin/imaging_jpeg
FileIn input(local_jpeg_path);
ImagingJPEGRaster reader;
if(input.IsOpen() && reader.Open(input))
    media_card.SetImage(reader.GetImage()); // media_card is a live UiMediaCard
```

The direct reader keeps this plugin's stream limits; LoadFileAny may dispatch
to other registered Draw readers with their own policies. Controls must outlive
the window's Run(). The full styling/PropertyEditor demonstration remains
`upp_Ui/examples/UiMediaCardDemo`; the imaging demo concentrates on providers.

For full-fidelity images, add `ImagingIO` and call
`Upp::Imaging::LoadImageFile(path, image_data)`. EXR/HDR floating-point data
keeps values outside the display range; StreamRaster preview is RGBA8 and
clamps to its display range. Workbench offers exposure and OCIO view controls.
JPEG core saving accepts UInt8 Gray/RGB; alpha is rejected explicitly.
Workbench Save offers EXR, PNG, JPEG, JPEG XL, TIFF, WebP, Radiance HDR and DPX. EXR preserves original channels; other choices export the selected source group at full resolution, without baking exposure/gamma/OCIO preview. JPEG/HDR/DPX require RGB without alpha; JPEG is lossy and 8-bit, WebP is 8-bit, DPX is 16-bit.

For video, add `ImagingVideo`, include `<ImagingVideo/ImagingVideo.h>`, open a
local MP4/MOV with `Upp::Imaging::VideoReader`, then call `ReadNext` for U++
images/timestamps or `Seek` to select a time. The current backend is H.264,
not arbitrary MOV/MP4 codecs, audio or HDR video.

Generated small image/video samples for manual loading are retained under
`build/windows-x64/samples/`. JPEG XR is a different format from EXR and is
deferred. XML colour files (CTF/CLF/CDL/CC/CCC/look) are disabled; YAML configs,
non-XML LUTs and programmatic colour transforms remain supported.

See [MAINTAINING.md](MAINTAINING.md) for physical package paths and
[BUILD_AND_RUN.md](BUILD_AND_RUN.md) for the assembly and output directories.

## Full-fidelity image I/O

Add ImagingIO to .upp uses:

```cpp
#include <ImagingIO/ImagingIO.h>
using namespace Upp;
using namespace Upp::Imaging;
ImageData image;
Diagnostics diagnostics;
Result result = LoadImageFile("input.exr", image, &diagnostics);
if(result.IsOk())
    result = SaveImageFile("copy.exr", image, &diagnostics);
```

Inspect results/diagnostics. Load preserves output on failure. Save validates
sample/channel policy and publishes through a verified temporary file.
ImagingColor supplies transforms; ImagingAnalysis and ImagingDiagnostics provide
statistics and comparisons. Direct native API consumers add OpenImageIO,
OpenColorIO or the relevant codec package instead of implementation source owners.

## Reusable video reader

```cpp
#include <ImagingVideo/ImagingVideo.h>
Upp::Imaging::VideoReader reader;
Upp::Imaging::VideoFrame frame;
if(reader.Open("clip.mp4"))
    while(reader.ReadNext(frame)) {
        // frame.image is RGBA8; frame.time_ms is the timestamp.
    }
// Distinguish clean EOF with IsEof() from failure with GetError().
```

Add ImagingVideo to .upp uses. Seek(milliseconds) selects a time on subsequent
reading. See [Video API](../imaging/ImagingVideo/README.md) for limits and errors.
Additional preview packages are plugin/imaging_tiff, plugin/imaging_webp,
plugin/imaging_cineon, plugin/imaging_raw and plugin/imaging_heif; each exposes the matching imaging_<format>.h header
under its plugin package. The shared [ImagingRaster API](../integrations/ImagingRaster/README.md)
describes explicit reader limits and preview layout restrictions.
