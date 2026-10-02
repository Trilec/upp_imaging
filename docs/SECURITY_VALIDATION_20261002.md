# Focused Windows security validation — 2 October 2026

Status: PARTIAL. Prior delivery checkpoint f1194ea42318ee804509a050823e6df49e2c0bbf is published on main. The runs below compile the subsequent security worktree; the commit containing this record identifies that source checkpoint. No Release, sanitizer, fuzz, Linux or macOS execution is claimed.

| Debug target | Passed / failed | Exit | Run evidence |
| --- | --- | --- | --- |
| imaging_video_test | 24 / 0 | 0 | exec-E86EFA1B1CEE42EA7B1FDC2E2DC01736 |
| minizip_ng_test | 13 / 0 | 0 | exec-C76621C10151082B9949C557D9606787 |
| yaml_cpp_test | 5 / 0 | 0 | exec-92D808847C4ADA58EAB8E74E864C9DBE |
| opencolorio_test | 18 / 0 | 0 | exec-759402B22057A06433C9609BEAF97300 |
| imaging_workbench_ocio_test | 166 / 0 | 0 | exec-0EF0FE6E330FD6641EF2F9DF7EE776F5 |
| libpng_src_roundtrip_test | 5 / 0 | 0 | local CLANGx64 build/run, 2 October |
| libpng_roundtrip_test | 5 / 0 | 0 | local session 49625, build and run exits 0 |
| imaging_raster_test | 57 / 0 | 0 | local build session 97694 and run session 54252 |

Total: 293 checks, zero failures, eight test exits at 0. The 166-check Workbench run followed the video/minizip repair and preceded the PNG refresh. The 57-check all-format raster and both PNG tests ran after the PNG refresh. The actual Debug Workbench was rebuilt afterward in local session 54252, build exit 0. No duplicate integrated suite or GUI checklist was run.

The previous full Debug ledger remains 51 targets / 1,482 checks at the delivery checkpoint. The new minimum is 1,493 checks; a full 1,493-check run has NOT been performed. Initial failed repair iterations were followed by successful fresh builds. A stale 12-check minizip executable run (exec-C744149CD7C5DA3FA61184AEED2CF721) is excluded from new-source evidence.

## Identified artifacts

All paths below are relative to the repository and contain Debug executables.

| Path | SHA-256 |
| --- | --- |
| build/ImagingWorkbench_debug.exe | 8360206cc4b67a90f5d360864d12331d72fe069160a30e7252c1bd5b38b54ca3 |
| build/imaging_video_debug.exe | 5c93702589b8f2fa1d4cbaf40aff31a876b9bc30a14b4c6dd8bd55e31134d90d |
| build/minizip_ng_test_debug.exe | 7f0522003ca8df022fd7aa72efd6298d898ecba3f40b45ab65d1d9987479ec17 |
| build/yaml_cpp_test_debug.exe | 0611d2f0b27c12f686a8159c988eacc4dbb251dd4841ec3699f2ede700c8162b |
| build/opencolorio_security_debug.exe | ae309615b7f4ed7fa79b183d9dd2aab46589fc289e30ff436caa80bbc2712c0c |
| build/imaging_workbench_security_debug.exe | 85b1a96a169ebd84c111b29f7655a8b473a541d730c1a3bd5ff0e968a7f44133 |
| build/libpng_src_security_debug.exe | ec7f66bf1bdbfc93b4ea97f64e005387808cb15534d2b49983532a5215c99e82 |
| build/libpng_security_debug.exe | 1d070f93de12e594b7b31ef8f7226e90b3c6911c51bea457c991fcc01b28d038 |
| build/imaging_raster_security_debug.exe | 4729cf4120af07dec047cc6b71504418388adacd56add08b383e0e460b52a828 |

Workbench manually accepted by Curt. The current Debug hash identifies this new build; it does not establish the identity of Curt's earlier executable. Existing ImagingIO LNK4217 import warnings remain; the final application build succeeded.

## Remaining gates

Expat CVE-2025-66382 through OCIO XML FileTransforms remains unresolved. Native metadata/scratch/reference allocations and cooperative decode deadlines are not a hard isolation boundary. Complete the remaining dependency-family/provider review, including Draw's separately installed libpng 1.6.54 (inventoried; its PNGRaster wrapper never calls png_read_end). Security clearance and final Release publication are separate gates; bin/windows-x64/ImagingWorkbench.exe was not replaced.

Klick disconnected during the PNG refresh. Verification found no applied PNG mutation or pending transaction; local tools applied the refresh with checked file preconditions and a retained build/security-png-local-receipt.json. Klick subsequently reconnected. No branch, source tree or retained evidence was deleted in this security block.
