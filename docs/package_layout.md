# Package Layout

This repository is one U++ nest that can host many independent packages.

Rules for package layout:

- one repo/nest, many packages
- each package should depend only on what it needs
- third-party source should stay close to the package that owns it
- future packages should be importable independently when practical

Expected dependency direction:

- `zlib_src` is the strict upstream-source base package
- `zlib` is the user-facing compatibility layer
- future `libpng` should depend on `zlib`
- future OpenEXR packages will likely depend on Imath and may also depend on zlib
- OpenImageIO may remain external or become an adapter later, depending on complexity

Current zlib package policy:

- `zlib_src` builds imported upstream zlib 1.3.2 directly
- `zlib` provides a stable include path for normal apps
- on Windows/Core, `zlib` may resolve through U++ `plugin/z` to avoid duplicate symbols
- strict proof of vendored upstream linkage belongs to `zlib_src_test`
