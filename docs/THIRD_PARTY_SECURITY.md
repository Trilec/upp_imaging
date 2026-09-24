# Third-party security review

Checked 2026-09-24. This is a staged audit, not a release security
certificate. Expat, libheif, OpenImageIO and the bounded FFmpeg slice below
have source and focused Windows build evidence;
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
| CVE-2025-66382 | All releases, no fixed version announced | Upstream reports crafted 2 MiB XML can consume 25–100 seconds. OCIO's CTF/CLF, CDL and Iridas `.look` readers call `XML_ParserCreate` and feed complete user-selected XML streams to `XML_Parse`; Workbench FileTransform and file-backed OCIO configs can reach those readers. The trigger is non-public, so issue-specific exploitability remains unproven. | **Unresolved release gate** |

The maintainer also tracks further non-public reports in
[upstream issue #1160](https://github.com/libexpat/libexpat/issues/1160).
The `.ocio` configuration parser itself is YAML; file transforms referenced
from a config can still reach the XML readers. Neither extension filtering
nor a file-size cap excludes the reported ~2 MiB case. The unresolved DoS is described in
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
opening 64 times on the caller thread. The later clean Windows suite passed
and Workbench was manually accepted by Curt; remaining security gates are
separate. The old 1.23.1 build is affected and must not be published.

## OpenImageIO: 3.1.17.0 focused validation complete

The previous linked OIIO main, utility, public-header and static-plugin
family was 3.1.15.0 at `cbe57bc005678ca310835473568121719861734c`.
The source tree is now refreshed to the stable 3.1.17.0 tag
`73bc189f7d8469a9760ce9c5099b686c77695074` (checked 2026-09-24).
This repository compiles separately copied main/utility/header files and
direct plugin sources from a pinned submodule, so advancing only the
plugin pointer would create a mixed-version library. The MinGW
main-thread input/output/global error wrappers are separate downstream
files and have a distinct lifetime contract.

The [3.1.16.0 release](https://github.com/AcademySoftwareFoundation/OpenImageIO/releases/tag/v3.1.16.0)
fixes Cineon invalid-bit-depth heap overflow (`CVE-2026-63638`) and
OpenEXR edge-tile heap write (`CVE-2026-63422`), both in linked readers
reachable from user images. It also hardens linked DPX, EXIF, filesystem
and other readers, and adds the `limits:resolution` per-dimension guard
to complement `limits:imagesize_MB`. The
[3.1.17.0 release](https://github.com/AcademySoftwareFoundation/OpenImageIO/releases/tag/v3.1.17.0)
adds reader `check_open()`/compression-ratio coverage, shared EXIF/ICC
bounds fixes, and RAW/TIFF/JPEG XL hardening. These are in the retained
reader surface; exact issue-by-issue reachability depends on the enabled
format and caller, but the named Cineon/EXR defects are clearly relevant.
The copied main, utility and public-header slices and the plugin submodule
are now version-consistent. Existing robinmap/Imath include adaptations and
the separate MinGW main-thread error wrappers remain; upstream's changes
do not prove that distinct shutdown workaround unnecessary.
The final focused matrix passed 10 affected targets in each of Debug and
Release: 306 checks, 20 exits at 0. This includes the 20-check DPX/Cineon
test's repeated malformed opens on the ordinary caller thread. The later
clean Windows suite passed and Workbench was manually accepted by Curt.
Other security gates still prevent a releasable binary.

The copied 3.1.17.0 backend defaults to `limits:channels=1024`,
`limits:resolution=1048576` per dimension and `limits:imagesize_MB` to the
smaller of 32768 or detected physical memory. The repository wrapper now
sets 65,536 pixels per dimension and 2,048 MiB uncompressed image data
before registering readers. OIIO checks these when opening supported
images; ImagingCore separately rejects buffers over `INT_MAX` bytes.
These limits cover the ordinary OIIO read path and the Workbench, but not
arbitrary metadata expansion inside format parsers or OCIO XML parsing.
Workbench still enumerates subimages for display without a count cap;
metadata and frame/subimage policies remain a release gate. The OIIO
global attributes are configurable by an embedding application after
initialization for workloads that need a documented larger budget.

## OpenEXR: 3.4.13 to 3.4.14

The linked OpenEXR and OpenEXRCore imports now contain all 20 changed
implementation/header files from the official
[3.4.14 security tag](https://github.com/AcademySoftwareFoundation/openexr/releases/tag/v3.4.14),
commit `777d231a179de1711d2a942810d9559216ab3f4c`.
Iex and IlmThread source files did not change between the two tags;
repository-generated configuration and include bridges were versioned
together. One upstream blank-line whitespace cleanup was made in
`internal_dwa_compressor.h`; the other changed imports match the tag
after line-ending normalization. The 3.4.14 release fixes malformed
metadata and decompression defects in linked readers, including
`CVE-2026-61555` in `ImfMultiView`; it also fixes ILP32 defects that do
not apply to this x64 build. OpenEXRUtil, Python bindings and command-line
tools are not linked here, so their advisory paths are not reachable.
The linked OpenJPH provider is the standalone 0.27.1 release, while
OpenEXR 3.4.14 vendors 0.31.0. The latter has an API/ABI break and was
not substituted into this package graph. The official
[OpenEXR advisory](https://github.com/AcademySoftwareFoundation/openexr/security/advisories/GHSA-2f85-52wj-hc3c)
identifies an assertion reached by a malformed JPEG 2000 QCD marker in
OpenJPH 0.26.3 and names 0.27.1 as fixed. The imported 0.27.1 source
replaces that assertion with input validation. All changed core files
between official 0.26.3 and 0.27.1 were imported, including the
`ojph_mem_c.c` rename; one upstream whitespace-only blank line was
normalized in `ojph_file.h`. OpenEXR's generated config names 0.27.1.
The focused Windows EXR Core, high-level and ImagingIO tests passed
106 checks per configuration after this update, 212 checks and six clean
exits total. Those tests verify compilation and normal decoding, not the
advisory's crafted trigger; malformed HTJ2K coverage remains pending.

## Remaining graph and release boundary

The remaining graph includes libde265 and dav1d below libheif,
Imath, OpenJPH and libdeflate below OpenEXR, JPEG XL with nested
Brotli/Highway/skcms, OpenColorIO with
bundled internals, minizip-ng, yaml-cpp, and the bounded FFmpeg source
slice. PNG/JPEG/WebP/TIFF/RAW/support packages still need review.
These families are not
covered by the Expat or libheif checkpoints. Exact pins and enabled slices are in
the package READMEs and source manifests, but release/advisory
applicability has not been verified for every family. Status:
**unresolved**, not "no vulnerabilities found".

FFmpeg's retained scalar, local-file H.264/MOV/MP4 slice was refreshed
from `n9.0.1` (`bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`) to
`n9.0.2` (`946fcce07b6dcd0331c8cc609192aeff5e1924f8`), the
[official 9.0 maintenance release](https://ffmpeg.org/download.html) of
2026-09-18 (checked 2026-09-24). The submodule supplies the implementation
and public headers; generated Windows configuration and version headers were
updated together. Its `libswscale/format.c` did not change between the tags,
so the separate `chroma_pos_compat.c` materializer remains necessary with
`CONFIG_UNSTABLE=0`.
The local tag object verifies cryptographically against the
[FFmpeg git-tag key](https://ffmpeg.org/git-tag-key.asc) with fingerprint
`DD1EC9E8DE085C629B3E1846B18E8928B3948D64`, matching the
[official download page](https://ffmpeg.org/download.html). GPG reports a
good signature but exit 1 because this published key expired before the
2026-09-18 tag date. Separately, the official 9.0.2 source archive has
SHA-256 `8C3850283EB25FA026482078A04051E0BE17347B09EF81A0849BEC15A96E002E`;
its detached signature verifies with exit 0 against FFmpeg's published
release key `FCF986EA15E6E293A5644F10B4322F04D67658D8`. All 3,923
C/header/assembly files in the four linked library trees match the
pinned checkout after Git line-ending normalization. The local keyring
has no independent web-of-trust certification; the key fingerprints are
matched to FFmpeg's official download page. Exact logs and comparison
script are under `build/windows-x64/release/ffmpeg-origin/`.

The 9.0.2 diff contains a reachable MOV `keys` atom count bound before
allocation (`8a77705045`) and native H.264 direct/slice corrections
(`90f2d59a5b`, `7bc6dfe1f4`, `cb34c72359`, `16dfae5c88`). The
`swscale_unscaled.c` unaligned-access fix (`1b087e9c85`) is also compiled;
the first-frame YUV420-to-RGBA path does not itself exercise that planar
copy case. The local build excludes changed network, encoding, hardware and
other codec paths. These are source-diff applicability findings, not a claim
that every FFmpeg vulnerability is closed; review the
[official security list](https://ffmpeg.org/security.html) as new issues
are published. Six FFmpeg tests passed in both Windows configurations,
including the 27-check first-frame pixel contract; five additional runs
per configuration passed with exit 0.

The installed U++ 18468 `plugin/z` header still declares 1.3.1, but
the `GitHubOut` assembly now resolves U++ Core's `plugin/z` dependency
to `third_party/codecs/plugin/z`, which delegates to the repository's
`zlib_src` 1.3.2 and forwards the matching header. This avoids a second
zlib implementation in the package graph. The [official 1.3.2 release](https://github.com/madler/zlib/releases/tag/v1.3.2)
addresses security-audit findings. Both Windows `zlib_test` configurations
reported runtime 1.3.2 and passed round trips; the version assertion now
checks the compile-time and runtime providers exactly. This resolves the
provider-version mismatch for this assembly. Other assembly/toolchain
configurations must verify their own package resolution.

The security refresh must keep OpenImageIO's separate source copies
consistent and retain the MinGW main-thread error wrappers until a
verified upstream fix replaces their distinct shutdown contract.
HEIF registry teardown, OCIO FileTransform ownership and FFmpeg
generated configuration likewise require explicit review before
their families change.
