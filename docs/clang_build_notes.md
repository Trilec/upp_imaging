# CLANGx64 build notes

The Windows acceptance method uses the bundled U++ clang toolchain. Configure the nests in GitHubOut.var.example; the repository root is not an include nest.

U++ Core already links plugin/z on Windows. Public zlib and libpng use that provider to avoid duplicate symbols. The standalone libpng_src -> zlib_src route is separately built and exercised by libpng_src_roundtrip_test without Core. Both routes are intentional current build policies.

OpenColorIO's SystemMonitors implementation remains headless; monitor enumeration and ICC discovery are not provided. UI consumers use the canonical OpenColorIO case. GUI tests that construct Ctrl objects must use GUI_APP_MAIN even when no window is opened.

Run tools/validate.ps1 for Debug/Release validation. It records separate build, stdout and stderr logs under out/validation and requires successful summaries and clean exits.
