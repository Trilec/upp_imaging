# OpenImageIO 3.1.15.0 Source Dependency Audit

## Upstream authority

- Pinned release: `v3.1.15.0`
- Checked-out commit: `cbe57bc005678ca310835473568121719861734c`
- Upstream project: `https://github.com/AcademySoftwareFoundation/OpenImageIO`

Authority files inspected:

- `CMakeLists.txt`
- `src/libutil/CMakeLists.txt`
- `src/libOpenImageIO/CMakeLists.txt`
- `src/include/CMakeLists.txt`
- `src/cmake/externalpackages.cmake`
- `src/cmake/add_oiio_plugin.cmake`
- `src/cmake/dependency_utils.cmake`
- `src/cmake/compiler.cmake`
- `src/cmake/Config.cmake.in`
- `src/build-scripts/OpenImageIO.pc.in`
- `src/build-scripts/version_win32.rc.in`
- `src/openexr.imageio/CMakeLists.txt`
- `src/png.imageio/CMakeLists.txt`
- `src/jpeg.imageio/CMakeLists.txt`
- `src/tiff.imageio/CMakeLists.txt`

## Upstream target boundaries

### `OpenImageIO_Util`

- Purpose: utility/runtime support layer
- Source manifest location: `src/libutil/CMakeLists.txt:set(libOpenImageIO_Util_srcs ...)`
- Source count: 18 `.cpp`
- Sources: `argparse.cpp`, `benchmark.cpp`, `errorhandler.cpp`, `farmhash.cpp`, `filesystem.cpp`, `fmath.cpp`, `filter.cpp`, `hashes.cpp`, `paramlist.cpp`, `plugin.cpp`, `SHA1.cpp`, `strutil.cpp`, `sysutil.cpp`, `thread.cpp`, `timer.cpp`, `typedesc.cpp`, `ustring.cpp`, `xxhash.cpp`
- Public headers: `OpenImageIO/argparse.h`, `benchmark.h`, `errorhandler.h`, `filter.h`, `filesystem.h`, `fmath.h`, `hash.h`, `optparser.h`, `parallel.h`, `paramlist.h`, `plugin.h`, `refcnt.h`, `span.h`, `string_view.h`, `strutil.h`, `strongparam.h`, `thread.h`, `timer.h`, `typedesc.h`, `ustring.h`, `vecparam.h`
- Internal headers: `src/libutil/SHA1.h`, `src/libutil/stb_sprintf.h`
- Generated/configured headers: `version_win32.rc` on Windows only
- Direct dependencies: `Threads::Threads`, `Imath::Imath`, `fmt::fmt-header-only` (or internalized fmt headers), `GCC_ATOMIC_LIBRARIES` when needed
- Optional deps: `TBB::tbb` private only
- Compile definitions: `OpenImageIO_Util_EXPORTS`; `OIIO_STATIC_DEFINE=1` for static builds; `WIN32_LEAN_AND_MEAN`, `NOMINMAX`, `NOGDI`, `VC_EXTRALEAN` on Windows
- Link libraries: `Threads::Threads`, `Imath::Imath`, `fmt`; `psapi` on Windows; `TBB::tbb` only if present
- Separate U++ package: yes, should be `openimageio_util_src`

### `OpenImageIO`

- Purpose: main image I/O API layer
- Source manifest location: `src/libOpenImageIO/CMakeLists.txt:set(libOpenImageIO_srcs ...)`
- Embedded sibling sources: `src/libtexture/texturesys.cpp`, `texture3d.cpp`, `environment.cpp`, `texoptions.cpp`, `imagecache.cpp`
- Source count: 38 `.cpp` entries in the target source list, before any embedded plugin sources
- Sources: `imagebufalgo.cpp`, `imagebufalgo_pixelmath.cpp`, `imagebufalgo_channels.cpp`, `imagebufalgo_compare.cpp`, `imagebufalgo_copy.cpp`, `imagebufalgo_deep.cpp`, `imagebufalgo_draw.cpp`, `imagebufalgo_addsub.cpp`, `imagebufalgo_muldiv.cpp`, `imagebufalgo_mad.cpp`, `imagebufalgo_minmaxchan.cpp`, `imagebufalgo_orient.cpp`, `imagebufalgo_xform.cpp`, `imagebufalgo_demosaic.cpp`, `imagebufalgo_yee.cpp`, `imagebufalgo_flip.cpp`, `deepdata.cpp`, `exif.cpp`, `exif-canon.cpp`, `formatspec.cpp`, `icc.cpp`, `imagebuf.cpp`, `imageinput.cpp`, `imageio.cpp`, `imageioplugin.cpp`, `imageoutput.cpp`, `iptc.cpp`, `xmp.cpp`, `color_ocio.cpp`, `maketexture.cpp`, `bluenoise.cpp`, `printinfo.cpp`, `oiio_gpu.cpp`, plus the 5 `src/libtexture/*.cpp` files above
- Public headers: `OpenImageIO/imageio.h`, `imagebuf.h`, `imagebufalgo.h`, `imagebufalgo_util.h`, `imagecache.h`, `imagebufalgo_opencv.h`, `color.h`, `deepdata.h`, `filesystem.h`, `plugin.h`, `thread.h`, `typedesc.h`, `ustring.h`, `strutil.h`, `span.h`, `image_span.h`, `memory.h`, `platform.h`, `paramlist.h`, `parallel.h`, `filter.h`, `fmath.h`, `simd.h`, `texture.h`, `sysutil.h`, `tiffutils.h`, `attrdelegate.h`
- Internal headers: `imageio_pvt.h`, `buildopts.h`, `imagecache_pvt.h`, `imagecache_memory_pvt.h`, `imagecache_memory_print.h`, `texture_pvt.h`, `imagebufalgo_demosaic_prv.h`, `exif.h`, `kissfft.hh`
- Generated/configured headers: `buildopts.h` from `src/libOpenImageIO/buildopts.h.in`; `version_win32.rc` on Windows; `oiioversion.h` from `src/include/OpenImageIO/oiioversion.h.in`; `OpenImageIOConfig.cmake`, `OpenImageIOConfigVersion.cmake`, `OpenImageIOTargets.cmake`, and `OpenImageIO.pc` are also generated/configured support files
- Direct dependencies: `OpenImageIO_Util`, `Imath::Imath`, `OpenEXR::OpenEXR`, `OpenColorIO::OpenColorIO`, `ZLIB::ZLIB`
- Optional deps: `pugixml` only when externalized, `TBB::tbb`, `Freetype::Freetype`, `BZip2::BZip2`, `CMAKE_DL_LIBS`, `psapi` on Windows, `ws2_32` on MinGW
- Compile definitions: `OpenImageIO_EXPORTS`; `OIIO_STATIC_DEFINE=1` for static builds; `OIIO_OPENEXR_CORE_DEFAULT=1` when OpenEXR >= 3.1.10; `OIIO_FREETYPE_VERSION`, `OIIO_PYTHON_VERSION`, `OIIO_QT_VERSION`, `OIIO_TBB_VERSION`; `EMBED_PLUGINS=1` only when plugins are embedded
- Separate U++ package: yes, should be `openimageio_src`

### Plugin targets

Upstream has 32 plugin directories under `src/*.imageio`, but the first milestone only needs the four baseline codecs:

- `src/openexr.imageio` -> `exrinput.cpp`, `exroutput.cpp`, optional `exrinput_c.cpp`
- `src/png.imageio` -> `pnginput.cpp`, `pngoutput.cpp`
- `src/jpeg.imageio` -> `jpeginput.cpp`, `jpegoutput.cpp`
- `src/tiff.imageio` -> `tiffinput.cpp`, `tiffoutput.cpp`

Each directory is handled by `add_oiio_plugin()`:

- with `EMBEDPLUGINS=ON`, the plugin sources are appended into `OpenImageIO`
- with `EMBEDPLUGINS=OFF`, each becomes a separate `MODULE` target

This release therefore supports either embedded or separate plugin packaging. For U++, separate plugin packages are the cleaner boundary.

Recommended future U++ plugin packages:

- `openimageio_plugin_openexr`
- `openimageio_plugin_png`
- `openimageio_plugin_jpeg`
- `openimageio_plugin_tiff`

## Minimum initial public surface

For the first no-I/O audit probe, the smallest usable surface is:

- `OpenImageIO/oiioversion.h`
- `OpenImageIO/export.h`
- `OpenImageIO/typedesc.h`
- `OpenImageIO/strutil.h`
- `OpenImageIO/filesystem.h`
- `OpenImageIO/ustring.h`

The core API adds:

- `OpenImageIO/imageio.h`
- `OpenImageIO/imagebuf.h`
- `OpenImageIO/imagebufalgo.h`
- `OpenImageIO/imagecache.h`
- `OpenImageIO/color.h`

Broader installed API exists, but it is not needed for the first strict compile-and-link milestone.

## Generated and configured headers

### `oiioversion.h`

- Template: `src/include/OpenImageIO/oiioversion.h.in`
- Values required for `3.1.15.0`: `OIIO_VERSION_MAJOR=3`, `OIIO_VERSION_MINOR=1`, `OIIO_VERSION_PATCH=15`, `OIIO_VERSION_TWEAK=0`, `OIIO_VERSION_RELEASE_TYPE` empty
- Namespace values: outer namespace defaults to `OpenImageIO`; inner namespace is `v3_1` because supported releases omit the patch from the versioned namespace
- Safe to pin in a repository package: yes, as a generated header mirroring the upstream release
- Recommended repository path: `openimageio_src/OpenImageIO/oiioversion.h`

### `buildopts.h`

- Template: `src/libOpenImageIO/buildopts.h.in`
- Values required: `OIIO_DEFAULT_PLUGIN_SEARCHPATH`, `OIIO_ALL_BUILD_DEPS_FOUND`
- `OIIO_DEFAULT_PLUGIN_SEARCHPATH` is safe to pin as empty for the first no-I/O milestone
- `OIIO_ALL_BUILD_DEPS_FOUND` can be left empty or populated from the package graph
- Recommended repository path: `openimageio_src/buildopts.h`

### `export.h`

- Upstream file exists in `src/include/OpenImageIO/export.h`
- A generated export header is optional (`USE_GENERATED_EXPORT_HEADER`), not required for the release build path
- Repository equivalent already exists upstream as source header, so no generated replacement is required for the first milestone

### Windows resource header

- Template: `src/build-scripts/version_win32.rc.in`
- Needed only for Windows library builds
- Recommended repository path: generated build output, not checked in

### Package config files

- `src/cmake/Config.cmake.in`
- `src/build-scripts/OpenImageIO.pc.in`

These are support files, not public headers, but they define the install-time package boundary.

## Mandatory dependency audit

### Core library requirements

Mandatory for `OpenImageIO_Util`:

- `Imath` -> mandatory, stable package available as `imath`
- `fmt` -> mandatory, stable package available as `fmt`
- `Threads` -> platform/runtime requirement

Mandatory for `OpenImageIO`:

- `Imath` -> mandatory, stable package available as `imath`
- `OpenEXR` -> mandatory, stable package available as `openexr`
- `zlib` -> mandatory, stable package available as `zlib`
- `OpenColorIO` -> mandatory, stable package available as `opencolorio`
- `TIFF` -> mandatory in upstream build system, stable package available as `libtiff`
- `fmt` -> mandatory, stable package available as `fmt`
- `Robinmap` -> mandatory, stable package available as `robinmap`
- `JPEG` family -> mandatory build dependency in upstream dependency discovery; stable package available as `libjpeg_turbo`

Not mandatory for the core library milestone:

- `PNG` -> optional plugin dependency
- `TBB` -> optional acceleration dependency
- `Freetype` -> optional feature dependency
- `BZip2` -> optional feature dependency
- `pugixml` -> optional external replacement for bundled copy

### Plugin dependencies

- `openexr.imageio` -> `OpenEXR`, `Imath`; optional `OpenEXRCore` path for `exrinput_c.cpp`
- `png.imageio` -> `PNG`, `ZLIB`
- `jpeg.imageio` -> `libjpeg-turbo` or `JPEG`; optional `libuhdr`
- `tiff.imageio` -> `TIFF`, `ZLIB`, `JPEG`

### Optional and disableable dependencies

These are optional in upstream and can stay disabled for the first strict milestone:

- `Boost` -> only needed by older `OpenVDB`
- `pugixml` -> optional external replacement
- `OpenCV` -> optional feature
- `FFmpeg` -> optional plugin/tool feature
- `WebP` -> optional plugin
- `OpenJPEG` -> optional plugin
- `Libheif` -> optional plugin
- `LibRaw` -> optional plugin
- `DCMTK` -> optional plugin
- `GIF` -> optional plugin
- `Ptex` -> optional plugin
- `Qt` -> viewer tool only
- `Python` -> bindings and tests only
- `TBB` -> optional acceleration dependency
- `CUDA` -> optional GPU path
- `OpenVDB` -> optional plugin/feature path
- `JXL` -> optional plugin
- `R3DSDK` -> optional tool/plugin path
- `Nuke` -> tool integration only
- `libuhdr` -> optional Ultra HDR path in JPEG plugin

### Missing or unmodeled dependencies

The following upstream packages are referenced but are not required for the first milestone:

- `OpenVDB`, `Ptex`, `Libheif`, `LibRaw`, `OpenJPEG`, `WebP`, `FFmpeg`, `DCMTK`, `GIF`, `JXL`, `OpenCV`, `Qt`, `Python`, `CUDA`, `Boost`

These can all remain disabled without blocking the first strict utility or core package milestone.

## Plugin model

OpenImageIO 3.1.15.0 supports both models:

- embedded plugins: `EMBEDPLUGINS=ON`
- separate module plugins: `EMBEDPLUGINS=OFF`

For a U++ package graph, separate plugin packages are the better boundary because they keep codec dependencies out of the core target.

Baseline codec package mapping:

- EXR plugin: separate package, optionally split further only if the C API path is needed
- PNG plugin: separate package
- JPEG plugin: separate package
- TIFF plugin: separate package

The core library does not need to load or write an image file for the first strict milestone.

## Strict versus stable dependency policy

Recommended policy for the first strict OpenImageIO packages:

- `openimageio_util_src` should depend on stable application-facing packages where they exist, especially `imath`, `fmt`, and the platform threading provider
- `openimageio_src` should depend on stable application-facing packages where they exist: `openexr`, `imath`, `zlib`, `libtiff`, `libjpeg_turbo`, `opencolorio`, `fmt`, `robinmap`
- plugin packages should depend on the corresponding stable codec packages and only on the minimal extra stable dependencies they need

Why this is the right split:

- it keeps the strict package honest about compiling pinned upstream code
- it avoids mixing strict and stable implementations of the same library in a normal app target
- it respects the existing Windows `zlib` provider split
- it keeps optional codec dependencies out of the core package boundary

Mixed-strategy note:

- Windows `zlib` must stay on the stable provider split already used by the repo
- no other mixed strict/stable dependency is required by the upstream evidence for the first milestone

## Windows and CLANGx64 audit

Required or relevant Windows definitions and behaviors:

- `OIIO_STATIC_DEFINE=1` for static builds
- `OpenImageIO_EXPORTS` and `OpenImageIO_Util_EXPORTS` only while building the libraries themselves
- `WIN32_LEAN_AND_MEAN`, `NOMINMAX`, `NOGDI`, `VC_EXTRALEAN` on the library targets
- `psapi` link dependency for Windows builds
- `ws2_32` only for MinGW builds
- `version_win32.rc.in` is the Windows version resource template
- `OPENIMAGEIO_PLUGIN_VERSION`/`OIIO_PLUGIN_VERSION` is the plugin ABI identity

Static-build risks:

- embedded plugins pull codec dependencies into the main library if `EMBEDPLUGINS=ON`
- separate module plugins are safer for U++ packaging
- `NOWARNINGS` is not part of the recommendation

Known warning risks:

- upstream may still emit nonportable include-path warnings from mixed-case header names on Windows
- those warnings do not change the dependency boundary

## SIMD and platform configuration

Safest initial `CLANGx64` configuration:

- `CMAKE_CXX_STANDARD=17`
- `ENABLE_FAST_MATH=OFF`
- `OIIO_USE_CUDA=OFF`
- leave SIMD/autodispatch at upstream defaults
- do not add architecture-specific AVX/SSE flags in the first milestone
- do not force `TEX_BATCH_SIZE`

What can remain disabled:

- aggressive SIMD tuning
- CUDA
- fast math
- architecture-specific compile flags

## Namespace and ABI configuration

- public outer namespace: `OpenImageIO`
- public alias: `OIIO`
- current inline namespace for this release: `v3_1`
- legacy namespace kept for compatibility: `v3_0`
- plugin ABI version: `OIIO_PLUGIN_VERSION 27`
- shared-library `SOVERSION`: `3.1` for supported releases

These values must stay aligned across the strict and future stable packages.

## Existing repository collision audit

No current `upp_imaging` file collisions were found for these names:

- `imageio.h`
- `typedesc.h`
- `ustring.h`
- `filesystem.h`
- `strutil.h`
- `OpenImageIO`
- `OIIO`
- `openimageio`

Current collision risk is low for a future lowercase package namespace such as `openimageio/`.

Recommended include convention:

- stable route: `#include <openimageio/imageio.h>`
- strict route: keep the strict package’s internal headers private; do not expose raw upstream `OpenImageIO/...` paths to application code unless the package wrapper is intentionally mirroring them

## OpenColorIO relationship

In this release, OpenColorIO is a direct core-library dependency, not just a tool or plugin dependency.

Evidence:

- `src/libOpenImageIO/CMakeLists.txt` links `OpenColorIO::OpenColorIO` directly
- `src/libOpenImageIO/color_ocio.cpp` includes `OpenColorIO/OpenColorIO.h`
- `src/include/OpenImageIO/color.h` documents runtime OCIO-backed color configuration

Conclusion:

- the first strict core package should keep OCIO enabled
- the first utility-only milestone can omit it, but the no-I/O core milestone cannot

## OpenEXR and Imath relationship

Imath:

- required by `OpenImageIO_Util`
- required by `OpenImageIO`
- required by the OpenEXR plugin path

OpenEXR:

- required by `OpenImageIO`
- required by the OpenEXR plugin path
- not required by `OpenImageIO_Util`

Recommended future U++ dependencies:

- `openimageio_util_src` -> `imath`
- `openimageio_src` -> `openexr`, `imath`
- `openimageio_plugin_openexr` -> `openexr`, `imath`

## Proposed gated sequence

### Task 008A - strict OpenImageIO utility library

- objective: compile and link `OpenImageIO_Util`
- packages affected: `openimageio_util_src`
- dependencies: `imath`, `fmt`, platform threading provider
- validation target: compile/link only, then a tiny probe for `TypeDesc`, `Strutil`, `Filesystem`, `ustring`, and the strict marker
- explicitly deferred work: `OpenImageIO`, plugins, file I/O, tools, Python, viewers

### Task 008B - strict OpenImageIO core library

- objective: compile and link `OpenImageIO` without loading or writing an image file
- packages affected: `openimageio_src`
- dependencies: `openimageio_util_src`, `openexr`, `imath`, `zlib`, `libtiff`, `libjpeg_turbo`, `opencolorio`, `fmt`, `robinmap`
- validation target: probe for version, `TypeDesc`, `ImageSpec`, one string/utility API, and the strict marker
- explicitly deferred work: plugins, file I/O, tools, Python, viewers

### Task 009 - stable `openimageio` wrapper

- objective: add the application-facing wrapper once the strict core is stable
- packages affected: `openimageio`
- dependencies: `openimageio_src`
- validation target: include-route and API boundary check
- explicitly deferred work: format plugins and tools

### Task 010A - OpenEXR plugin

- objective: first format plugin boundary
- packages affected: `openimageio_plugin_openexr`
- dependencies: `openimageio_src`, `openexr`, `imath`
- validation target: plugin registration boundary only
- explicitly deferred work: PNG, JPEG, TIFF, and all other plugins

### Task 010B - PNG plugin

- objective: add PNG codec support as a separate package
- packages affected: `openimageio_plugin_png`
- dependencies: `openimageio_src`, `libpng`, `zlib`
- validation target: plugin registration boundary only
- explicitly deferred work: other codecs

### Task 010C - JPEG plugin

- objective: add JPEG codec support as a separate package
- packages affected: `openimageio_plugin_jpeg`
- dependencies: `openimageio_src`, `libjpeg_turbo`
- validation target: plugin registration boundary only
- explicitly deferred work: other codecs

### Task 010D - TIFF plugin

- objective: add TIFF codec support as a separate package
- packages affected: `openimageio_plugin_tiff`
- dependencies: `openimageio_src`, `libtiff`, `zlib`, `libjpeg_turbo`
- validation target: plugin registration boundary only
- explicitly deferred work: other codecs

### Task 011 - baseline multi-format validation

- objective: exercise the strict core plus the baseline plugin set
- packages affected: core + four baseline plugins
- dependencies: previous tasks complete
- validation target: no-I/O boundary plus format smoke tests
- explicitly deferred work: the long tail of optional codecs and tools

## Exact next task

Task 008A - strict OpenImageIO utility library.

- package name: `openimageio_util_src`
- upstream target: `OpenImageIO_Util`
- source count: 18 `.cpp`
- header/config requirements: utility public headers plus `version_win32.rc` on Windows; no `oiioversion.h` or `buildopts.h` beyond the util target’s normal include needs
- dependencies: `imath`, `fmt`, platform threading support
- compile definitions: `OpenImageIO_Util_EXPORTS`, static-defining macro if built static, Windows lean/minimal macros
- probe behavior: report utility version info, construct `TypeDesc`, exercise `Strutil` and `Filesystem`, verify the strict marker, no image file I/O
- validation targets: compile/link only, then a tiny audit-only probe
- out-of-scope: stable wrapper, format plugins, image file I/O, tools, Python, viewer integration, LumaPix integration

## Files changed

- `docs/openimageio_source_dependency_audit.md`
