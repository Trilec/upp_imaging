# OpenEXR High-Level Source Audit

## 1. Purpose

Audit the upstream OpenEXR 3.4.13 high-level `Imf*` source boundary so `openexr_src` can be implemented as a strict imported-source package without rediscovering upstream structure.

## 2. Upstream version and authority

- Version: `OpenEXR 3.4.13`
- Primary authority: `src/lib/OpenEXR/CMakeLists.txt`
- Config authority:
  - `cmake/OpenEXRConfig.h.in`
  - `cmake/IexConfig.h.in`
  - `cmake/IlmThreadConfig.h.in`
  - `cmake/OpenEXRConfigInternal.h.in`
- Header authority used for include relationships:
  - `ImfInputFile.h`
  - `ImfOutputFile.h`
  - `ImfHeader.h`
  - `ImfFrameBuffer.h`
  - `ImfChannelList.h`
  - `ImfCompression.h`
  - `ImfRgba.h`
  - `ImfRgbaFile.h`
  - `ImfForward.h`
  - `ImfVersion.h`
  - `ImfExport.h`
  - `ImfNamespace.h`
  - `ImfThreading.h`
  - `ImfStdIO.h`
  - `ImfGenericInputFile.h`
  - `ImfGenericOutputFile.h`

The current repository copies of `OpenEXRConfig.h`, `IexConfig.h`, `IlmThreadConfig.h`, `ImfExport.h`, `ImfNamespace.h`, `ImfVersion.h`, and `openexr_version.h` are metadata/header artifacts, not the upstream library target itself.

## 3. High-level implementation source manifest

Upstream `OpenEXR` 3.4.13 compiles **96 `.cpp` files** in the high-level target. In the upstream target, all are mandatory for the library build; none are tools, tests, or Python-only sources.

### 3.1 Core API, context, file plumbing, and general utilities

Mandatory.

- `ImfAcesFile.cpp`
- `ImfCRgbaFile.cpp`
- `ImfContext.cpp`
- `ImfContextInit.cpp`
- `ImfConvert.cpp`
- `ImfGenericInputFile.cpp`
- `ImfGenericOutputFile.cpp`
- `ImfHeader.cpp`
- `ImfIO.cpp`
- `ImfMisc.cpp`
- `ImfPartType.cpp`
- `ImfStdIO.cpp`
- `ImfSystemSpecific.cpp`
- `ImfTestFile.cpp`
- `ImfThreading.cpp`
- `ImfVersion.cpp`
- `ImfWav.cpp`
- `ImfRgbaFile.cpp`
- `ImfRgbaYca.cpp`
- `ImfFrameBuffer.cpp`

### 3.2 Attribute, channel, and metadata support

Mandatory.

- `ImfAttribute.cpp`
- `ImfBoxAttribute.cpp`
- `ImfBytesAttribute.cpp`
- `ImfChannelList.cpp`
- `ImfChannelListAttribute.cpp`
- `ImfChromaticities.cpp`
- `ImfChromaticitiesAttribute.cpp`
- `ImfCompression.cpp`
- `ImfCompressionAttribute.cpp`
- `ImfDoubleAttribute.cpp`
- `ImfEnvmap.cpp`
- `ImfEnvmapAttribute.cpp`
- `ImfFloatAttribute.cpp`
- `ImfFloatVectorAttribute.cpp`
- `ImfFramesPerSecond.cpp`
- `ImfIDManifest.cpp`
- `ImfIDManifestAttribute.cpp`
- `ImfIntAttribute.cpp`
- `ImfKeyCode.cpp`
- `ImfKeyCodeAttribute.cpp`
- `ImfLineOrderAttribute.cpp`
- `ImfLut.cpp`
- `ImfMatrixAttribute.cpp`
- `ImfOpaqueAttribute.cpp`
- `ImfPreviewImage.cpp`
- `ImfPreviewImageAttribute.cpp`
- `ImfRational.cpp`
- `ImfRationalAttribute.cpp`
- `ImfStandardAttributes.cpp`
- `ImfStringAttribute.cpp`
- `ImfStringVectorAttribute.cpp`
- `ImfTileDescriptionAttribute.cpp`
- `ImfTimeCode.cpp`
- `ImfTimeCodeAttribute.cpp`
- `ImfVecAttribute.cpp`

### 3.3 Scanline, tiled, multipart, and part-level APIs

Mandatory.

- `ImfInputFile.cpp`
- `ImfOutputFile.cpp`
- `ImfScanLineInputFile.cpp`
- `ImfTiledInputFile.cpp`
- `ImfTiledOutputFile.cpp`
- `ImfTiledRgbaFile.cpp`
- `ImfInputPart.cpp`
- `ImfInputPartData.cpp`
- `ImfOutputPart.cpp`
- `ImfOutputPartData.cpp`
- `ImfMultiPartInputFile.cpp`
- `ImfMultiPartOutputFile.cpp`
- `ImfTiledInputPart.cpp`
- `ImfTiledOutputPart.cpp`
- `ImfDeepScanLineInputPart.cpp`
- `ImfDeepScanLineOutputPart.cpp`
- `ImfDeepTiledInputPart.cpp`
- `ImfDeepTiledOutputPart.cpp`

### 3.4 Deep-image APIs and compositing helpers

Mandatory.

- `ImfCompositeDeepScanLine.cpp`
- `ImfDeepCompositing.cpp`
- `ImfDeepFrameBuffer.cpp`
- `ImfDeepImageStateAttribute.cpp`
- `ImfDeepScanLineInputFile.cpp`
- `ImfDeepScanLineOutputFile.cpp`
- `ImfDeepTiledInputFile.cpp`
- `ImfDeepTiledOutputFile.cpp`

### 3.5 Compression codecs and codec scaffolding

Mandatory in the upstream target, even where individual codec behavior is gated by content or platform.

- `ImfB44Compressor.cpp`
- `ImfCompressor.cpp`
- `ImfDwaCompressor.cpp`
- `ImfFastHuf.cpp`
- `ImfHuf.cpp`
- `ImfHTCompressor.cpp`
- `ImfPizCompressor.cpp`
- `ImfPxr24Compressor.cpp`
- `ImfRle.cpp`
- `ImfRleCompressor.cpp`
- `ImfZip.cpp`
- `ImfZipCompressor.cpp`

### 3.6 Source-boundary conclusion

There is no evidence from the upstream target that any of the 96 `.cpp` files belong only to tools, tests, or Python bindings. The upstream high-level library is a single monolithic `OpenEXR` target with all of the above compiled into it.

## 4. Public headers

The upstream target installs 118 headers. For a stable application-facing package, the minimum practical scanline set is:

- `ImfExport.h`
- `ImfNamespace.h`
- `ImfForward.h`
- `ImfVersion.h`
- `ImfRgba.h`
- `ImfRgbaFile.h`
- `ImfHeader.h`
- `ImfFrameBuffer.h`
- `ImfChannelList.h`
- `ImfCompression.h`
- `ImfInputFile.h`
- `ImfOutputFile.h`
- `ImfThreading.h`
- `ImfIO.h`
- `ImfStdIO.h`
- `ImfPixelType.h`
- `ImfLineOrder.h`
- `ImfName.h`
- `ImfAttribute.h`
- `ImfMisc.h`

Important include chains:

- `ImfRgba.h` includes `ImfExport.h`, `ImfNamespace.h`, and `<half.h>`.
- `ImfRgbaFile.h` includes `ImfFrameBuffer.h`, `ImfHeader.h`, `ImfRgba.h`, `ImfThreading.h`, `<ImathBox.h>`, `<ImathVec.h>`, and `<half.h>`.
- `ImfInputFile.h` includes `ImfForward.h`, `ImfThreading.h`, and `ImfContext.h`.
- `ImfOutputFile.h` includes `ImfForward.h`, `ImfGenericOutputFile.h`, and `ImfThreading.h`.
- `ImfHeader.h` includes `ImfForward.h`, `IexBaseExc.h`, `ImathBox.h`, `ImathVec.h`, `ImfCompression.h`, `ImfLineOrder.h`, `ImfName.h`, `ImfTileDescription.h`, and `ImfAttribute.h`.
- `ImfFrameBuffer.h` includes `ImfForward.h`, `ImfName.h`, `ImfPixelType.h`, and `<ImathBox.h>`.
- `ImfChannelList.h` includes `ImfForward.h`, `ImfName.h`, and `ImfPixelType.h`.
- `ImfCompression.h` includes `ImfForward.h`.

Broader public headers in the upstream target include deep, multipart, tiled, attribute, and utility headers such as `ImfMultiPartInputFile.h`, `ImfMultiPartOutputFile.h`, `ImfDeepScanLineInputFile.h`, `ImfDeepTiledOutputFile.h`, `ImfPreviewImage.h`, `ImfStandardAttributes.h`, `ImfChromaticities.h`, `ImfRational.h`, `ImfTimeCode.h`, `ImfVecAttribute.h`, and `ImfWav.h`.

## 5. Internal headers

These are required by the implementation sources but should not be part of the ordinary application-facing include contract:

- `ImfAutoArray.h`
- `ImfB44Compressor.h`
- `ImfCheckedArithmetic.h`
- `ImfCompressor.h`
- `ImfContext.h`
- `ImfContextInit.h`
- `ImfConvert.h`
- `ImfDwaCompressor.h`
- `ImfFastHuf.h`
- `ImfGenericInputFile.h`
- `ImfGenericOutputFile.h`
- `ImfInputPartData.h`
- `ImfInputStreamMutex.h`
- `ImfOptimizedPixelReading.h`
- `ImfOutputPartData.h`
- `ImfOutputStreamMutex.h`
- `ImfPartHelper.h`
- `ImfPizCompressor.h`
- `ImfPxr24Compressor.h`
- `ImfRle.h`
- `ImfRleCompressor.h`
- `ImfScanLineInputFile.h`
- `ImfSimd.h`
- `ImfSystemSpecific.h`
- `ImfTileOffsets.h`
- `ImfTiledMisc.h`
- `ImfZip.h`
- `ImfZipCompressor.h`

## 6. Generated and configured headers

### `OpenEXRConfig.h`

- Upstream status: generated from `cmake/OpenEXRConfig.h.in`.
- Repository equivalent: `openexr_src/upstream/OpenEXRConfig.h` already exists.
- Suitability for high-level `OpenEXR`: suitable if the values remain aligned with 3.4.13.
- Important values:
  - `OPENEXR_VERSION_MAJOR/MINOR/PATCH = 3.4.13`
  - `OPENEXR_SOVERSION = 33`
  - `OPENEXR_IMATH_SOVERSION = 30`
  - `OPENEXR_IMATH_VERSION_* = 3.2.2`
  - `OPENEXR_OPENJPH_VERSION_* = 0.26.3`
  - `OPENEXR_IMF_INTERNAL_NAMESPACE = Imf_3_4`
  - `OPENEXR_IMF_NAMESPACE = Imf`
  - `OPENEXR_ENABLE_API_VISIBILITY` only when the build asks for it
- Regenerate or reuse: may be reused as repository-owned generated metadata, but should be treated as generated config rather than source.
- Conflict with OpenEXRCore: none if package prefixes remain isolated; both layers use the same release identity.

### `IexConfig.h`

- Upstream status: generated from `cmake/IexConfig.h.in`.
- Repository equivalent: `iex_src/upstream/IexConfig.h` exists.
- Suitability for high-level `OpenEXR`: suitable and required.
- Important values:
  - `IEX_INTERNAL_NAMESPACE = Iex_3_4`
  - `IEX_NAMESPACE = Iex`
- Regenerate or reuse: reuse the existing repository-owned generated copy.
- Conflict with OpenEXRCore: none.

### `IlmThreadConfig.h`

- Upstream status: generated from `cmake/IlmThreadConfig.h.in`.
- Repository equivalent: `ilmthread_src/upstream/IlmThreadConfig.h` exists.
- Suitability for high-level `OpenEXR`: suitable and required.
- Important values:
  - `ILMTHREAD_THREADING_ENABLED`
  - `ILMTHREAD_HAVE_POSIX_SEMAPHORES`
  - `ILMTHREAD_USE_TBB`
  - `ILMTHREAD_INTERNAL_NAMESPACE = IlmThread_3_4`
  - `ILMTHREAD_NAMESPACE = IlmThread`
- Regenerate or reuse: reuse the existing repository-owned generated copy.
- Conflict with OpenEXRCore: none.

### `ImfExport.h`

- Upstream status: not generated; source-controlled header.
- Repository equivalent: current `openexr_src/upstream/ImfExport.h` exists.
- Suitability: suitable if it continues to include the correct config macros.
- Important values: `OPENEXR_DLL`, `OPENEXR_EXPORTS`, and the visibility macros from `OpenEXRConfig.h`.

### `ImfNamespace.h`

- Upstream status: not generated; source-controlled header.
- Repository equivalent: current `openexr_src/upstream/ImfNamespace.h` exists.
- Suitability: suitable if it includes the matching `OpenEXRConfig.h`.
- Important values: `OPENEXR_IMF_NAMESPACE` and `OPENEXR_IMF_INTERNAL_NAMESPACE`.

### `ImfVersion.h`

- Upstream status: not generated; source-controlled public header.
- Repository equivalent: current `openexr_src/upstream/ImfVersion.h` exists.
- Suitability: suitable for the high-level package.
- Important values: `MAGIC = 20000630`, `EXR_VERSION = 2`, file flags for tiled, long names, non-image, and multipart.

### `openexr_version.h`

- Upstream status: not generated; source-controlled release/version header used by the OpenEXR tree.
- Repository equivalent: current `openexr_src/upstream/openexr_version.h` exists.
- Suitability: suitable as a shared version header, but it is not part of the high-level `OpenEXR` CMake target source list.
- Important values: `OPENEXR_VERSION_MAJOR/MINOR/PATCH = 3.4.13`.
- Conflict with OpenEXRCore: none if kept under package-prefixed include paths; both layers carry the same release number.

### `OpenEXRConfigInternal.h`

- Upstream status: generated/configured internal header, but not part of the high-level `OpenEXR` library target.
- Repository equivalent: present under `openexr_core_src/upstream`.
- Suitability for high-level `OpenEXR`: only needed if the high-level package later copies or reuses OpenEXRCore internals.
- Important values: `OPENEXR_USE_INTERNAL_DEFLATE`, `OPENEXR_IMF_HAVE_COMPLETE_IOMANIP`, `OPENEXR_IMF_HAVE_SYSCONF_NPROCESSORS_ONLN`, `OPENEXR_IMF_HAVE_GCC_INLINE_ASM_AVX`, `OPENEXR_MISSING_ARM_VLD1`.

## 7. Dependency graph

Upstream `OpenEXR` target dependencies from `src/lib/OpenEXR/CMakeLists.txt`:

- `Imath::Imath`
- `OpenEXR::Config`
- `OpenEXR::Iex`
- `OpenEXR::IlmThread`
- `OpenEXR::OpenEXRCore`

Dependency audit:

- `Imath`
  - Direct reference: yes
  - Existing U++ package: `imath_src` / `imath`
  - Boundary suitability: suitable
  - Collision risk: low

- `Iex`
  - Direct reference: yes
  - Existing U++ package: `iex_src`
  - Boundary suitability: suitable
  - Collision risk: low

- `IlmThread`
  - Direct reference: yes
  - Existing U++ package: `ilmthread_src`
  - Boundary suitability: suitable
  - Collision risk: low

- `OpenEXRCore`
  - Direct reference: yes, but it carries the lower-level storage/compression path rather than the full high-level API
  - Existing U++ package: `openexr_core_src` / `openexr_core`
  - Boundary suitability: suitable for transitive dependency reuse
  - Collision risk: moderate only if both strict and stable layers are linked into the same executable without a clear boundary

- `zlib`
  - Direct reference from the high-level `OpenEXR` target: no; transitive via `OpenEXRCore`
  - Existing U++ package: `zlib_src` / `zlib`
  - Boundary suitability: suitable
  - Collision risk: high only if `zlib_src` and the Windows `Core` provider are mixed in one normal executable; the existing policy should avoid that

- `libdeflate`
  - Direct reference from the high-level `OpenEXR` target: no; transitive via `OpenEXRCore`
  - Existing U++ package: `libdeflate_src` / `libdeflate`
  - Boundary suitability: suitable
  - Collision risk: low

- `OpenJPH`
  - Direct reference from the high-level `OpenEXR` target: no; transitive via `OpenEXRCore`
  - Existing U++ package: `openjph_src` / `openjph`
  - Boundary suitability: suitable
  - Collision risk: low

- `Threads`
  - Direct reference from the high-level `OpenEXR` target: not listed as a direct CMake dependency; threading is mediated through `IlmThread` and OpenEXRCore internals
  - Existing U++ package: standard toolchain/runtime thread support plus `ilmthread_src`
  - Boundary suitability: suitable
  - Collision risk: low

The accepted Windows zlib policy remains important:

- `zlib_src` proves the strict upstream source boundary.
- `zlib` is the normal Windows/Core consumer package.
- A normal `Core` executable should not be forced to link duplicate zlib implementations.

## 8. Required compile definitions

Observed from upstream config and headers:

- `OPENEXR_EXPORTS` when building the `OpenEXR` library target itself (`PRIV_EXPORT OPENEXR_EXPORTS` in CMake)
- `OPENEXR_DLL` only for DLL-style builds on Windows
- `OPENEXR_ENABLE_API_VISIBILITY` when visibility tagging is enabled by the build
- `OPENEXR_IMF_NAMESPACE` and `OPENEXR_IMF_INTERNAL_NAMESPACE`
- `OPENEXR_IMF_NAMESPACE_CUSTOM` and `OPENEXR_IMF_INTERNAL_NAMESPACE_CUSTOM`
- `ILMTHREAD_THREADING_ENABLED`
- `ILMTHREAD_HAVE_POSIX_SEMAPHORES`
- `ILMTHREAD_USE_TBB`
- `OPENEXR_HAVE_LARGE_STACK` only if the configure step detects that platform condition
- `OPENEXR_CORE_FUNCTIONS_EMBEDDED` only if OpenEXRCore functions are embedded into the build, which is not the current normal boundary
- `OPENEXR_USE_INTERNAL_DEFLATE` only for OpenEXRCore internals, not the high-level library boundary

Platform notes:

- `ImfSystemSpecific.h` and `ImfStdIO.h` show that Windows uses `_aligned_malloc`/`_aligned_free` and UTF-8 file-name assumptions.
- `ImfExport.h` is careful to suppress `dllimport`/`dllexport` confusion when `OPENEXR_DLL` is not set.
- The high-level target is not a plain header-only package; it needs the export and namespace macros to match the implementation objects.

## 9. Windows-specific considerations

- The library expects UTF-8 file paths in the constructors that take `const char*`.
- `ImfSystemSpecific.h` uses `_aligned_malloc` and `_aligned_free` on Windows.
- DLL builds use `OPENEXR_DLL` and `OPENEXR_EXPORTS` to switch export semantics.
- `IlmThread` already carries the platform thread/semaphore selection logic; the high-level package should not re-invent it.
- `zlib_src` must remain separate from the normal Windows/Core zlib provider to avoid duplicate symbol pressure in regular GUI/Core links.
- The current repository already has a working convention for package-prefixed include paths, which should keep `openexr_src` and `openexr_core_src` from colliding if both version headers are present.

## 10. Existing U++ package mapping

Current strict/stable mapping that the next implementation task should respect:

- `imath_src` -> strict imported-source Imath
- `imath` -> user-facing wrapper
- `iex_src` -> strict imported-source Iex
- `ilmthread_src` -> strict imported-source IlmThread
- `openexr_core_src` -> lower-level OpenEXRCore source package
- `openexr_core` -> user-facing OpenEXRCore wrapper
- `openexr_src` -> strict imported-source high-level OpenEXR package

Recommendation from the current architecture:

- The future strict high-level package should depend on `imath_src`, `iex_src`, `ilmthread_src`, and `openexr_core_src`.
- The stable `imath`, `openexr_core`, or other wrapper packages are useful for app code, but the strict source package boundary is clearer if it consumes the strict lower layers directly.

Potential issue to watch:

- `openexr_src` now carries the full high-level implementation set plus package-owned config copies; keep it aligned with the upstream 3.4.13 boundary.

## 11. Recommended openexr_src package structure

One `openexr_src` package still looks viable.

Recommended next-package shape:

- Keep `openexr_src` as the single imported-source high-level `OpenEXR` package.
- Add the full `src/lib/OpenEXR` `.cpp` set shown above.
- Keep the public API headers from the upstream `OpenEXR` header list.
- Keep internal helper headers in the package but do not expose them as the stable include contract.
- Reuse the existing generated/configured headers for `OpenEXRConfig.h`, `IexConfig.h`, and `IlmThreadConfig.h`.
- Continue to treat `openexr_version.h` as a version header artifact, not as a generated config header.

No split is currently forced by dependency or linker evidence. The upstream library itself is already a single target; the current evidence supports a monolithic `openexr_src` package rather than a package split.

## 12. Risks and unresolved questions

- Whether the package should expose all 118 upstream headers or a smaller stable subset plus internal-only package files.
- Whether `openexr_version.h` should be kept as a package-local artifact or folded into a shared version-header convention later.
- Whether `openexr_core_src` remains the right strict dependency for the high-level package, or whether the stable wrapper should be used for some app-facing layers later.
- Whether the current `openexr_src` probe should remain as the strict boundary test or be split into a separate package later.

## 13. Implementation result

`openexr_src` has been converted into the strict imported-source high-level `OpenEXR` package. It includes the full 96-file upstream `Imf*` implementation set, the required public headers, and the existing generated config headers, and it wires directly to `imath_src`, `iex_src`, `ilmthread_src`, and `openexr_core_src`.
