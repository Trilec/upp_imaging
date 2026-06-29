# OpenEXR Generated Config

This document tracks generated/config headers needed for OpenEXR lower-layer packaging.

## OpenEXRConfig.h

- upstream template: `openexr-3.4.13/cmake/OpenEXRConfig.h.in`
- repository-owned generated output: `openexr_src/upstream/OpenEXRConfig.h`
- values used:
  - version: `3.4.13`
  - soversion: `33`
  - Imath version linkage: `30 / 3.2.2`
  - OpenJPH version markers: `0.26.3`
  - internal namespace: `Imf_3_4`
  - public namespace: `Imf`
  - visibility: static-package-friendly defaults
- why valid: values were derived from the inspected 3.4.13 release metadata and static-build-oriented defaults from upstream CMake
- note: packaging metadata, not unmodified upstream source

## IexConfig.h

- upstream template: `openexr-3.4.13/cmake/IexConfig.h.in`
- needed by: `IexNamespace.h`
- values needed:
  - internal namespace: expected `Iex_3_4`
  - public namespace: `Iex`
- status in this task: repository-owned generated header added for `iex_src`

## IexConfigInternal.h

- upstream template: `openexr-3.4.13/cmake/IexConfigInternal.h.in`
- repository-owned generated output: `iex_src/upstream/IexConfigInternal.h`
- values used:
  - `HAVE_UCONTEXT_H` unset
  - `IEX_HAVE_CONTROL_REGISTER_SUPPORT` unset
  - `IEX_HAVE_SIGCONTEXT_CONTROL_REGISTER_SUPPORT` unset
- why valid: current Windows/CLANGx64 preflight does not rely on the Unix signal/ucontext FPE path guarded by these macros
- note: packaging metadata, not unmodified upstream source

## IlmThreadConfig.h

- upstream template: `openexr-3.4.13/cmake/IlmThreadConfig.h.in`
- needed by: `IlmThreadNamespace.h`
- values needed:
  - `ILMTHREAD_THREADING_ENABLED`
  - `ILMTHREAD_HAVE_POSIX_SEMAPHORES`
  - `ILMTHREAD_USE_TBB`
  - internal namespace: expected `IlmThread_3_4`
  - public namespace: `IlmThread`
- status in this task: repository-owned generated header added for `ilmthread_src`
- local Windows expectation: threading enabled, no POSIX semaphores, no TBB

## Export Headers

- `ImfExport.h`, `IexExport.h`, and `IlmThreadExport.h` are upstream headers, not generated files
- they all depend on top-level config macros such as `OPENEXR_DLL` and visibility macros from `OpenEXRConfig.h`
- for current static-package builds, the non-DLL paths are the intended configuration

## Already Added In Current Preflight Work

- `OpenEXRConfig.h`
- `IexConfig.h`
- `IexConfigInternal.h`
- `IlmThreadConfig.h`

## Still Needed For OpenEXRCore / Full OpenEXR

- `OpenEXRConfigInternal.h`
- `openexr_config.h` integration details for `OpenEXRCore`
- compression-specific config headers or macro decisions for `OpenEXRCore`
- OpenJPH / deflate control macros or package decisions
- any additional platform threading/config glue beyond the current IlmThread layer
