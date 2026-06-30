# OpenEXRCore Plan

## What OpenEXRCore Is

`OpenEXRCore` is the low-level C/C++ core layer beneath the higher-level OpenEXR C++ image API.

It is not a tiny helper library. It is the first layer where compression, context management, chunk I/O, and several internal implementation paths all arrive at once.

## Source Files Needed For A Minimal Build

Even a reduced build still points at a substantial source set from `src/lib/OpenEXRCore`, including:

- base/context/memory/string/attribute/channel list primitives
- header parsing and writing
- chunk/coding/compression/encode/decode paths
- part/standard attribute support
- low-level pack/unpack and validation

From upstream `CMakeLists.txt`, a real package attempt would need many `.c` and `.cpp` files, not just a couple of public entry points.

## Public Headers Needed For A First Probe

Likely public header surface:

- `openexr.h`
- `openexr_base.h`
- `openexr_errors.h`
- `openexr_context.h`
- `openexr_compression.h`
- `openexr_part.h`
- `openexr_version.h`

These already depend on generated/config glue such as `openexr_config.h` and `OpenEXRConfig.h`.

## Generated / Config Headers Needed

At minimum:

- `OpenEXRConfig.h`
- `OpenEXRConfigInternal.h`
- `openexr_config.h`
- `IlmThreadConfig.h`
- `ImathConfig.h`

Likely also requires the surrounding macro decisions from upstream CMake for:

- symbol visibility
- embedded vs separate core functions
- threading
- compression backend selection

## Compression Dependency Findings

`OpenEXRCore` is the first layer where compression is unavoidable in practice.

Observed from upstream:

- ZIP/ZIPS paths use `libdeflate`
- RLE, PIZ, PXR24, DWA, and HTJ2K paths are part of the same core library build
- compression dispatch is centralized enough that a naive “just scanline RGBA” subset is not obviously tiny

## Deflate / zlib Findings

- OpenEXR 3.4.13 uses `libdeflate`, not classic `zlib`, for its ZIP-style compression backend in current upstream packaging
- if `libdeflate` is not found externally, upstream uses vendored deflate sources
- this means our existing `zlib_src` package does not satisfy OpenEXRCore's deflate dependency story
- a future OpenEXRCore package must either:
  - package vendored `libdeflate` behavior as part of the OpenEXRCore layer, or
  - add a separate `libdeflate_src` package first

## libdeflate package result

- selected source strategy: standalone official libdeflate release
- selected version: `1.25`
- source archive: `https://github.com/ebiggers/libdeflate/archive/refs/tags/v1.25.tar.gz`
- SHA-256: `D11473C1AD4C57D874695E8026865E38B47116BBCB872BFC622EC8F37A86017D`
- expected role: independent compression package and likely future dependency of `openexr_core_src`

Why this strategy:

- it keeps `upp_imaging` focused on independently usable packages
- it matches the libdeflate version vendored by OpenEXR 3.4.13
- it removes one major ambiguity from future `OpenEXRCore` packaging

Expected OpenEXRCore impact:

- this should satisfy the ZIP/ZIPS deflate backend story better than trying to route through our existing zlib packages
- it does not solve the OpenJPH/HTJ2K side of the boundary

## OpenJPH Findings

- OpenEXR 3.4.13 checks for external `openjph >= 0.21.0`
- if not found, upstream vendors OpenJPH into `OpenEXRCore`
- upstream release tree inspected here contains vendored OpenJPH `0.26.3`
- HTJ2K support is therefore part of the current OpenEXRCore packaging surface unless explicitly disabled by a packaging strategy we have not yet reproduced in U++

## Can OpenJPH Be Disabled Cleanly?

Not clearly from the current preflight.

What is clear:

- upstream `OpenEXRCore/CMakeLists.txt` always resolves either external or vendored OpenJPH
- the inspected build logic does not expose a simple obvious “OFF” switch in the part reviewed here that would let us skip HTJ2K code cleanly for a first U++ package attempt

So for now, OpenJPH should be treated as part of the OpenEXRCore packaging problem, not an optional footnote.

## OpenJPH / HTJ2K Decision After libdeflate

- OpenJPH is still the next blocker after libdeflate
- from the inspected OpenEXR 3.4.13 build logic, there is no obvious small, clean U++-level switch already mapped here that disables HTJ2K/OpenJPH while leaving the rest of OpenEXRCore neatly packaged
- upstream currently resolves either external OpenJPH or vendored OpenJPH for OpenEXRCore
- therefore the next task should either:
  - package OpenJPH intentionally, or
  - do one more OpenEXRCore header/config/compression probe focused specifically on proving a disable strategy before attempting a full `openexr_core_src`

Current recommendation: OpenJPH remains the next blocker.

## Can A Minimal Scanline RGBA Only Build Be Realistic?

Maybe later, but not honestly enough to claim from this preflight.

Reasons:

- the higher-level scanline RGBA API sits above `OpenEXRCore`
- `OpenEXRCore` itself still carries broad compression and chunk machinery
- trying to subset too aggressively now risks producing a misleading package that compiles but is not faithful to the real upstream dependency surface

## Safer Packaging Direction

For the first real `OpenEXRCore` attempt, a fuller strict-source package is safer than pretending the core can be reduced to a couple of files.

That package should still be selective about docs/tests/tools, but not overly clever about core implementation subsetting.

## Recommendation

Recommendation: **Option C**

Do not package `openexr_core_src` yet; first add a deflate/OpenJPH packaging or disable strategy.

Why:

- `Iex` and `IlmThread` are now clean and test-backed
- `OpenEXRCore` is the first place where the dependency graph stops being lightweight
- `libdeflate` is a distinct dependency from our existing zlib stack, but it is now straightforward to package independently
- OpenJPH is part of the current upstream core story and should be handled intentionally, not accidentally
- the next clean engineering step is to decide whether to:
  - package OpenJPH deliberately, or
  - establish a documented way to disable the corresponding paths for a first strict U++ build

Until that choice is made, full EXR read/write remains intentionally deferred.
