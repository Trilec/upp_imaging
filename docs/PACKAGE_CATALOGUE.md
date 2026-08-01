# Package Catalogue

Primary navigation for `upp_imaging`.

## How to read this

- direct packages are for programmers who want the established native APIs directly
- `_src` packages compile pinned upstream sources and are not for app code
- narrow helpers expose only the supported file subset for a format
- framework packages provide the backend-neutral U++ Imaging API under `Upp::Imaging`
- raster plugins under `plugin/*` integrate supported formats into ordinary U++ `Upp::Image` workflows
- probes check package/header/object boundaries quickly
- tests validate behavior and numbers
- the diagnostic application `ImagingWorkbench` exercises the full stack visually

## Public application-facing packages

### `openexr`
- Purpose: stable high-level OpenEXR API for applications
- Pinned upstream version: 3.4.13
- Public include route: `#include <openexr/Imf.h>`
- Direct package dependency: `openexr_src`
- Implementation or delegate: delegates; no second copy of OpenEXR sources
- Primary validation target: `openexr_test`
- Current status: stable high-level boundary complete

### `openexr_core`
- Purpose: stable OpenEXRCore C API for applications
- Pinned upstream version: 3.4.13
- Public include route: `#include <openexr_core/openexr.h>`
- Direct package dependency: `openexr_core_src`
- Implementation or delegate: delegates
- Primary validation target: `openexr_core_write_probe`, `openexr_core_roundtrip_test`
- Current status: scanline RGBA subset complete

### `OpenColorIO`
- Purpose: canonical OpenColorIO user-facing package
- Pinned upstream version: 2.5.2
- Public include route: `#include <OpenColorIO/OpenColorIO.h>`
- Direct package dependency: `opencolorio_src`
- Implementation or delegate: delegates
- Primary validation target: `opencolorio_test`, `opencolorio_gui_link_test`
- Current status: canonical public boundary complete; the old `opencolorio` public package name is intentionally not provided on Windows

### `openimageio_headers`
- Purpose: internal strict OpenImageIO public-header package
- Pinned upstream version: 3.1.15.0
- Public include route: `#include <OpenImageIO/imageio.h>` and related headers under `OpenImageIO/`
- Direct package dependency: `fmt`, `imath`, `libtiff`
- Implementation or delegate: owns the strict header tree only; exports the native include route; compiles no OIIO source; does not register plugins
- Primary validation target: indirectly used by `openimageio_src`, `openimageio_util_src`
- Current status: internal routing package separated so the public `OpenImageIO` wrapper can be created later

### `OpenImageIO`
- Purpose: canonical public OpenImageIO application-facing package
- Pinned upstream version: 3.1.15.0
- Public include route: `#include <OpenImageIO/OIIO.h>`
- Direct package dependency: `openimageio_headers`, `openimageio_src`, `openimageio_plugin_openexr`, `openimageio_plugin_png`
- Depends on the source implementation and the static EXR/PNG registration packages
- Implementation or delegate: delegates; static OpenEXR and PNG plugins are integrated
- Primary validation target: `openimageio_io_test`
- Current status: source/application boundary complete; OpenEXR and PNG integration validated; JPEG, TIFF and other formats not validated; dynamic plugin loading not validated
- Validated OIIO formats: **OpenEXR** and **PNG** only.

### `oiio`
- Purpose: temporary compatibility-forwarding package for `OpenImageIO`
- Public include route: `#include <oiio/OIIO.h>`
- Direct package dependency: `OpenImageIO`
- Implementation or delegate: forwards only; no implementation source
- Validated OIIO formats: **OpenEXR** and **PNG** only. Other formats must not be described as available through OIIO until their plugins are compiled, registered and tested.

### `openimageio_io_test`
- Purpose: validates the EXR/PNG integration path of the `oiio` package
- Depends on: `oiio`
- Primary validation target: `openimageio_io_test`
- Current status: passes under CLANGx64

### `imath`
- Purpose: stable Imath math and half-float package
- Pinned upstream version: 3.2.2
- Public include route: `#include <imath/half.h>` and `#include <imath/ImathVec.h>`
- Direct package dependency: `imath_src`
- Implementation or delegate: delegates
- Primary validation target: `imath_test`
- Current status: foundation complete

### `zlib`
- Purpose: stable zlib compatibility package for normal U++ applications
- Pinned upstream version: 1.3.2 strict / 1.3.1 Windows provider
- Public include route: `#include <zlib/zlib.h>`
- Direct package dependency: `zlib_src` on non-Windows, `plugin/z` on Windows/Core
- Implementation or delegate: platform split; stable wrapper stays public while the Windows provider is the U++ compatibility path
- Primary validation target: `zlib_test`
- Current status: foundation complete

### `libpng`
- Purpose: stable libpng package for applications
- Pinned upstream version: 1.6.58
- Public include route: `#include <libpng/png.h>`
- Direct package dependency: `zlib`
- Implementation or delegate: contains imported libpng implementation compiled against `zlib`
- Primary validation target: `libpng_test`, `libpng_roundtrip_test`
- Current status: RGBA8 subset complete

### `libjpeg_turbo`
- Purpose: stable libjpeg-turbo package for applications
- Pinned upstream version: 3.2.0
- Public include route: `#include <libjpeg_turbo/jpeglib.h>`
- Direct package dependency: `libjpeg_turbo_src`
- Implementation or delegate: delegates
- Primary validation target: `libjpeg_turbo_test`
- Current status: RGB8 baseline complete

### `libtiff`
- Purpose: stable libtiff package for applications
- Pinned upstream version: 4.7.2
- Public include route: `#include <libtiff/tiff.h>` and `#include <libtiff/tiffio.h>`
- Direct package dependency: `libtiff_src`
- Implementation or delegate: delegates
- Primary validation target: `libtiff_test`, `tiff_io_test`
- Current status: typed RGBA subset complete

### `libdeflate`
- Purpose: stable libdeflate package for applications
- Pinned upstream version: 1.25
- Public include route: `#include <libdeflate/libdeflate.h>`
- Direct package dependency: `libdeflate_src`
- Implementation or delegate: delegates
- Primary validation target: `libdeflate_test`
- Current status: foundation complete

### `openjph`
- Purpose: stable OpenJPH package for applications
- Pinned upstream version: 0.26.3
- Public include route: `#include <openjph/openjph.h>`
- Direct package dependency: `openjph_src`
- Implementation or delegate: delegates
- Primary validation target: `openjph_test`
- Current status: foundation complete

### `fmt`
- Purpose: stable fmt formatting package for applications
- Pinned upstream version: 12.2.0
- Public include route: `#include <fmt/format.h>`
- Direct package dependency: `fmt_src`
- Implementation or delegate: delegates
- Primary validation target: `fmt_test`
- Current status: header-only stable package

### `robinmap`
- Purpose: stable robin-map package for applications
- Pinned upstream version: 1.4.1
- Public include route: `#include <robinmap/robin_map.h>`
- Direct package dependency: `robinmap_src`
- Implementation or delegate: delegates
- Primary validation target: `robinmap_test`
- Current status: header-only stable package

## Upp::Imaging framework (planned)

All framework public types live under the `Upp::Imaging` namespace. Public headers must not expose `OIIO::*` or `OCIO::*` types.

### `ImagingCore` (planned)
- Purpose: backend-neutral image data model and result contracts
- Public include route: `#include <ImagingCore/ImagingCore.h>`
- Depends on: U++ Core only
- Must not depend on: OpenImageIO, OpenColorIO, CtrlLib, ImagingWorkbench, `plugin/exr`
- Public concepts: `ImageSpec`, `ImageBuffer`, `ImageData`, `Metadata`, `DataWindow`, `SampleType`, `ChannelLayout`, `Result`, `Diagnostics`
- Current status: not implemented; design target documented

### `ImagingIO` (planned)
- Purpose: full-fidelity U++ image loading and saving
- Planned with OpenImageIO as its initial backend
- Public include route: `#include <ImagingIO/ImagingIO.h>`
- Planned dependency set: `ImagingCore`, `OpenImageIO`
- Public headers expose only `Upp::Imaging` types
- Current status: not implemented; design target documented

### `ImagingColor` (planned)
- Purpose: backend-neutral colour-processing operations
- Planned with OpenColorIO as its initial backend
- Public include route: `#include <ImagingColor/ImagingColor.h>`
- Planned dependency set: `ImagingCore`, `OpenColorIO`
- Public headers must not expose OCIO processor, config or transform types
- Current status: not implemented; design target documented

### `ImagingAnalysis` (planned)
- Purpose: reusable image-analysis algorithms
- Public include route: `#include <ImagingAnalysis/ImagingAnalysis.h>`
- Depends on: `ImagingCore`
- Initial scope: histograms, channel statistics, source probes, finite and non-finite value handling
- Later scope: waveform analysis, vectorscope analysis, other reusable scopes
- Current status: not implemented; design target documented

### `ImagingDiagnostics` (planned)
- Purpose: shared structured validation and reporting
- Public include route: `#include <ImagingDiagnostics/ImagingDiagnostics.h>`
- Depends on: `ImagingCore`
- GUI-independent
- Must not depend on: CtrlLib, ImagingWorkbench
- Supports: deterministic package tests, readable console reports, numerical comparisons, image specification reports, metadata reports, channel and sample-type reports, timing and operation diagnostics
- Current status: not implemented; design target documented

### `Imaging` (planned)
- Purpose: convenience umbrella for applications wanting the standard complete U++ Imaging framework
- Public include route: `#include <Imaging/Imaging.h>`
- Planned dependency set: `ImagingCore`, `ImagingIO`, `ImagingColor`, `ImagingAnalysis`, `ImagingDiagnostics`
- Because `ImagingIO` and `ImagingColor` planned initial backends are OpenImageIO and OpenColorIO, including `Imaging` brings those standard backends
- Applications needing a lighter dependency set may include only `ImagingCore`, `ImagingIO`, `ImagingColor` or `ImagingAnalysis` individually
- Must not automatically include `plugin/exr`
- Current status: not implemented; design target documented

## U++ raster integration plugins (planned)

These plugins are opt-in and display-oriented. They are not the full-fidelity APIs.

### `plugin/exr` (planned)
- Purpose: integrate EXR files into ordinary U++ `StreamRaster` and `Upp::Image` workflows
- Eventually allows display-oriented use such as loading an EXR preview into `Upp::Image`
- Not the full-fidelity EXR API
- May convert floating-point pixels to display pixels, selected RGB or RGBA channels to `Upp::Image`, HDR values through a defined display policy
- Must not claim to preserve arbitrary EXR channels, full source metadata, unmodified half or float pixels, source data-window semantics, or deep or multipart EXR data
- Must remain opt-in so it does not silently change normal U++ raster-loading behaviour
- Current status: not implemented
- Reserved future pattern: `plugin/exr`, `plugin/jxl`, `plugin/hdr`

## Narrow format IO helpers

### `openexr_io`
- Supported data model: scanline RGBA, HALF/FLOAT
- Supported file subset: NONE and ZIP compression only, one image, simple row-major round-trip
- Underlying stable package: `openexr_core`
- Primary test: `openexr_io_test`
- Current limitations: narrow helper only; no tiled, multipart, deep, metadata, or arbitrary-channel support
- Note: `openexr` is the direct high-level upstream API; `openexr_io` is the separate U++-friendly helper

### `png_io`
- Supported data model: RGBA8
- Supported file subset: ordinary PNG read/save for the tested subset
- Underlying stable package: `libpng`
- Primary test: `png_io_test`
- Current limitations: metadata, ICC, gamma policy, and source bit depth are not preserved

### `jpeg_io`
- Supported data model: RGB8
- Supported file subset: baseline JPEG read/save for the tested subset
- Underlying stable package: `libjpeg_turbo`
- Primary test: `jpeg_io_test`
- Current limitations: lossy by design; not a general metadata-preserving wrapper

### `tiff_io`
- Supported data model: typed RGBA scanline TIFF
- Supported file subset: classic single-directory scanline TIFF only
- Underlying stable package: `libtiff`
- Primary test: `tiff_io_test`
- Current limitations: no tiled, planar-separate, palette, grayscale, CMYK, metadata, ICC, or BigTIFF claim

## Strict imported-source packages

Ordinary applications must not depend directly on `_src` packages.

| Package | Upstream project and version | Why the strict package exists | Direct dependencies | Strict validation target | App use |
| --- | --- | --- | --- | --- | --- |
| `zlib_src` | zlib 1.3.2 | pinned source proof for the compression base | none | `zlib_src_test` | no |
| `libpng_src` | libpng 1.6.58 | pinned source proof for PNG | `zlib_src` | `libpng_src_test`, `libpng_src_roundtrip_test` | no |
| `imath_src` | Imath 3.2.2 | pinned source proof for math/half support | none | `imath_src_test` | no |
| `libdeflate_src` | libdeflate 1.25 | pinned standalone compression source proof | none | `libdeflate_src_test` | no |
| `openjph_src` | OpenJPH 0.26.3 | pinned source proof for HTJ2K | none | `openjph_src_test` | no |
| `iex_src` | OpenEXR 3.4.13 stack | strict exception/error layer for OpenEXR | none | `iex_src_test` | no |
| `ilmthread_src` | OpenEXR 3.4.13 stack | strict threading layer for OpenEXR | `iex_src` | `ilmthread_src_test` | no |
| `openexr_core_src` | OpenEXR 3.4.13 | lower-level OpenEXRCore C layer | `imath_src`, `ilmthread_src`, `libdeflate_src`, `openjph_src` | `openexr_core_src_probe`, `openexr_core_write_probe`, `openexr_core_roundtrip_test` | no |
| `openexr_src` | OpenEXR 3.4.13 | high-level OpenEXR C++ API source boundary | `imath_src`, `iex_src`, `ilmthread_src`, `openexr_core_src` | `openexr_src_probe`, `openexr_src_test` | no |
| `libjpeg_turbo_src` | libjpeg-turbo 3.2.0 | pinned source proof for JPEG | none | `libjpeg_turbo_src_test` | no |
| `libtiff_src` | libtiff 4.7.2 | pinned source proof for TIFF | `zlib`, `libdeflate` | `libtiff_src_test` | no |
| `opencolorio_src` | OpenColorIO 2.5.2 | pinned source proof for OCIO | `expat`, `yaml_cpp`, `pystring`, `minizip_ng`, `imath`, `zlib` | `opencolorio_src_test` | no |
| `openimageio_src` | OpenImageIO 3.1.15.0 | pinned source proof for OpenImageIO | `openexr`, `OpenColorIO`, `imath`, `zlib`, `libpng`, `libjpeg_turbo`, `libtiff`, `fmt`, `robinmap` | `openimageio_src_test`, `openimageio_io_test` | no |
| `fmt_src` | fmt 12.2.0 | pinned header-only source proof | none | `fmt_src_test` | no |
| `robinmap_src` | robin-map 1.4.1 | pinned header-only source proof | none | `robinmap_src_test` | no |

## Tests and probes

### OpenEXR stack
- Probes: `openexr_src_probe`, `openexr_core_src_probe`
- Strict tests: `openexr_src_test`, `openexr_core_write_probe`, `openexr_core_roundtrip_test`
- Stable tests: `openexr_test`, `openexr_core_write_probe`, `openexr_core_roundtrip_test`, `openexr_io_test`
- Role split: probe = package/header/object boundary check; test = file or numerical validation

### PNG
- `libpng_src_test`, `libpng_test`, `libpng_src_roundtrip_test`, `libpng_roundtrip_test`, `png_io_test`, `upp_png_plugin_test`
- `probe` vs `test`: source validation stays strict; helper tests validate the U++-friendly path

### JPEG
- `libjpeg_turbo_src_test`, `libjpeg_turbo_test`, `jpeg_io_test`, `libjpeg_turbo_gui_link_test`

### TIFF
- `libtiff_src_test`, `libtiff_test`, `tiff_io_test`, `libtiff_gui_link_test`

### OpenColorIO
- `opencolorio_src_test`, `opencolorio_test`, `opencolorio_gui_link_test`
- Integration checks: `ocio_dependencies_test`, `ocio_dependencies_gui_link_test`

### OpenImageIO
- `openimageio_prereq_test`, `openimageio_io_test`
- EXR and PNG integration validated; JPEG/TIFF not yet routed through OIIO; dynamic plugin loading not validated

### Dependency foundations
- `zlib_src_test`, `zlib_test`
- `imath_src_test`, `imath_test`
- `libdeflate_src_test`, `libdeflate_test`
- `openjph_src_test`, `openjph_test`

### Shared integration and viewer tests
- `imaging_roundtrip_test_support`: shared numerical and chart support
- `imaging_roundtrip_viewer`: diagnostic viewer; supplementary only
- `imaging_roundtrip_viewer_ocio`: viewer variant with OCIO preview
- `imaging_roundtrip_viewer_ocio_smoke_test`: smoke validation for the viewer path

## Shared infrastructure

- `imaging_roundtrip_test_support` is the common numerical test utility layer
- numerical tests determine PASS/FAIL
- viewer inspection is supplementary and never overrides the comparison result
- `imaging_roundtrip_viewer` is a diagnostic application, not a correctness authority

## Internal compatibility and forwarding headers

Repository-root shims:

- `half.h`
- `ImathBox.h`
- `ImathVec.h`

These are internal include-routing compatibility shims.

- they forward to `imath_src`
- application code should not treat them as public standalone APIs
- they exist so upstream OpenEXR angled includes can resolve without consumer-level strict source paths
- they must remain minimal

Stable OpenEXR bridge headers:

- `openexr/half.h`
- `openexr/ImathBox.h`
- `openexr/ImathVec.h`

These are not new implementations. They are the stable package's internal bridge layer into `openexr_src`.

## Dependency maps

### Stable OpenEXR stack

```text
application
    ↓
openexr
    ↓
openexr_src
    ├── imath_src
    ├── iex_src
    ├── ilmthread_src
    └── openexr_core_src
```

Separate narrow helper:

```text
application
    ↓
openexr_io
    ↓
openexr_core
```

### OpenColorIO stack

```text
application
    ↓
opencolorio
    ↓
opencolorio_src
    ├── expat
    ├── yaml_cpp
    ├── pystring
    ├── minizip_ng
    ├── imath
    └── zlib
```

`opencolorio_src` is the strict imported-source boundary. `OpenColorIO` is the stable application-facing wrapper. The public rename is deliberately breaking because Windows cannot host case-only package directories.

### OpenImageIO stack

```text
application
    ↓
OpenImageIO
    ↓
openimageio_src
    ↓
stable codec/dependency packages
```

Currently integrated dependencies:

- `openexr`
- `OpenColorIO`
- `imath`
- `zlib`
- `libpng`
- `libjpeg_turbo`
- `libtiff`
- `fmt`
- `robinmap`

Static OpenEXR and PNG plugins are integrated into the OpenImageIO build. JPEG and TIFF are not yet routed through the OIIO boundary. Dynamic plugin loading is not validated.

Currently validated OIIO formats: **OpenEXR** and **PNG**. JPEG, TIFF and other formats are not described as available through OIIO until their OIIO plugins are compiled, registered and tested.

### Planned Upp::Imaging stack

```text
application
    ↓
Imaging (umbrella)
    ↓
ImagingCore, ImagingIO, ImagingColor, ImagingAnalysis, ImagingDiagnostics
    ├── ImagingIO    → oiio (OpenImageIO) for full-fidelity load/save
    ├── ImagingColor → OpenColorIO for colour processing
    └── ImagingCore  → Core only, no OIIO or OCIO dependency
```

`ImagingCore` does not depend on `oiio` or `OpenColorIO`. Applications needing only the backend-neutral data model can include `ImagingCore` alone.

### Planned plugin/* stack

```text
plugin/exr
    ↓
openexr (or OpenImageIO directly for the display conversion)
    ↓
openexr_src (or oiio → openimageio_src)
```

`plugin/exr` is an opt-in display-oriented integration with `Upp::Image`. It is not the full-fidelity EXR API.

## Repository strategy

`upp_imaging` remains one U++ nest containing many independently usable packages.

Separate `upp_openimageio` or `upp_opencolorio` repositories are not being created yet.

A later split may be considered only if release cadence, ownership, distribution, or application integration needs justify it.

Stable package boundaries provide the useful separation now without duplicating vendored source or coordinating multiple repositories.
