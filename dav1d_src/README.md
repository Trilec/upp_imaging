# dav1d_src

Pinned scalar dav1d AV1 decoder backend for the HEIF/AVIF import stack.

- upstream release: 1.5.4 (`Sonic`)
- exact upstream/mirror commit: `54706fc6bc0cdecab7e9593974a4039cc038fca7`
- canonical upstream: VideoLAN dav1d
- repository submodule uses the read-only `libsdl-org/dav1d` GitHub mirror of the canonical source so the dependency remains reproducible through the repository tooling
- license: BSD-2-Clause

The package compiles dav1d's ordinary scalar library sources plus the Windows thread shim. The 13 upstream bit-depth template sources are instantiated separately for 8-bit and 16-bit through repository-owned wrappers. Assembly/SIMD is deliberately disabled in the first U++ slice; both AV1 bit-depth paths remain enabled.

Repository-owned `config.h` and `vcs_version.h` replace Meson-generated headers. No Meson/Ninja step, CLI tools, examples or tests are part of the package build.

On Windows, the repository-generated config follows dav1d's upstream feature-test branch: when the active CRT already declares `fseeko`/`ftello`, it enables `_FILE_OFFSET_BITS=64` instead of aliasing those names to `_fseeki64`/`_ftelli64`. The U++ CLANGx64 MinGW/UCRT headers provide the POSIX names, so defining the aliases would rewrite declarations inside `<stdio.h>` and create conflicting prototypes.
