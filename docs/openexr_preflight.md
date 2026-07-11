# OpenEXR Preflight

## Current status

- the preflight investigation has been converted into the working strict `openexr_src` package
- `openexr_src_probe` now builds and runs successfully

## Version inspected

- OpenEXR `3.4.13`

## Source archive

- archive: `https://github.com/AcademySoftwareFoundation/openexr/archive/refs/tags/v3.4.13.tar.gz`
- SHA-256: `1ED0CEE48AC8C77DA235C8CA8AB85D031D43CD790EDA36AF87FED4CF316CF2DF`
- license: BSD-3-Clause

## Imath compatibility

- upstream OpenEXR `3.4.13` looks for `Imath >= 3.1`
- upstream OpenEXR's own fallback tag is `Imath v3.2.2`
- current local `imath_src` `3.2.2` is compatible for first-pass packaging

## Source tree areas inspected

- `src/lib/Iex`
- `src/lib/IlmThread`
- `src/lib/OpenEXR`
- `src/lib/OpenEXRCore`
- `src/lib/OpenEXRUtil`
- `cmake/OpenEXRSetup.cmake`
- `cmake/OpenEXRConfig.h.in`

## Immediate dependency findings

- OpenEXR public C++ library depends on:
  - `Imath`
  - `Iex`
  - `IlmThread`
  - `OpenEXRCore`
- `OpenEXRCore` brings additional compression/config concerns
- `zlib` is not the only compression dependency story anymore in current OpenEXR
- current tree also references vendored/internal `OpenJPH` for HTJ2K support paths

## Generated config/header findings

Likely generated or config-managed files for a real package attempt include:

- `OpenEXRConfig.h`
- `IexConfig.h`
- `IlmThreadConfig.h`
- possibly config-managed namespace/export/version wrappers per library

This preflight package includes a repository-owned `OpenEXRConfig.h` derived from upstream `cmake/OpenEXRConfig.h.in`, but does not yet attempt the full generated-config set required for a real library build.

Lower-layer progress in this task:

- `iex_src` uses a repository-owned `IexConfig.h` derived from upstream `cmake/IexConfig.h.in`
- `iex_src_test` now passes under local `CLANGx64`
- `ilmthread_src` uses a repository-owned `IlmThreadConfig.h` derived from upstream `cmake/IlmThreadConfig.h.in`
- `ilmthread_src_test` now passes under local `CLANGx64`
- `openexr_core_src` is still deferred

## Minimal package strategy

Recommended package direction:

- `openexr_src`: strict imported-source package layer
- `openexr`: user-facing package layer later, only after the strict package is proven

What was added for the strict package:

- `openexr_src/`: full imported high-level source package with generated config and include bridges
- `openexr_src_probe/`: strict boundary probe

Historical probe limitation that was resolved:

- imported OpenEXR public headers such as `ImfRgba.h` include `<half.h>` directly
- the package now resolves that through the local source-boundary layout and imported bridge headers
- the strict source package now exercises the broader high-level API surface successfully

## What should be attempted first next

- add a user-facing `openexr` wrapper package only if the stable boundary is needed
- extend validation into an actual EXR file read/write round-trip if that becomes the next milestone
- keep the generated-config and include-bridge policy aligned with upstream 3.4.13

## Deliberately not attempted yet

- no full OpenEXR library package build
- no user-facing `openexr` wrapper package
- no EXR read/write round-trip
- no OpenEXRUtil package
- no Python bindings
- no tools/examples/test data import
- no claim of a stable wrapper package yet
