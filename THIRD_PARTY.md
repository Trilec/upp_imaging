# Third-Party Code

## libheif

- Upstream version: 1.23.5 (from 1.23.1)
- Official release: `https://github.com/strukturag/libheif/releases/tag/v1.23.5`
- Pinned submodule commit: `413e2a87e6a70b3eccc3a3adc5801179dd2d9e00`
- Release archive SHA-256, computed locally:
  `3BE49950C75D3FD9318BA775E8253248DD9E08C61EA955FCA81B6B89C02D6B2B`
- License: LGPL-3.0-or-later; upstream `COPYING` is retained in the submodule.
- Imported 2026-09-24. The upstream tag was verified with `git ls-remote`;
  only the submodule pin and repository-owned generated version header and
  package metadata changed. The decode-only dav1d/libde265 source manifest
  and registry-lifetime overlay are retained. No upstream source was edited.
- Security scope and applicable 1.23.4/1.23.5 fixes are in
  `docs/THIRD_PARTY_SECURITY.md`.

## Expat

- Upstream version: 2.8.5 (from 2.7.2)
- Official release archive:
  `https://github.com/libexpat/libexpat/releases/download/R_2_8_5/expat-2.8.5.tar.gz`
- Archive SHA-256:
  `920DDE485E15EDA0CCE8D2310B41D492C534E5E3D89AD407A0B4176DD2FF88FE`
- License: MIT/X Consortium; original `COPYING` is in
  `third_party/support/expat_src/upstream/COPYING`.
- Imported 2026-09-24. The upstream `lib/` sources and public headers match
  the release archive; the repository-generated `expat_config.h` is mirrored
  into `upstream/` and `upstream/lib/` for the Windows U++ static build.
- Linked provider: `expat_src` through `expat`, used by OCIO XML readers.
  DTD, namespaces, general entities and large-size support are enabled;
  the Windows `rand_s` entropy provider is compiled. Tools and examples
  are not linked.
- Security applicability and the remaining upstream issue are tracked in
  `docs/THIRD_PARTY_SECURITY.md`; this version is not certified
  vulnerability-free.

## zlib

- Upstream name: zlib
- Upstream version: 1.3.2
- Upstream authors: Jean-loup Gailly and Mark Adler
- Source archive used: `https://zlib.net/zlib-1.3.2.tar.gz`
- SHA-256: `BB329A0A2CD0274D05519D61C667C062E06990D72E125EE2DFA8DE64F0119D16`
- License: zlib License
- Date imported: 2026-06-28
- Files modified after import: No upstream files modified

Notes:

- The imported upstream source tree is preserved under `third_party/codecs/zlib_src/upstream/`.
- `zlib_src` is the strict package that compiles imported upstream zlib 1.3.2 sources directly.
- `zlib` is a compatibility package. On Windows/Core builds it can reuse U++ `plugin/z` linkage to avoid duplicate static zlib symbols, because `Core` already links `plugin/z` on Windows.

## libpng

- Upstream name: libpng
- Upstream version: 1.6.58
- Upstream authors: Guy Eric Schalnat, Andreas Dilger, Glenn Randers-Pehrson, Cosmin Truta, John Bowler, and other libpng contributors
- Source archive used: `https://download.sourceforge.net/libpng/libpng-1.6.58.tar.xz`
- SHA-256: `28EB403F51F0F7405249132CECFE82EA5C0EF97F1B32C5A65828814AE0D34775`
- License: PNG Reference Library License version 2
- Date imported: 2026-06-28
- Files modified after import: No upstream files modified

Notes:

- The imported upstream source tree is preserved under `third_party/codecs/libpng_src/upstream/`.
- `libpng_src` is the strict package that compiles imported upstream libpng 1.6.58 sources directly.
- `libpng_src` depends on `zlib_src`.
- The release-provided file `scripts/pnglibconf.h.prebuilt` was copied into `third_party/codecs/libpng_src/upstream/pnglibconf.h` to avoid local config generation.
- A local bridge header `third_party/codecs/libpng_src/upstream/zlib.h` was added so the imported libpng sources can include zlib from `zlib_src` without modifying upstream libpng files.
- `libpng` is the current user-facing package and compiles the same imported libpng sources against `zlib` for compatibility with normal U++ applications.

## Imath

- Upstream name: Imath
- Upstream version: 3.2.2
- Upstream authors: Contributors to the OpenEXR Project / Academy Software Foundation
- Source archive used: `https://github.com/AcademySoftwareFoundation/Imath/archive/refs/tags/v3.2.2.tar.gz`
- SHA-256: `B4275D83FB95521510E389B8D13AF10298ED5BED1C8E13EFD961D91B1105E462`
- License: BSD-3-Clause
- Date imported: 2026-06-28
- Files modified after import: No upstream files modified

Notes:

- The imported upstream source tree is preserved under `third_party/imath/imath_src/upstream/`.
- `imath_src` is the strict package that compiles imported upstream Imath 3.2.2 sources directly.
- `imath` is the current user-facing package and delegates to `imath_src`.
- A repository-owned generated config header `third_party/imath/imath_src/upstream/ImathConfig.h` was created from upstream `config/ImathConfig.h.in` using the upstream release defaults needed for this local static package build.
- OpenEXR is not included in this import.

## OpenEXR

- Upstream name: OpenEXR
- Upstream version inspected: 3.4.13
- Upstream authors: Contributors to the OpenEXR Project / Academy Software Foundation
- Source archive inspected: `https://github.com/AcademySoftwareFoundation/openexr/archive/refs/tags/v3.4.13.tar.gz`
- SHA-256: `1ED0CEE48AC8C77DA235C8CA8AB85D031D43CD790EDA36AF87FED4CF316CF2DF`
- License: BSD-3-Clause
- Date inspected: 2026-06-28
- Files modified after import: repository-owned wrapper/header and config files were added around the imported upstream source tree

Notes:

- This task does not add full OpenEXR support.
- `openexr_src` now carries the full high-level OpenEXR source package for 3.4.13.
- Repository-owned generated config and bridge headers remain in `third_party/openexr/openexr_src/upstream/` to preserve the strict source boundary.
- The package depends directly on `imath_src`, `iex_src`, `ilmthread_src`, and `openexr_core_src`.

## Iex

- Upstream name: Iex
- Upstream version inspected from OpenEXR release: 3.4.13 source tree
- Source origin: `openexr-3.4.13/src/lib/Iex`
- Files modified after import: Yes

Notes:

- `iex_src` packages the lower OpenEXR exception/error layer.
- Repository-owned generated headers `IexConfig.h` and `IexConfigInternal.h` were added from upstream templates / local static-package assumptions.
- Imported file `third_party/openexr/iex_src/upstream/IexMathFpu.cpp` was minimally adjusted to use quoted local config-header includes for the U++ package include model.

## IlmThread

- Upstream name: IlmThread
- Upstream version inspected from OpenEXR release: 3.4.13 source tree
- Source origin: `openexr-3.4.13/src/lib/IlmThread`
- Files modified after import: No imported upstream source files modified

Notes:

- `ilmthread_src` packages the lower OpenEXR threading layer.
- Repository-owned generated header `IlmThreadConfig.h` was added from upstream template / local Windows static-package assumptions.

## libdeflate

- Upstream name: libdeflate
- Upstream version: 1.25
- Upstream authors/project: Eric Biggers / libdeflate project
- Source archive used: `https://github.com/ebiggers/libdeflate/archive/refs/tags/v1.25.tar.gz`
- SHA-256: `D11473C1AD4C57D874695E8026865E38B47116BBCB872BFC622EC8F37A86017D`
- License: MIT
- Date imported: 2026-06-28
- Files modified after import: No upstream files modified

Notes:

- `libdeflate_src` packages the official standalone libdeflate release, not the OpenEXR-vendored subtree.
- The selected version matches the libdeflate version vendored by OpenEXR 3.4.13.
- `libdeflate` is the user-facing wrapper package and currently delegates to `libdeflate_src`.

## JPEG XL / libjxl

- Upstream name: libjxl
- Upstream version: 0.12.0
- Upstream commit: `a7a9c787341cf703dede03c2009fa460cae5e5df`
- Source origin: `https://github.com/libjxl/libjxl.git`
- License: BSD-3-Clause with upstream PATENTS grant
- Date pinned: 2026-08-12
- Files modified after import: upstream files are not modified; repository-owned U++ manifests and generated static headers live outside the pinned tree

Pinned build dependencies used by `jpegxl_src`:

- Brotli: `028fb5a23661f123017c060daa546b55cf4bde29` (MIT)
- Highway: `457c891775a7397bdb0376bb1031e6e027af1c48` (Apache-2.0/BSD-3-Clause)
- skcms: `96d9171c94b937a1b5f0293de7309ac16311b722` (BSD-3-Clause)

Notes:

- `third_party/codecs/jpegxl_src/upstream` is a pinned Git submodule and its required nested dependencies are initialized recursively from libjxl's own submodule declarations.
- `jpegxl_src` compiles the libjxl codec, thread runners, Brotli, core Highway runtime dispatch, and baseline skcms directly through U++ `import.ext`.
- JPEG XL container boxes are enabled.
- Lossless JPEG reconstruction/transcoding is disabled, so libjpeg-turbo is not part of this backend slice.
- lcms, googletest, sjpeg, libpng, zlib, testdata and command-line tools are not compiled by this package.
- Repository-owned generated `version.h` and static export headers replace CMake-generated headers for this U++ source build.

## OpenImageIO source family

- Upstream release: [v3.1.17.0](https://github.com/AcademySoftwareFoundation/OpenImageIO/releases/tag/v3.1.17.0)
- Exact plugin-source submodule: `73bc189f7d8469a9760ce9c5099b686c77695074`
- Enabled: copied main, utility and public-header slices with statically registered selected image plugins; `third_party/openimageio/` package READMEs identify the compiled files.
- Local integration: robinmap/Imath include adaptations and separate MinGW main-thread error-storage wrappers are retained. The upstream source license is Apache-2.0; see `LICENSES.md`.
- Security validation: 10 affected test targets in both Windows configurations, 306 checks and 20 clean exits. The clean full suite and Workbench remain pending.

## OpenImageIO JPEG XL plugin

- Upstream project: OpenImageIO
- Upstream version: 3.1.17.0
- Source origin: `src/jpegxl.imageio/jxlinput.cpp` and `src/jpegxl.imageio/jxloutput.cpp`
- Upstream source license: Apache-2.0
- Date integrated: 2026-08-12; source refreshed: 2026-09-24
- Backend: repository-pinned libjxl 0.12.0

Notes:

- `openimageio_plugin_jpegxl` statically registers the OpenImageIO JPEG XL reader/writer through the stable `OpenImageIO` package.
- The reader is retained from the 3.1.17.0 source slice; the repository copy of the writer is packaged locally for the U++ static plugin model.
- ImagingIO applies a stricter framework policy than the raw plugin and currently rejects JPEG XL GrayAlpha and arbitrary extra-channel/MultiChannel layouts.

## Local lifetime integration

Pinned source trees are unchanged by the layout migration. OpenColorIO's source manifest compiles a reproducible local FileTransform.cpp overlay that owns its LUT format registry and raw format objects at process shutdown; its generator and original BSD-3-Clause copyright are retained. OpenImageIO source wrappers expose shutdown of MinGW main-thread error storage before the U++ heap audit, retaining pending-error reporting. These downstream integration files live outside upstream trees. See docs/STRUCTURE_MIGRATION.md and the respective package READMEs.
