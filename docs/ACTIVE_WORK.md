# Active Work

Recovery authority for work currently in flight. After fetching `main`, read this file before using chat history or starting validation. `docs/WINDOWS_ACCEPTANCE.md` is the self-contained validator contract and remains authoritative for gate order and expected totals.

## BASE

- Acceptance started at `d4016ee168b38c66af5db4bdd5cc68f9ff541c0e` (`origin/main` at refresh).
- U++ installation: `E:/upp-18468`; builder: `E:/upp-18468/umk.exe`; method: `GitHubOut`, `CLANGx64`, Debug `DEBUG_FULL` Noblitz and Release.
- Installed U++ and pinned upstream sources were not modified.

## TASK

Complete the bounded Windows acceptance matrix. The sixteen still-image gates are green in Debug and Release. The opt-in EXR raster gate is repaired and green; FFmpeg Debug/Release and repeatability remain.

## TOUCHED

- `plugin/exr/exr.cpp` - selects the registered OpenEXR reader directly for already magic-validated input and consumes reader diagnostics on open failure.
- `plugin_exr_test/main.cpp` - runs deliberate malformed/truncated input checks on a joined worker so OpenImageIO thread-local diagnostics are destroyed before U++ HEAPDBG teardown.
- `ffmpeg_headers/generated/config.h` and `config_components.h` - explicitly disable every optional capability/component referenced by the imported scalar source closure without enabling new FFmpeg features.
- `ffmpeg_headers_test/main.cpp` - excludes bare token-paste prefixes such as `HAVE_` from the generated-macro coverage audit.
- `ffmpeg_swscale_src/chroma_pos_compat.c` and `import.ext` - materialize the one legacy chroma-position helper that pinned FFmpeg hides when unstable swscale backends are disabled.

## STATUS

- The previously published ImagingIO root shutdown defect remains closed: libheif/libde265 cleanup runs before U++ leak diagnostics, and the main ImagingIO gate completed five consecutive `79/0`, exit `0` runs without the temporary VppLog prewarm.
- All sixteen still-image gates passed in required order in both Debug and Release with exact totals and normal exits.
- `plugin_exr_test` initially reported `22/0` and then failed during HEAPDBG shutdown. A temporary prewarmed diagnostic log identified live OpenImageIO error strings from the intentional truncated EXR probe. The diagnostic was removed.
- The production adapter now avoids unrelated fallback-reader probing, drains its expected open error, and the focused test confines unavoidable OIIO error-storage lifetime to a joined worker.
- The first FFmpeg Debug gate initially stopped at `7/1`: the generated-config audit found 108 real identifiers left implicitly false plus a bare token-paste `HAVE_` prefix. The generated-equivalent headers now state every disabled value explicitly, preserving the existing scalar/disabled behavior, and the audit only accepts prefixes followed by an identifier suffix.
- The restarted FFmpeg Debug lane passed headers, avutil, avcodec and avformat, then stopped while compiling swscale. Four CPU capabilities formed through token concatenation are now explicitly `0`. The next link exposed pinned FFmpeg's unconditional stable-graph reference to `ff_sws_chroma_pos()` even though its definition is inside `CONFIG_UNSTABLE`; a guarded local materializer supplies that exact helper without enabling unstable backends or changing the upstream submodule.
- No expected total, feature policy, installed U++ file, or pinned upstream file changed.

## PUBLISHED

- This checkpoint: scalar swscale configuration and pinned stable-graph closure.
- `eb56539` - FFmpeg generated-configuration coverage closure.
- `06e195d` - EXR negative-input teardown and direct-reader repair.
- `d4016ee168b38c66af5db4bdd5cc68f9ff541c0e` - preceding published acceptance ledger.
- `93c4bc38e1f0d2ac505e06d84d35894a8768709f` - ImagingIO integration-gate teardown repair.
- `626282d90efe40ccc9517bec13245e5bcab15d16` - focused OIIO gate teardown and pinned-API repair.
- `5ef0f3e70df06e0d6e5e5263df213392a6223041` - HEIF lifecycle and focused-gate repair.

## VALIDATION

- Still image Debug: all 16 required gates passed in order with exact totals; all exited `0`.
- Still image Release: all 16 required gates passed in order with exact totals; all exited `0`.
- `plugin_exr_test` Debug after repair: `22/0`, exit `0`, plus three additional `22/0`, exit `0` runs; no HEAPDBG/access violation or pending OIIO error.
- `plugin_exr_test` Release after repair: `22/0`, exit `0`.
- FFmpeg pin verified locally: signed tag `n9.0.1`, exact submodule commit `bf1b838f2ab88b4f8fd83443325c782ea0e0f7fa`.
- `ffmpeg_headers_test` focused Debug regression: config audit `sources=237`, `scanned=608`, `referenced=260`, `generated=393`, `missing=0`; `8/0`, exit `0`.
- Restarted FFmpeg Debug lane before the swscale repair: headers `8/0`, avutil `13/0`, avcodec `12/0`, avformat `14/0`, all exit `0`; stopped at the swscale build and did not run first-frame.
- `ffmpeg_swscale_test` focused Debug regression after repair: `13/0`, exit `0`.

## NEXT ACTION

- Publish this swscale checkpoint, then restart the six-gate FFmpeg Debug lane at `ffmpeg_headers_test`.
- Continue with all six Release gates and five additional first-frame runs per configuration.
- Stop on the first substantive failure, repair the root cause, publish a coherent checkpoint, and restart the affected accumulated lane.
