# ffmpeg_headers_test

Compile-contract and generated-configuration parity test for the pinned FFmpeg 9.0.1 public-header boundary.

The test links no FFmpeg implementation objects. It verifies that the repository-owned public `avconfig.h`, the four initial media-library header families, their expected release majors, and the core public context types can coexist in one U++ CLANGx64 translation unit.

Its eighth gate also reads the four production `import.ext` manifests, recursively follows reachable pinned FFmpeg/generated includes, collects referenced `CONFIG_*`, `HAVE_*`, `ARCH_*` and `AV_HAVE_*` identifiers, and fails with the complete missing-definition list when the repository-owned generated configuration is incomplete. The audit checks definition completeness; it does not decide whether a capability should be enabled.

Expected result: `SUMMARY passed=8 failed=0`.
