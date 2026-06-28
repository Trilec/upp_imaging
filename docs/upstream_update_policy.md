# Upstream Update Policy

- Record the upstream source version for each imported package.
- Record the source archive and SHA-256 hash.
- Avoid local patches where possible.
- If patches are required, document them in the package README and `THIRD_PARTY.md`.
- Update third-party code package by package instead of mixing unrelated upgrades.

For the initial `zlib` import, no upstream source files were modified.
The current Windows packaging workaround is implemented only in package metadata and wrapper headers, not in upstream zlib sources.
