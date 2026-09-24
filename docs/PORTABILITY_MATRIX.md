# Portability matrix

Checked 2026-09-24. Source review and Windows execution are separate evidence.
A named target is not a passed target.

| Target | Build evidence | Runtime evidence | Status |
| --- | --- | --- | --- |
| Windows x64, U++ CLANGx64 | Focused Core/IO/EXR and Expat builds use the new intermediate root; complete retained build pending | Focused deterministic tests pass; Workbench manual checks pending | PARTIAL |
| Linux x64 | No local Linux compiler/U++ assembly; Expat generated config deliberately rejects non-Windows; FFmpeg generated `config.h` declares x86_64 and Windows capabilities | None in this checkpoint | BLOCKED / untested |
| macOS ARM64 | No macOS host/U++ assembly; Expat generated config deliberately rejects non-Windows; FFmpeg generated `config.h` assumes x86_64 | None | BLOCKED / untested |
| macOS x64 | Not adopted as a supported release target | None | Unsupported |

The Windows Expat `expat_config.h` previously asserted Unix APIs including
`mmap`, `unistd.h` and `/dev/urandom`. The 2.8.5 update now declares
only the validated Windows capabilities and compiles its `rand_s` entropy
source. This removes false capabilities from that Windows build; it does
not produce a POSIX configuration. `third_party/ffmpeg/ffmpeg_headers/generated/config.h`
still declares `ARCH_X86_64=1`, `HAVE_WINDOWS_H=1` and
`HAVE_PTHREADS=0`. Those values must be regenerated and validated for
each target before the FFmpeg slice can build there.

The EXR round-trip tests now use a runner-provided runtime directory,
falling back to the platform temp directory when run standalone. This
removes their repository-specific absolute path and source-tree `out/`
writes. Windows Unicode paths, large-file behavior, case-sensitive
includes, TLS/static destruction and native decoder capability macros
still need dedicated cross-platform tests. Source review alone is not
runtime proof.

## Next-machine setup and first gates

Install a U++ checkout and native C/C++ toolchain for the target.
Create a `GitHubOut` assembly with the same source nests as
`GitHubOut.var.example`, replacing every path with an absolute path
valid on that machine. Set its `OUTPUT` to an absolute
`<checkout>/build/linux-x64/umk` or
`<checkout>/build/macos-arm64/umk`. Use the native U++ method selected
on that machine; the Windows `CLANGx64` method is not portable by name.

From a POSIX shell at the repository root, after setting the three
machine-specific values:

```sh
UMK=/absolute/path/to/umk
METHOD=CLANG                         # replace with the configured native method
TARGET=linux-x64                    # use macos-arm64 on Apple Silicon
mkdir -p "build/$TARGET/validation/debug" "build/$TARGET/validation/release"
"$UMK" GitHubOut imaging_core_test "$METHOD" -H8 "build/$TARGET/validation/debug/imaging_core_test"
"build/$TARGET/validation/debug/imaging_core_test"
"$UMK" GitHubOut imaging_core_test "$METHOD" -rH8 "build/$TARGET/validation/release/imaging_core_test"
"build/$TARGET/validation/release/imaging_core_test"
```

These first commands do not constitute acceptance. Before extending to
the 49-package manifest, generate and review native configurations for
Expat, FFmpeg and any other Windows-pinned dependencies. Then run the
complete retained Debug and Release suite, Workbench build and manual
checks, and a supported ASan/UBSan configuration on the target machine.
Record exact source SHA, method, logs and exits. No Linux/macOS sanitizer
or fuzz execution has occurred in this Windows checkpoint.
