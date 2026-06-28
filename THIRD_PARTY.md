# Third-Party Code

## zlib

- Upstream name: zlib
- Upstream version: 1.3.2
- Upstream authors: Jean-loup Gailly and Mark Adler
- Source archive used: `https://zlib.net/zlib-1.3.2.tar.gz`
- SHA-256: `BB329A0A2CD0274D05519D61C667C062E06990D72E125EE2DFA8DE64F0119D16`
- License: zlib License
- Date imported: 2026-06-28
- Files modified after import: No upstream files modified

Notes:

- The imported upstream source tree is preserved under `zlib/upstream/`.
- The current Windows `Core` + `zlib` proof build reuses U++ `plugin/z` linkage to avoid duplicate static zlib symbols, because `Core` already links `plugin/z` on Windows.
