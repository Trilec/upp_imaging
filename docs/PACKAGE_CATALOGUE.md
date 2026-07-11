# Package Catalogue

Primary navigation for `upp_imaging`.

## How to read this

- public packages are the ones ordinary application code should use directly
- strict `_src` packages compile pinned upstream sources and are not for app code
- narrow helpers expose only the supported file subset for a format
- probes check package/header/object boundaries quickly
- tests validate behavior and numbers; viewers are supplementary

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

### `opencolorio`
- Purpose: stable OpenColorIO user-facing package
- Pinned upstream version: 2.5.2
- Public include route: `#include <opencolorio/OpenColorIO.h>`
- Direct package dependency: `opencolorio_src`
- Implementation or delegate: delegates
- Primary validation target: `opencolorio_test`, `opencolorio_gui_link_test`
- Current status: packaged and validated under CLANGx64

### `openimageio_prereq_test`
- Purpose: coexistence test for the planned OpenImageIO prerequisite stack
- Depends on: `openexr`, `imath`, `zlib`, `libpng`, `libjpeg_turbo`, `libtiff`, `opencolorio`, `fmt`, `robinmap`
- Primary validation target: `openimageio_prereq_test`
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

### OpenImageIO prerequisites
- `openimageio_prereq_test`

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

`opencolorio_src` is the strict imported-source boundary. `opencolorio` is the stable application-facing wrapper.

### Future OpenImageIO stack

```text
application
    ↓
future openimageio
    ↓
future openimageio_src
    ↓
stable codec/dependency packages
```

Currently proven likely prerequisites:

- `openexr`
- `imath`
- `zlib`
- `libpng`
- `libjpeg_turbo`
- `libtiff`
- `opencolorio`
- `fmt`
- `robinmap`

Planned only. `openimageio` and `openimageio_src` do not exist yet.

## Repository strategy

`upp_imaging` remains one U++ nest containing many independently usable packages.

Separate `upp_openimageio` or `upp_opencolorio` repositories are not being created yet.

A later split may be considered only if release cadence, ownership, distribution, or application integration needs justify it.

Stable package boundaries provide the useful separation now without duplicating vendored source or coordinating multiple repositories.
