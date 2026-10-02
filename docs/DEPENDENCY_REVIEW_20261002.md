# Dependency disposition review — 2 October 2026

Baseline: ed15a3aaf855e6100df87bda2701dc8555b91905. This records the bounded
Windows source/configuration review separately from execution and security
clearance. A release number or an empty advisory page is not proof of safety.

| Family / retained version | Compiled boundary and reviewed disposition |
| --- | --- |
| TIFF 4.7.2 | Static library; CCITT/LZW/PackBits/LogLuv/ZIP/libdeflate enabled. JPEG/OJPEG/JBIG/LERC/LZMA/ZSTD/WebP codecs and TIFF command-line tools disabled. The 4.7.2 library release includes allocation/overflow/IFD-loop repairs. Tool advisories do not describe this compiled slice. The repository TiffInput.cpp overlay now bounds native allocations to 256 MiB each / 512 MiB cumulatively for filename and IOProxy reads; lower OIIO limits:imagesize_MB also lower these budgets. Oversized optional metadata can be skipped while image opening succeeds; this is not a hard process cap. |
| WebP 1.6.0, 4fa21912338357f89e4fd51cf2368325b59e9bd9 | Scalar decoder/encoder/SharpYUV/mux/demux with threads; no upstream tools. Retained NEWS includes earlier lossless decoder hardening and subsequent maintenance fixes. No new applicable fix was selected in this review; native scratch remains outside application buffer caps. |
| LibRaw 0.22.2, b93f6e45c194f5df9b02a43b1af9a54b4f41f33f | Camera/DNG/internal lossless-JPEG paths compiled; RawSpeed, external JPEG, Adobe DNG SDK and LCMS disabled. Changelog contains 0.22.1 TALOS-2026-2364/2363/2359/2358/2331/2330 repairs, followed by 0.22.2 size, recursion and metadata fixes. OIIO applies raw:max_raw_memory_mb before unpack; ImagingRaster supplies its decoded budget. This does not cap every metadata allocation. |
| libjpeg-turbo 3.2.0 | Static scalar IJG API; TurboJPEG API, tools and SIMD disabled. Official source-asset SHA matches the recorded import (6f30092cef9fb839779646608f4ee14ae3cbac989c47fa05e841b0841f09878e). The new ordinary OIIO JPEG reader/writer links it; TIFF JPEG and JXL JPEG reconstruction remain disabled. |
| JXL 0.12.0, a7a9c787341cf703dede03c2009fa460cae5e5df | Boxes/core decoder/encoder enabled; JPEG reconstruction, tools/extras/tests omitted. GHSA-4qrh-mvcg-349p and GHSA-fw8c-5996-j6q5 affect versions before 0.6.1; this retained version is outside those ranges. Version 0.12.0 also tracks temporary codestream/box allocations; caller limits still are not a whole-decoder memory cap. |
| Highway 457c891775a7397bdb0376bb1031e6e027af1c48 | Seven core translation units selected by JXL import.ext; contrib/tests/examples omitted. Maintainer advisory page has no published advisories on inspection. This is a reviewed source boundary, not a no-vulnerabilities assertion. |
| skcms 96d9171c94b937a1b5f0293de7309ac16311b722 | JXL compiles its production skcms.cc colour-management implementation. ICC-related native allocations remain inside the decoder trust boundary. The exact local retained commit and compiled source boundary were reviewed. The remote pinned-source lookup remained unavailable; no independent skcms advisory clearance is claimed. ICC processing remains an explicitly accepted native-parser risk within the trusted-input scope; a broader advisory audit remains future work. |
| Imath 3.2.2 | Math/half provider used by OIIO/OpenEXR. The retained release notes describe build/declaration/comment fixes. This component is not an independent image/container decoder. |
| libdeflate 1.25 | Standalone compression provider. Retained NEWS includes the earlier clang Debug checksum repair (1.23) and later build fixes; gzip command-line expansion behavior is not the compiled application path. Output-buffer lengths remain the consumer's responsibility. |
| fmt 12.2.0 | Local FMT_VERSION is 120200. GHSA-65g5-63wg-xjh4 affects macOS fmt::say in versions >=8,<12; this Windows build and retained version are outside that scope. No fmt::say caller was found in imaging/integrations/apps. |
| pystring 1.1.4 / robin-map 1.4.1 | String utilities and header-only hash containers; no file decoder/tool process. No justified update selected. Their use does not remove size/iteration limits required at callers. Robin-map release notes describe a CMake change. The official pystring security page was subsequently inspected and listed no published advisories; absence is not proof of safety. |
| Draw plugin/png 1.6.54 | Installed separately at E:/upp-18468/uppsrc/plugin/png. PNGRaster never calls png_read_end; the named CVE-2026-46675 call sequence is absent from that wrapper. Raw libpng API/version and other advisories remain separate; repository libpng 1.6.59 updates did not replace this provider. |

Earlier Expat, HEIF, OIIO, OpenEXR/OpenJPH, FFmpeg, zlib, minizip, YAML and
Brotli findings remain in THIRD_PARTY_SECURITY.md. Their recorded patches and
applicability conclusions are preserved; no whole-tree audit is inferred.

## Primary evidence

- TIFF: https://libtiff.gitlab.io/libtiff/releases/v4.7.2.html
- LibRaw: https://github.com/LibRaw/LibRaw/blob/0.22.2/Changelog.txt
- JPEG: https://github.com/libjpeg-turbo/libjpeg-turbo/releases/tag/3.2.0
- WebP: retained upstream/NEWS and https://chromium.googlesource.com/webm/libwebp/
- JXL: https://github.com/libjxl/libjxl/releases/tag/v0.12.0
- JXL advisories: https://github.com/libjxl/libjxl/security/advisories/GHSA-4qrh-mvcg-349p and https://github.com/libjxl/libjxl/security/advisories/GHSA-fw8c-5996-j6q5
- Highway: https://github.com/google/highway/security/advisories
- Imath: https://github.com/AcademySoftwareFoundation/Imath/releases/tag/v3.2.2
- libdeflate: https://github.com/ebiggers/libdeflate/blob/v1.25/NEWS.md
- fmt: https://github.com/fmtlib/fmt/security/advisories/GHSA-65g5-63wg-xjh4
- pystring: https://github.com/imageworks/pystring/security
- skcms pinned source (lookup unavailable): https://skia.googlesource.com/skcms/+/96d9171c94b937a1b5f0293de7309ac16311b722
- robin-map: https://github.com/Tessil/robin-map/releases/tag/v1.4.1

Disposition: complete for the user-approved Windows trusted-local milestone.
Curt selected trusted local inputs with enforced restrictions on 2 October;
OCIO XML input is disabled, TIFF allocations are bounded, and shared image,
config and LUT preflight limits are implemented. INPUT_POLICY.md states the
remaining native-library, config-reference and advisory-coverage limitations.
No unresolved known finding is represented as a library fix or a hostile-input
safety guarantee. Independent skcms advisory review, isolation and broader
platform validation remain future work outside this milestone. Final Debug
execution evidence is recorded separately in DELIVERY_COMPLETION_20261002.md.
No new Release validation or publication is claimed.
