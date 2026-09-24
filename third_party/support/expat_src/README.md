# expat_src

Strict upstream Expat 2.8.5 source package.

## Provenance

- version: `2.8.5`
- archive: `expat-2.8.5.tar.gz`
- upstream source location: `https://github.com/libexpat/libexpat/releases/download/R_2_8_5/expat-2.8.5.tar.gz`
- SHA-256: `920DDE485E15EDA0CCE8D2310B41D492C534E5E3D89AD407A0B4176DD2FF88FE`
- license: MIT/X Consortium license
- import date: `2026-09-24`

## Local configuration

```text
static library
XML_DTD enabled
XML_NS enabled
XML_LARGE_SIZE enabled
Windows rand_s entropy provider compiled
examples disabled
xmlwf disabled
tests disabled
shared library disabled
Windows x64 / CLANGx64
```

## Generated headers

- `expat_config.h`

Mirrored into `upstream/expat_config.h` and
`upstream/lib/expat_config.h` for the imported source tree.
This repository-generated header declares only validated Windows capabilities
and fails explicitly on another platform. Linux and macOS require a separately
probed configuration before those builds can be claimed.

## Disabled features

- examples
- xmlwf
- tests
- shared library

## Validated toolchain

- U++ `umk`
- `CLANGx64`
