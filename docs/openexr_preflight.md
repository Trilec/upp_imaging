# OpenEXR Preflight

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

## Minimal package strategy

Recommended package direction:

- `openexr_src`: strict imported-source package layer
- `openexr`: user-facing package layer later, only after the strict package is proven

For this task, only a preflight skeleton is added:

- `openexr_src/`: header/probe skeleton only
- `openexr_src_probe/`: compile-time probe only

Current probe limitation discovered:

- imported OpenEXR public headers such as `ImfRgba.h` include `<half.h>` directly
- under the current U++ package include-path behavior, that needs an additional packaging decision before a broader OpenEXR header set can be used cleanly
- the current probe therefore stays on the safer version/config/namespace surface instead of claiming broader header usability yet

## What should be attempted first next

- package `Iex`, `IlmThread`, and `OpenEXRCore` explicitly as strict source layers or carefully scoped internal subpackages
- decide whether to keep OpenEXR's current compression surface minimal for the first scanline RGBA round-trip
- add a minimal strict OpenEXR library build before any EXR file read/write test

## Deliberately not attempted yet

- no full OpenEXR library package build
- no EXR read/write round-trip
- no OpenEXRUtil package
- no Python bindings
- no tools/examples/test data import
- no claim of OpenEXR support yet
