# Third-party security review

Checked 2026-09-24. This is a staged audit, not a release security
certificate. The Expat and libheif families below have source and Windows
focused build evidence;
the rest of the retained dependency graph still needs per-family advisory,
configuration and provenance review before a release candidate can be approved.

## Expat: 2.7.2 to 2.8.5

The old 2.7.2 source archive SHA-256 was
`13D42A125897329BFEECAB899CB9B5A3EC8C26072994B5CD4C41F28241F5BCE7`.
The new official `R_2_8_5` release archive SHA-256 is
`920DDE485E15EDA0CCE8D2310B41D492C534E5E3D89AD407A0B4176DD2FF88FE`,
matching the digest on the official GitHub release asset. This is the
latest listed maintenance release as of the checked date. The source
tree under `third_party/support/expat_src/upstream/lib/`, including the
new `random_rand_s.c` source, public headers, and license, comes from
that archive. The three copies of `expat_config.h` are a repository-generated
Windows configuration, not upstream source. No older Expat patch was retained.

The linked Windows provider is `expat_src` via `expat`, used by the
OpenColorIO CTF, CDL and Iridas XML readers. The U++ manifest compiles
`xmlparse.c`, `xmltok.c`, `xmlrole.c` and `random_rand_s.c`.
DTD, namespaces, general entities and large-size support remain enabled.
`xmlwf`, tests, examples and other entropy providers are not compiled.
The generated config fails clearly on non-Windows platforms; a probed
Linux/macOS config is required. The current Workbench can accept user
supplied OCIO XML files, so library parse flaws are potentially reachable.

| Advisory (official Expat change log) | Affected / fixed | Component and current reachability | Disposition |
| --- | --- | --- | --- |
| CVE-2026-93990 | Before 2.8.5 / 2.8.5 | UTF-16 surrogate validation in the library; compiled and reachable through XML input | Updated; repeated malformed UTF-16 regression added |
| CVE-2026-66046, CVE-2026-76641 | Before 2.8.4 / 2.8.4 | Attribute lookup quadratic runtime; compiled and reachable | Updated |
| CVE-2026-45186 | Before 2.8.1 / 2.8.1 | Attribute collision quadratic runtime; compiled and reachable | Updated |
| CVE-2026-41080 | Before 2.8.0 / 2.8.0 | Insufficient hash-salt entropy; library compiled; Windows rand_s provider now linked | Updated; no caller uses deprecated XML_SetHashSalt |
| CVE-2026-24515 | Before 2.7.4 / 2.7.4 | External-entity parser plus unknown-encoding callback; compiled, but those APIs are not used by current OCIO readers | Updated; not reachable through current OCIO callers |
| CVE-2026-25210 | Before 2.7.4 / 2.7.4 | `doContent` buffer-size overflow check; compiled and potentially reachable through XML input | Updated |
| CVE-2026-32776, CVE-2026-32777 | Before 2.7.5 / 2.7.5 | External parameter-entity paths; compiled, but current OCIO readers do not enable parameter entity parsing | Updated; not reachable through current OCIO callers |
| CVE-2026-32778 | Before 2.7.5 / 2.7.5 | `XML_ParserCreateNS` or `XML_ParserCreate_MM` after OOM; compiled, but current OCIO readers use `XML_ParserCreate` | Updated; not reachable through current OCIO callers |
| CVE-2026-56132, CVE-2026-56403 through CVE-2026-56408 | Before 2.8.2 / 2.8.2 | Core parser bounds/overflow paths; compiled; crafted XML may reach them | Updated |
| CVE-2026-72522 | Before 2.8.3 / 2.8.3 | 16-bit XML_Char conversion; not compiled in this UTF-8 XML_Char build | Updated defensively; current config not affected |
| CVE-2026-76956 | Before 2.8.4 / 2.8.4 | getentropy provider; not compiled on Windows | Updated defensively; current provider not affected |
| CVE-2026-50219, CVE-2026-56131, CVE-2026-56412 | Before 2.8.2 / 2.8.2 | Parser re-entry through callbacks; code compiled; current OCIO callers do not re-enter the parser from handlers | Updated; native clients could use those APIs |
| CVE-2026-76957 | Before 2.8.4 / 2.8.4 | Custom encoding callback re-entry; code compiled; current OCIO callers do not install custom encoding callbacks | Updated |
| CVE-2026-56409 through CVE-2026-56411 | Before 2.8.2 / 2.8.2 | xmlwf command-line tool, not compiled | Not affected |
| CVE-2025-66382 | All releases, no fixed version announced | Upstream reports crafted 2 MiB XML can consume 25–100 seconds. OCIO parses user supplied XML; the non-public trigger prevents confirming whether the Workbench pathway reaches it. | **Unresolved release gate** |

The maintainer also tracks further non-public reports in
[upstream issue #1160](https://github.com/libexpat/libexpat/issues/1160).
The unresolved DoS is described in
[upstream issue #1076](https://github.com/libexpat/libexpat/issues/1076).
Release notes and affected components are in the
[Expat 2.8.5 change log](https://github.com/libexpat/libexpat/blob/R_2_8_5/expat/Changes).
A version update does not resolve those undisclosed issues. Do not label
the release secure while the reachable XML boundary is unbounded.

Validation: `expat_test` 4/0, `opencolorio_test` 18/0 and
`imaging_workbench_ocio_test` 136/0 passed in both Windows configurations
through `tools/validate.ps1`, all six exits 0. The 64-iteration malformed
UTF-16 case runs on the caller thread. Exact logs are in
`docs/ACTIVE_WORK.md`.

## libheif: 1.23.1 to 1.23.5

The previous linked `libheif_src` was release 1.23.1 at
`2c4bbb54c2738d4a5efbbe3e5fa1d5d76bb88eb0`. This checkpoint moves
the submodule to the verified official 1.23.5 tag
`413e2a87e6a70b3eccc3a3adc5801179dd2d9e00`. It compiles the
ordinary container parser and colour-conversion core with built-in dav1d
and libde265 decoders, no encoders, WebCodecs, runtime plugins or
experimental/uncompressed codec sources. ImagingIO and the OIIO HEIF
plugin can open user-supplied HEIF/AVIF files, so the normal parser and
decode paths are reachable. The package's registry-lifetime overlay
must be reviewed against each new pin.

The [upstream 1.23.2 security release](https://github.com/strukturag/libheif/releases/tag/v1.23.2)
fixes critical derived-item/duplicate-alpha plane out-of-bounds writes
(`GHSA-g89c-p67h-r497`, `GHSA-2jg2-4ch7-h545`) and derived-image
reference amplification (`CVE-2026-84447`). The relevant container,
plane, grid, identity and overlay code is compiled; user-supplied files
can reach it. Its MIME/unci decompression-bomb fix
(`CVE-2026-84384`) has unresolved applicability to this build's MIME
metadata path; the uncompressed codec source is excluded. Sequence
timing (`CVE-2026-84446`) is compiled, but reachability through the
current still-image callers is unresolved. The unci encoder and
inline-mask API findings are outside the retained user-file decode path.
The [upstream 1.23.3 security release](https://github.com/strukturag/libheif/releases/tag/v1.23.3)
fixes a reachable alpha-reference cycle deadlock
(`GHSA-8fmq-r4pf-7m57`) and image colour-conversion reads
(`GHSA-w7mc-p8jc-p853`, `GHSA-9rj8-5mp5-26c9`). Its uncompressed
decoder and SVT-AV1 encoder flaws are outside the selected source slice;
the oversized crop assertion (`CVE-2026-84450`) is in the compiled
container path and potentially reachable.

The [upstream 1.23.4 security release](https://github.com/strukturag/libheif/releases/tag/v1.23.4)
fixes an unbounded `iinf` child item count and quadratic `iref` scan,
unbounded derived-item cycle recursion, and a parallel grid decode
deadlock. These are in the compiled container/decode slice; current
caller reachability is yes for user files. Its WebCodecs decoder and
encoder-plugin findings are outside this configuration. The
[upstream 1.23.5 security release](https://github.com/strukturag/libheif/releases/tag/v1.23.5)
adds a codec-independent check of coded frame dimensions against
`max_image_size_pixels` before a decoder allocates the frame; upstream
states the mismatch class applies to every codec whose actual frame size
is carried in its bitstream. This potentially affects our dav1d and
libde265 decoders even though the published demonstration used libaom.
The 1.23.5 encoder, JPEG 2000 and uncompressed-image findings are
outside the retained decode-only codec slice. Target 1.23.5 is the
latest listed compatible maintenance release on the checked date.
The repository-generated `heif_version.h` now matches 1.23.5. No new
implementation `.cc` files were added between the pins, so the existing
decode-only manifest still selects the intended source slice. The
upstream `plugin_registry.cc` and normal init/deinit code did not change;
the local registry-lifetime overlay remains and passed its unused-link and
paired-init focused regressions. `heif_imagingio_test` passed 13/0,
`heif_oiio_test` 12/0 and `imaging_test` 6/0 in both Windows
configurations, all exits 0. The new cases pin exact decoded AVIF and HEIC
pixels, reject upstream's 857-byte oversized AV1 header corpus within ten
seconds without replacing the caller's image, and repeat malformed AVIF
opening 64 times on the caller thread. The clean full suite and Workbench
checks remain pending. The old 1.23.1 build is affected and must not be
published.

## Remaining graph and release boundary

The remaining graph includes libde265 and dav1d below libheif,
OpenImageIO source/header/plugin copies, OpenEXR with Imath, OpenJPH and
libdeflate, JPEG XL with nested Brotli/Highway/skcms, OpenColorIO with
bundled internals, minizip-ng, yaml-cpp, and the bounded FFmpeg source
slice. PNG/JPEG/WebP/TIFF/RAW/support packages and U++ `plugin/z` must
also be traced to the actual linked provider. These families are not
covered by the Expat or libheif checkpoints. Exact pins and enabled slices are in
the package READMEs and source manifests, but release/advisory
applicability has not been verified for every family. Status:
**unresolved**, not "no vulnerabilities found".

One concrete pending pin is FFmpeg: the retained scalar, local-file
H.264/MOV/MP4 slice is `n9.0.1` at
`bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`, while the
[official download page](https://ffmpeg.org/download.html) lists
9.0.2 as the current 9.0 maintenance release (2026-09-18).
Applicability of its changes to this exact compiled source slice is
unresolved. A bump must update the source slice, generated configuration,
headers, `chroma_pos_compat.c` parity and five first-frame runs per
configuration together; no version-only edit is justified.

On Windows, `third_party/codecs/zlib/zlib.upp` selects the installed U++
`plugin/z` provider, whose `lib/zlib.h` declares 1.3.1 in the local
U++ 18468 installation. The repository's `zlib_src` copy is 1.3.2 but
is selected only outside Windows. Consequently, a Windows build cannot
claim it links 1.3.2. The [zlib upstream site](https://zlib.net/) lists
1.3.2 and describes security audit fixes since 1.3.1. The exact exposure
of the linked 1.3.1 source is not yet resolved. Updating the installed
U++ tree is outside this task, and changing the repository manifest alone
could create duplicate zlib symbols because U++ Core also uses `plugin/z`.
This remains a release security gate until the actual linked provider is
updated or shown not to contain an applicable defect.

The security refresh must keep OpenImageIO's separate source copies
consistent and retain the MinGW main-thread error wrappers until a
verified upstream fix replaces their distinct shutdown contract.
HEIF registry teardown, OCIO FileTransform ownership and FFmpeg
generated configuration likewise require explicit review before
their families change.
