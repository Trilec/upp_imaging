# Active Work

The repository structure migration is complete and accepted. This document travels
with the migration commit; resolve its final SHA with `git rev-parse HEAD`.

- START SHA: `db87e3787e1d01cb75c982ae7ee0c315161b431f`.
- Source root directories: 177 to 8 (ignored `out/` is build output).
- Packages: 174 to 124; 50 removed, zero merged.
- Test/probe/benchmark packages: 91 to 50; 49 deterministic tests and one manual
  benchmark remain. The 41 removals and their reasons are individually recorded.
- Authoritative acceptance: all 49 tests in Debug and Release, 98 normal exits,
  2734 passed checks, zero failures.
- Production app/benchmark builds and separate openjph public API checks pass
  in both configurations. Release benchmark quick smoke passes; Debug manual
  benchmark exceeded its observation timeout.
- Pinned upstream content and all eight submodule revisions are unchanged.

See [migration decisions](STRUCTURE_MIGRATION.md),
[acceptance evidence and commands](WINDOWS_ACCEPTANCE.md), and
[current package layout](package_layout.md).
The prior hardening review remains historical context and records deferred
performance work; its removed probe packages are no longer acceptance gates.

Final publication checks: clean staged diff, fetch before ordinary push, no force,
HEAD equal to origin/main, and a clean worktree.
