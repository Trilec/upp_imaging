# Usage

Configure the logical nests using [GitHubOut.var.example](../GitHubOut.var.example). Package names remain independent of directory layout. [Build and run](BUILD_AND_RUN.md) defines `build/<platform>/` for disposable output and `bin/<platform>/` for verified Release applications; update the active local assembly, not just its repository example.

## Framework image I/O

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

Inspect the result and structured diagnostics. Loading preserves the caller's output on failure. Saving validates format policy and publishes through a same-directory temporary file after readability verification. Choose an extension supported by the source sample type and channel layout; conversion is not implicit.

Use `ImagingColor` for backend-neutral color transforms, `ImagingAnalysis` for source statistics and `ImagingDiagnostics` for comparisons. The [framework tests](../tests/) show complete configuration and error-handling examples.

## Native API access

Use `<OpenImageIO/OIIO.h>`, `<OpenColorIO/OpenColorIO.h>`, `<openexr/Imf.h>` or the relevant public codec header when native APIs are required. Add the corresponding public package to `uses`. Source-owner packages are implementation details unless deliberately testing the standalone provider route.

JPEG is available through ImagingIO, plugin/imaging_jpeg and the direct libjpeg_turbo API. FFmpeg also backs the reusable ImagingVideo reader; see [FFmpeg](../third_party/ffmpeg/FFmpeg/README.md).

See [FORMAT_QUICKSTART.md](FORMAT_QUICKSTART.md) and [ImagingPluginDemo](../apps/ImagingPluginDemo/) for a real file-loading UiMediaCard example with copyable C++.

## U++ display integration

Add a format package from the `integrations/` nest and include its public header. EXR retains `plugin/exr` and `<plugin/exr/exr.h>`. Other packages use `<plugin/imaging_<format>/imaging_<format>.h>`.

| Format | Package | Direct raster class |
| --- | --- | --- |
| EXR | `plugin/exr` | `EXRRaster` |
| PNG | `plugin/imaging_png` | `ImagingPNGRaster` |
| JXL | `plugin/imaging_jxl` | `ImagingJXLRaster` |
| HDR | `plugin/imaging_hdr` | `ImagingHDRRaster` |
| DPX | `plugin/imaging_dpx` | `ImagingDPXRaster` |
| Cineon | `plugin/imaging_cineon` | `ImagingCineonRaster` |
| RAW | `plugin/imaging_raw` | `ImagingRAWRaster` |
| WebP | `plugin/imaging_webp` | `ImagingWebPRaster` |
| HEIF / HEIC / AVIF | `plugin/imaging_heif` | `ImagingHEIFRaster` |
| TIFF | `plugin/imaging_tiff` | `ImagingTIFFRaster` |

Example with `plugin/imaging_webp` in the application's `uses`:

```cpp
#include <plugin/imaging_webp/imaging_webp.h>
using namespace Upp;
Image image = StreamRaster::LoadFileAny("sample.webp");
```

The adapters share [ImagingRaster](../integrations/ImagingRaster/README.md), produce RGBA8 preview, and reject deep, volume, multipart and mipmapped images. Use ImagingIO to preserve full-fidelity typed data. Direct adapter instances support lower input/allocation budgets through `SetLimits` before `Open`; other registered U++ readers retain their own policies.

Run `imaging_raster_test` to regenerate small manual samples under `build/windows-x64/samples/`. It includes eleven positive image formats across ten reader families, with original synthetic Cineon and linear camera DNG fixtures. The [test README](../tests/imaging_raster_test/README.md) records their scope.

## Interactive tools

Build `ImagingWorkbench` from the `apps/` nest. The workbench provides channel inspection, bounded previews, histograms, OCIO displays and EXR/PNG roundtrips. Build `imaging_workbench_bench` from `tools/` for manual timings; `--quick` selects its shorter run.

ImagingWorkbench is the primary interactive integration check, not a substitute for the deterministic acceptance suite. Its normal Windows launch location after local validation is `bin/windows-x64/ImagingWorkbench.exe`; Debug and benchmark staging stay in `build/windows-x64/`. Keep logs and user-generated images out of `bin/`.

## U++ video frames

Add `ImagingVideo` to `uses` and include `<ImagingVideo/ImagingVideo.h>`:

```cpp
Upp::Imaging::VideoReader reader;
Upp::Imaging::VideoFrame frame;
if(reader.Open("clip.mp4"))
    while(reader.ReadNext(frame)) {
        // frame.image is a U++ RGBA8 Image; frame.time_ms is its timestamp.
    }
// Distinguish clean EOF (IsEof()) from failure (GetError()).
```

Local H.264 MP4/MOV only. `Seek(milliseconds)` discards earlier frames on the
next read. Set explicit `VideoLimits` when opening if the default budget differs
from your workload. See [API and limits](../imaging/ImagingVideo/README.md).
Generate manual MP4/MOV samples with `imaging_video_test`. Workbench offers
Play/Pause, Next and Restart; Space/Right/Home are their keyboard equivalents.
