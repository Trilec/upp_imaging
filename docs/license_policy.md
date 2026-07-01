# License Policy

- Each upstream library keeps its own license.
- Upstream license text should be preserved in the repository.
- `THIRD_PARTY.md` tracks imported upstream code and import details.
- Upstream copyright and notice text must not be removed.
- Release-provided generated headers that are required for a clean import, such as libpng's `pnglibconf.h`, should be preserved and documented.
- Repository-owned generated config headers derived from upstream templates, such as Imath's `ImathConfig.h`, should be documented clearly as packaging metadata.
- When a library is useful on its own, prefer its official standalone release over a vendored copy from another upstream project when that choice stays compatible with the dependency branch.
- The same rule applies to OpenJPH here: prefer the official standalone release when it can be matched cleanly to the OpenEXR dependency version.
