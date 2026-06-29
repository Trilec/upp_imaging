# Upstream Update Policy

- Record the upstream source version for each imported package.
- Record the source archive and SHA-256 hash.
- Avoid local patches where possible.
- If patches are required, document them in the package README and `THIRD_PARTY.md`.
- Update third-party code package by package instead of mixing unrelated upgrades.

For the initial `zlib` import, no upstream source files were modified.
The current Windows compatibility workaround is implemented only in package metadata and wrapper headers, not in upstream zlib sources.

For the initial `libpng` import, no upstream source files were modified.
The release-provided `scripts/pnglibconf.h.prebuilt` file was copied into the imported source tree as `libpng_src/upstream/pnglibconf.h` instead of being regenerated locally.

For the initial `Imath` import, no upstream source files were modified.
The generated config header `imath_src/upstream/ImathConfig.h` was created from upstream `config/ImathConfig.h.in` using upstream release defaults required for the local static package build and should be treated as packaging metadata, not as an upstream source patch.
