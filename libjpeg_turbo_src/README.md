# libjpeg_turbo_src

Strict upstream libjpeg-turbo 3.2.0 source package.

## Provenance

- version: `3.2.0`
- source archive: `libjpeg-turbo-3.2.0.tar.gz`
- SHA-256: `6f30092cef9fb839779646608f4ee14ae3cbac989c47fa05e841b0841f09878e`
- upstream license: IJG License plus Modified BSD License, as rolled up in `LICENSE.md`
- import date: `2026-07-06`

## Local configuration

```text
ENABLE_STATIC = ON
ENABLE_SHARED = OFF
WITH_SIMD = OFF
WITH_TURBOJPEG = OFF
WITH_TOOLS = OFF
WITH_TESTS = OFF
WITH_PROFILE = OFF
WITH_ARITH_ENC = ON
WITH_ARITH_DEC = ON
WITH_JPEG7 = OFF
WITH_JPEG8 = OFF
```

## Generated headers

The checked-in generated headers are kept in `generated/`:

- `generated/jconfig.h`
- `generated/jconfigint.h`
- `generated/jversion.h`

These headers are derived from the official 3.2.0 `.in` templates for the fixed
Windows CLANGx64 scalar configuration.
