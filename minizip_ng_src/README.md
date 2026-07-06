# minizip_ng_src

Strict upstream minizip-ng 4.0.10 source package.

## Provenance

- version: `4.0.10`
- archive: `minizip-ng-4.0.10.tar.gz`
- upstream source location: `https://github.com/zlib-ng/minizip-ng/archive/refs/tags/4.0.10.tar.gz`
- SHA-256: `C362E35EE973FA7BE58CC5E38A4A6C23CC8F7E652555DAF4F115A9EB2D3A6BE7`
- license: zlib
- import date: `2026-07-06`

## Local configuration

```text
static library
MZ_COMPAT=OFF
MZ_ZLIB=ON
MZ_PKCRYPT=OFF
MZ_WZAES=OFF
MZ_OPENSSL=OFF
MZ_LIBBSD=OFF
MZ_BZIP2=OFF
MZ_LZMA=OFF
MZ_ZSTD=OFF
MZ_LIBCOMP=OFF
MZ_ICONV=OFF
MZ_BUILD_TESTS=OFF
MZ_BUILD_UNIT_TESTS=OFF
MZ_BUILD_FUZZ_TESTS=OFF
Windows CLANGx64
```

## Generated headers

None.

## Disabled features

- compatibility API
- PKWARE AES
- OpenSSL
- Windows Cryptography
- bzip2
- LZMA
- ZSTD
- Apple compression
- iconv
- tools
- tests

## Validated toolchain

- U++ `umk`
- `CLANGx64`
