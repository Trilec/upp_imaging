OpenColorIO 2.5.2 source package.

Archive: `OpenColorIO-2.5.2.tar.gz`
Official source: `https://github.com/AcademySoftwareFoundation/OpenColorIO/releases/download/v2.5.2/OpenColorIO-2.5.2.tar.gz`
SHA-256: `CB8B0AE38FA523BE8F899A0B2D6B8CA8CBCDA7BC4322C91D1AC2B6B2A0082474`
License: BSD-3-Clause
Import date: 2026-07-08

Generator: `E:\upp-18468\bin\clang\python\bin\python.exe tools/generate_ocio_builtin_configs.py`
Check mode: `E:\upp-18468\bin\clang\python\bin\python.exe tools/generate_ocio_builtin_configs.py --check`

Generated files:
- `generated_include/CG.cpp` - 641063 bytes, `FB88F58178CE3AAACB815FE2CA6F16282FD2B2187CE2610C1550ED209470712F`
- `generated_include/Studio.cpp` - 1669850 bytes, `573E6DEBF7C803B69792B8F50708167A737F9C2804A583DF3DA1FBBF5ACB8A96`

Relative include roots:
- `-I.`
- `-Iupstream/src`
- `-Iupstream/src/OpenColorIO`
- `-Icompat_include`
- `-Igenerated_include`
- `-Iext/sampleicc/src/include`
- `-Iext/xxHash/src/include`

Dependencies: expat, yaml_cpp, pystring, minizip_ng, imath, zlib.
Bundled internals: ext/sampleicc, ext/xxHash.

Configuration:
- CPU/GPU core enabled
- SIMD disabled (`OCIO_USE_SSE2`, `OCIO_USE_AVX`, `OCIO_USE_AVX2`, `OCIO_USE_AVX512` all `0`)
- system-monitor enumeration headless (`OCIO_HEADLESS_ENABLED`)
- no ICC profile discovery through `SystemMonitors` yet
- bgfx renderer, viewer integration, and bindings are not part of this package

Generated headers: `OpenColorIO/OpenColorABI.h`, `OpenColorIO/OpenColorIOConfig.h`, `upstream/src/OpenColorIO/CPUInfoConfig.h`.

Validated toolchain: `CLANGx64` (`clang++ 21.1.1`, target `x86_64-w64-windows-gnu`).

Known warnings: case-mismatch `OpenColorIO/...` include-path warnings on Windows, plus the usual upstream chatter.
