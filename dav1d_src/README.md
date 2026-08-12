# dav1d_src

Pinned scalar dav1d AV1 decoder backend for the HEIF/AVIF import stack.

- upstream release: 1.5.4 (`Sonic`)
- exact upstream/mirror commit: `54706fc6bc0cdecab7e9593974a4039cc038fca7`
- canonical upstream: VideoLAN dav1d
- repository submodule uses the read-only `libsdl-org/dav1d` GitHub mirror of the canonical source so the dependency remains reproducible through the repository tooling
- license: BSD-2-Clause

The package compiles dav1d's ordinary scalar library sources plus the Windows thread shim. The 13 upstream bit-depth template sources are instantiated separately for 8-bit and 16-bit through repository-owned wrappers. Assembly/SIMD is deliberately disabled in the first U++ slice; both AV1 bit-depth paths remain enabled.

Repository-owned `config.h` and `vcs_version.h` replace Meson-generated headers. No Meson/Ninja step, CLI tools, examples or tests are part of the package build.
