# OpenJPH / HTJ2K Strategy

## Where OpenEXR 3.4.13 Pulls OpenJPH Into The Build

OpenJPH is pulled in by the OpenEXR top-level build setup, not by a tiny optional helper.

Observed build logic:

- `cmake/OpenEXRSetup.cmake`
  - searches for external `openjph >= 0.21.0`
  - otherwise falls back to vendored OpenJPH
  - records OpenJPH version macros into `OpenEXRConfig.h`
- `src/lib/OpenEXRCore/CMakeLists.txt`
  - links external OpenJPH if found, or
  - vendors OpenJPH object code directly into `OpenEXRCore`

This means OpenJPH is part of the OpenEXRCore build story, not just a sidecar tool dependency.

## Which Files Reference OpenJPH

Primary source-level OpenJPH coupling inside OpenEXRCore:

- `src/lib/OpenEXRCore/internal_ht.cpp`
  - includes OpenJPH headers directly:
    - `openjph/ojph_arch.h`
    - `openjph/ojph_file.h`
    - `openjph/ojph_params.h`
    - `openjph/ojph_mem.h`
    - `openjph/ojph_codestream.h`
    - `openjph/ojph_message.h`
  - uses OpenJPH API types and functions in the HTJ2K encode/decode path

Related OpenEXRCore files tied to HTJ2K behavior:

- `src/lib/OpenEXRCore/internal_ht_common.cpp`
- `src/lib/OpenEXRCore/internal_ht_common.h`
- `src/lib/OpenEXRCore/compression.c`
- `src/lib/OpenEXRCore/openexr_attr.h`
- `src/lib/OpenEXRCore/debug.c`

## Is OpenJPH Only Needed For HTJ2K?

From the audited source, yes: the direct OpenJPH API usage is specifically tied to the HTJ2K compression path.

Evidence:

- `compression.c` maps:
  - `EXR_COMPRESSION_HTJ2K32`
  - `EXR_COMPRESSION_HTJ2K256`
  to `internal_exr_apply_ht()` / `internal_exr_undo_ht()`
- those HT functions live in the `internal_ht.*` implementation path
- `internal_ht.cpp` is the file that directly includes and uses OpenJPH

## Can HTJ2K Be Disabled Cleanly For A First openexr_core_src Package?

Not cleanly enough to claim yet.

What is clear:

- The OpenEXRCore build always expects either external or vendored OpenJPH in the audited upstream logic.
- The inspected CMake path does not expose an obvious simple option that says “build OpenEXRCore, but skip HTJ2K entirely”.
- HTJ2K values are part of public compression enums in `openexr_attr.h`.
- `compression.c` dispatch includes HTJ2K cases in both encode and decode switching.

What is still uncertain:

- whether a careful local package build could exclude `internal_ht.cpp` and patch the dispatch/config path without creating an ABI or behavior lie
- whether upstream has another disable path outside the parts audited here

So the honest answer is: **uncertain, and not yet cleanly demonstrated**.

## Should The Next Package Be openjph_src?

Probably yes, if the goal is a faithful strict-source OpenEXRCore package.

Why:

- OpenJPH is the next unresolved dependency pressure point after libdeflate
- OpenEXR 3.4.13 already has a specific OpenJPH expectation in its build logic
- pretending HTJ2K is gone without proving the disable path would be shaky packaging

## Standalone OpenJPH vs OpenEXR-Vendored OpenJPH

Current recommendation: prefer **official standalone OpenJPH** if the version can be matched cleanly.

Reasons:

- it matches the general `upp_imaging` approach of independently useful packages
- it keeps OpenEXRCore dependency handling explicit
- it avoids burying another reusable library inside OpenEXR-internal packaging

However:

- OpenEXR vendors OpenJPH `0.26.3` in the inspected 3.4.13 tree
- if a standalone OpenJPH package is added, it should be checked against that version first to avoid a mismatch right out of the gate

## Is OpenJPH Independently Useful Here?

Maybe, but less obviously than zlib, libpng, Imath, or libdeflate.

It can still make sense as an explicit package because:

- it keeps the OpenEXRCore graph honest
- it avoids opaque vendored magic later

But it may end up being more OpenEXR-facing than broadly useful to typical U++ users.

## Recommendation

Recommendation: **Option B, leaning practical**

Add a standalone `openjph_src` package next, version-matched against what OpenEXR 3.4.13 vendors or expects.

Why:

- libdeflate is now in place, so OpenJPH is the next real blocker
- the current audit does not prove a clean HTJ2K-disable route yet
- packaging OpenJPH directly is a cleaner next move than pretending HTJ2K can be wished away

Likely package names:

- `openjph_src`
- `openjph`
- `openjph_src_test`
- `openjph_test`
