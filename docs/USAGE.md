# Usage

Configure the logical nests using [GitHubOut.var.example](../GitHubOut.var.example). Package names remain independent of directory layout.

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

JPEG is available through libjpeg_turbo. It is not part of the ImagingIO format policy. FFmpeg is a separate direct media API; see [FFmpeg](../third_party/ffmpeg/FFmpeg/README.md).

## U++ display integration

Add `plugin/exr` from the `integrations/` nest to register EXR StreamRaster preview. This produces display-oriented U++ images; use ImagingIO to preserve full-fidelity typed image data.

## Interactive tools

Build `ImagingWorkbench` from the `apps/` nest. The workbench provides channel inspection, bounded previews, histograms, OCIO displays and EXR/PNG roundtrips. Build `imaging_workbench_bench` from `tools/` for manual timings; `--quick` selects its shorter run.
