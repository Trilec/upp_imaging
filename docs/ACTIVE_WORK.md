# Active Work

Recovery authority for current work only. Remote `main` is authoritative. For exact acceptance order and expected totals, read `docs/WINDOWS_ACCEPTANCE.md`.

## BASE

- Last Windows-validated baseline before the current lifecycle repair: `4f86dee683f372e887669ac31a88a155e72aa2ed`.
- That checkpoint fixed the OpenColorIO quoted-header collision, the libheif/libde265 static ABI boundary and LibRaw Winsock ownership.
- Windows evidence at that baseline:
  - `openimageio_io_test` Debug: build/link clean, **21/0**, normal exit.
  - `imaging_io_test` Debug: **79/0** summary, then reproducible exit-time access violation `0xC0000005`.
- Still-image Debug acceptance therefore stops at `imaging_io_test`; Release, `plugin_exr_test` and FFmpeg remain pending.

## TASK

`IMG-FIX-009` — make OpenImageIO shutdown deterministic before U++ `CONSOLE_APP_MAIN` enters `AppExit__()`.

Root-cause basis:

- U++ `CONSOLE_APP_MAIN` runs the app body, then calls `AppExit__()`.
- U++ `AppExit__()` calls `Thread::ShutdownThreads()` immediately.
- Pinned OpenImageIO 3.1.15.0 exposes `OIIO::shutdown()`, implemented as `default_thread_pool_shutdown()`; upstream documents that the application should call this immediately before exit when the pool is no longer needed.
- The failing test completes all 79 assertions before the access violation, so the repair is bounded to application/backend lifetime rather than test behavior or image-format logic.

## TOUCHED

Implementation slice:

- `OpenImageIO/OIIO.h`
- `OpenImageIO/OIIO.cpp`
- `ImagingIO/ImagingIO.h`
- `ImagingIO/Lifecycle.cpp` (new)
- `ImagingIO/ImagingIO.upp`
- `imaging_io_test/main.cpp`
- `docs/ACTIVE_WORK.md`

## IMPLEMENTATION

- Added `UppImaging::ShutdownOpenImageIO()` as the direct backend shutdown boundary; it calls `OIIO::shutdown()` once.
- Added backend-neutral `Upp::Imaging::ImageIOApplicationScope`.
  - constructor initializes static OpenImageIO registration;
  - destructor shuts OpenImageIO down;
  - copy/assignment disabled.
- `ImagingIO/Lifecycle.cpp` owns the backend-specific implementation so `ImagingIO.h` does not expose OIIO types.
- `imaging_io_test` constructs `ImageIOApplicationScope` as its first local object.
  - all subsequently-created image/test objects therefore destruct first;
  - the scope destructor then calls OpenImageIO shutdown;
  - only after the app body returns does U++ call `AppExit__()` and `Thread::ShutdownThreads()`.
- No pinned upstream source, plugin registration behavior, format policy, assertion, expected count or image API was changed.

## STATUS

**IMPLEMENTATION COMPLETE — PLATFORM VALIDATION PENDING**

The source repair is published on `main`. Windows execution is not available in the supervisor environment, so Gary must validate the exact current-main SHA.

## PUBLISHED

- Lifecycle source checkpoint: `3c1c7e3662cd13fca6c073fd631c1b4edd43e412` — `Shut down OpenImageIO before U++ app exit`.
- Current `main`: this recovery-log follow-up commit; verify with `git rev-parse HEAD` before validation.

## VALIDATION REQUIRED

On current `main`, first confirm `3c1c7e3662cd13fca6c073fd631c1b4edd43e412` is an ancestor of HEAD. Then use CLANGx64 Debug / Debug_Full / Noblitz:

1. Build/run `openimageio_io_test`; require **21/0** and normal exit.
2. Build/run `imaging_io_test` at least three times; every run must report **79/0** and process exit **0** with no `0xC0000005`.
3. If green, continue the remaining still-image Debug targets in the exact `docs/WINDOWS_ACCEPTANCE.md` order and stop at the first substantive failure.
4. Do not begin Release until the full still-image Debug lane is green.

## STOP CONDITIONS

Return to supervisor on:

- compile/link regression;
- any non-zero or abnormal process exit despite a green summary;
- changed test totals;
- another runtime crash/hang;
- source-manifest/dependency/API/multifile defect.

Do not weaken tests, add forced process termination, edit pinned upstream source or hide shutdown faults.

## NEXT ACTION

1. Gary: fetch current `main`, verify the lifecycle checkpoint is an ancestor, then run the validation sequence above.
2. If `imaging_io_test` exits cleanly, resume fail-fast Windows acceptance from the remaining Debug lane.
3. Return the first substantive failure with exact SHA/configuration/stage/evidence; make no architecture changes.
