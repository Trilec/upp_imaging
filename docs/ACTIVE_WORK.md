# Active Work

Recovery authority for work currently in flight. After fetching `main`, read this file before using chat history or starting validation. `docs/WINDOWS_ACCEPTANCE.md` is the self-contained validator contract and remains authoritative for gate order and expected totals.

## BASE

- Current exact main SHA before this cleanup: `344ef1ee726ce51aa60452c38d940951cc7dcb52`.
- Last meaningful Windows evidence: `openimageio_io_test` Debug `21/0` clean exit; `imaging_io_test` body `79/0`; `imaging_io_test` abnormal exit `0xC0000005` proven to be a U++ Debug HEAPDBG static-exit framework defect.

## TASK

Repository hygiene and retired-package cleanup, plus removal of the disproven OpenImageIO application-shutdown surface. The U++ Debug HEAPDBG static-exit `0xC0000005` remains a separate known framework blocker; it is not addressed by this cleanup.

## TOUCHED

- `ImagingIO/ImagingIO.h` — removed `ImageIOApplicationScope`.
- `ImagingIO/ImagingIO.upp` — removed `Lifecycle.cpp` from the source manifest.
- `ImagingIO/Lifecycle.cpp` — deleted.
- `imaging_io_test/main.cpp` — removed the `ImageIOApplicationScope` instance.
- `OpenImageIO/OIIO.h` — removed `ShutdownOpenImageIO()`.
- `OpenImageIO/OIIO.cpp` — removed the `ShutdownOpenImageIO()` wrapper and its `OIIO::shutdown()` call.
- `openexr_core_src_probe/` — deleted (retired probe).
- `openexr_src_probe/` — deleted (retired probe).
- `openimageio_src_probe/` — deleted (retired probe).
- `openimageio_util_src_probe/` — deleted (retired probe).
- `docs/ACTIVE_WORK.md` — rewritten to this concise recovery record.
- `docs/toolchain_matrix.md`, `docs/openexr_branch_plan.md`, `docs/openexr_preflight.md`, `openexr_src/README.md`, `openimageio_src/README.md`, `openimageio_util_src/README.md` — removed references to deleted probe packages.

## STATUS

- Cleanup state: retired probe packages removed; disproven lifecycle surface removed; documentation references updated.
- Windows acceptance remains stopped at `imaging_io_test` Debug: body `79/0`, abnormal exit `0xC0000005`.

## PUBLISHED

- `692a2616492b72704b8a22aaa7f0fe44978ea65d` — cleanup commit: removed retired imaging probes and stale lifecycle code.

## VALIDATION

- `openimageio_io_test` Debug `21/0` clean exit (retained after cleanup).
- `imaging_io_test` body `79/0`; abnormal exit `0xC0000005` is the independently proven U++ Debug HEAPDBG static-exit framework defect (`MemDiagCls::~MemDiagCls` -> `MemoryDumpLeaks` -> `VppLog` -> `LogOut::Line` -> late `StaticPrimitive_<Mutex>::Get` -> `0xC0000005`).
- Removed-package graph scan: no remaining `uses` or include reference points at a deleted package.

## NEXT ACTION

- Resume framework-level diagnosis/fix for the U++ Debug HEAPDBG static-exit shutdown defect, then continue the accumulated Windows acceptance matrix (still-image Debug, Release, `plugin_exr_test`, FFmpeg and repeatability).
