# Third-party security review

Checked 2026-09-24. This is a staged audit, not a release security
certificate. The Expat family below has source and Windows build evidence;
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

Validation: `expat_test` and the OCIO/Workbench caller tests run in Debug
and Release through `tools/validate.ps1`; exact totals and exit codes
belong in `docs/ACTIVE_WORK.md` once the checkpoint finishes. The
64-iteration malformed UTF-16 case runs on the caller thread.

## Remaining graph and release boundary

The current build includes pinned libheif with libde265 and dav1d,
OpenImageIO source/header/plugin copies, OpenEXR with Imath, OpenJPH and
libdeflate, JPEG XL with nested Brotli/Highway/skcms, OpenColorIO with
bundled internals, minizip-ng, yaml-cpp, and the bounded FFmpeg source
slice. PNG/JPEG/WebP/TIFF/RAW/support packages and U++ `plugin/z` must
also be traced to the actual linked provider. These families are not
covered by the Expat checkpoint. Exact pins and enabled slices are in
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
