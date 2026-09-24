# Toolchain validation

Windows x64 / U++ CLANGx64 is the validated target. The exact current test set and results are recorded in [Windows acceptance](WINDOWS_ACCEPTANCE.md).

Debug and Release are both required. The standalone PNG/zlib source-provider route is retained separately from the Windows/Core provider route. ImagingWorkbench supplies GUI build coverage, and its OCIO test exercises the application without opening a window.

Other operating systems and toolchains remain unvalidated by this Windows migration. Pinned source ownership and explicit manifests are retained for future portability work.
