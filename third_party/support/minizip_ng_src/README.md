# minizip_ng_src

minizip-ng 4.2.2 compiled source and public headers, with a small documented
Windows extraction-path overlay.

## Provenance

- official tag: `4.2.2`
- Git commit: `7b2387161c542fa9f427352dcdef76097d0d692b`
- source: https://github.com/zlib-ng/minizip-ng/tree/4.2.2
- refresh date: `2026-10-02`
- license: zlib, retained in `upstream/LICENSE`

All previously retained root C/header files, public header copies, upstream
README and CMakeLists are refreshed from this tag (LF normalization).
Unused auxiliary directories retain the original 4.0.10 import; they are
not part of the compiled U++ slice. This is not a complete CMake source import.

## Local overlay

`upstream/mz_os.c`, `mz_path_is_symlink_target_safe`: reject parent/target
combinations that exceed its 1024-byte buffers before copying. On Windows,
reject colon-bearing targets (drive-qualified paths and alternate streams).
The upstream 4.2.2 containment checks otherwise remain unchanged. Do not
describe this file as byte-identical upstream. These defenses follow direct
source inspection, not a claimed upstream advisory or CVE assignment.

## Compiled configuration

The U++ manifest retains zlib, memory/buffer/split streams and Windows OS
providers. Compatibility, PKCRYPT/WZAES, OpenSSL, bzip2, LZMA, ZSTD, Apple
compression, iconv, PPMd, tools and upstream tests remain disabled.
Two matching repository-generated mz_config.h copies select the Windows CRT
headers/OS providers and HAVE_ZLIB=1 with ZLIB_COMPAT=1 for the repository's
zlib ABI. The upstream/zlib.h bridge forwards to that provider. ZLIB_COMPAT
does not enable the disabled minizip compatibility API. Other codec/crypto macros remain
undefined. Their platform guard rejects unvalidated non-Windows use. This also
makes deflate handling explicit; the old tests covered only stored ZIP entries.
The upstream CMake build and non-Windows providers have not been validated.

Focused Debug evidence is recorded in docs/THIRD_PARTY_SECURITY.md.
