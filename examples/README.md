# Framework usage

These snippets use the `Imaging` package from the `imaging/` nest. This directory contains documentation examples and is not an additional U++ nest.

```cpp
#include <Imaging/Imaging.h>

using namespace Upp;
using namespace Upp::Imaging;

ImageData image;
Diagnostics diagnostics;
Result loaded = LoadImageFile("input.exr", image, &diagnostics);
if(loaded.IsOk()) {
    Result saved = SaveImageFile("output.exr", image, &diagnostics);
    // Inspect saved and diagnostics before reporting success.
}
```

Use ImagingWorkbench in `apps/` for interactive inspection. Full runnable examples of supported formats and error handling are in `tests/`.
