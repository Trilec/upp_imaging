# Active Work

Recovery authority for work currently in flight. After fetching `main`, read this file before using chat history or starting validation. `docs/WINDOWS_ACCEPTANCE.md` is the self-contained validator contract and remains authoritative for gate order and expected totals.

## BASE

- HEIF lifecycle repair SHA: `5ef0f3e70df06e0d6e5e5263df213392a6223041`.
- Repair was built with U++ `CLANGx64`, Debug, `DEBUG_FULL`, Noblitz (`umk -a`).
- Installed U++ was not modified. `E:/upp-18468/uppsrc/Core/Mt.h` retained SHA256 `5A83CA7EAE38A41811A7A1E7AFFDA3BC4C3D2070CE120EEF3E05C71974C99524` during diagnosis.

## TASK

Continue the complete Windows acceptance matrix from Phase A Debug on the exact published repository state. The prior HEAPDBG shutdown blocker is repaired at the repository-owned libheif integration boundary.

## TOUCHED

- `OpenImageIO/OIIO.cpp` - initializes the statically linked OIIO HEIF plugin lifecycle once and registers repository-owned process-exit cleanup.
- `openimageio_plugin_heif/RegisterHEIF.cpp` and `RegisterHEIF.h` - expose the OIIO/libheif initialization pair and call `heif_deinit()` at shutdown.
- `imaging_io_test/main.cpp` - removes the temporary `VppLog()` HEAPDBG prewarm diagnostic.
- `heif_oiio_test/main.cpp` - uses OIIO 3.1 format-name factory semantics and runs error-producing checks on joined worker threads so upstream thread-local error storage is released before U++ HEAPDBG teardown.

## STATUS

- Root cause: libheif's built-in libde265 registration called `de265_init()` before main and allocated the 60-byte `pps_scan_cache`; no matching `heif_deinit()` reached `de265_free()` before U++ leak diagnostics.
- Exact allocation path: `pps_scan_cache_init()` -> `de265_init()` -> `libde265_init_plugin()` -> libheif built-in decoder registration.
- Repair: pair OIIO's existing `oiio_heif_init()` with `heif_deinit()` through the repository integration package and an `atexit` callback registered during normal application initialization.
- Separate focused-test issue: OIIO 3.1 treats `ImageInput::create("probe.avif")` as a filename to validate/open, not a static factory-only query. OIIO also retains cleared global errors in main-thread TLS until thread exit, which occurs after U++ HEAPDBG's static destructor under this MinGW runtime. The test now queries the `heif` factory key and performs deliberate error checks on joined threads; expected totals are unchanged.
- No pinned upstream source, feature policy, package ownership, or installed U++ file was changed.

## PUBLISHED

- `5ef0f3e70df06e0d6e5e5263df213392a6223041` - HEIF lifecycle and focused-gate repair.
- `527f2d2a92691622dab13ff4df6c58a396577183` - preceding cleanup checkpoint.

## VALIDATION

- `openimageio_io_test` Debug `DEBUG_FULL` Noblitz: `21/0`, exit `0`.
- `imaging_io_test` Debug `DEBUG_FULL` Noblitz: five consecutive `79/0`, exit `0`, with `OIIO_USTRING_CLEANUP` unset and no HEAPDBG/access-violation output.
- `heif_oiio_test` Debug `DEBUG_FULL` Noblitz: three consecutive `11/0`, exit `0`, with no HEAPDBG/access-violation output.
- Diagnostic cdb breakpoints confirmed process-exit cleanup reaches both `heif_deinit()` and `de265_free()` before `Upp::MemoryDumpLeaks()`.

## NEXT ACTION

- Commit and publish this recovery ledger, then begin exact-SHA acceptance at Phase A Debug target 1.
- Run all sixteen still-image Debug targets in order, then all sixteen Release targets, `plugin_exr_test` Debug/Release, and the FFmpeg Debug/Release plus repeatability lanes from `docs/WINDOWS_ACCEPTANCE.md`.
- Stop on the first substantive failure, publish any required repair as a new checkpoint, and restart exact-SHA acceptance from the authoritative order.
