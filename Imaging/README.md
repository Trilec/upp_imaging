# Imaging

Convenience umbrella for applications that want the standard complete backend-neutral U++ Imaging framework.

Include:

```cpp
#include <Imaging/Imaging.h>
```

The umbrella forwards the public contracts of `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, and `ImagingDiagnostics`. It owns no additional image state or implementation policy.

Including `Imaging` brings the validated OpenImageIO and OpenColorIO backends transitively through `ImagingIO` and `ImagingColor`. It does not include optional raster plugins such as `plugin/exr`.

Applications wanting a smaller dependency set should include the individual framework packages directly.
