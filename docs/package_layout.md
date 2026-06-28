# Package Layout

This repository is one U++ nest that can host many independent packages.

Rules for package layout:

- one repo/nest, many packages
- each package should depend only on what it needs
- third-party source should stay close to the package that owns it
- future packages should be importable independently when practical

Expected dependency direction:

- `zlib` is standalone
- future `libpng` should depend on `zlib`
- future OpenEXR packages will likely depend on Imath and may also depend on zlib
- OpenImageIO may remain external or become an adapter later, depending on complexity
