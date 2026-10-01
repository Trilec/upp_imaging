# Windows imaging delivery tasks

Finish means a usable Windows Debug Workbench and reusable U++ packages for the advertised image formats and bounded H.264 MP4/MOV video. Workbench calls reusable libraries; decoding and conversion are shared.

- [x] Shared bounded U++ raster preview bridge and opt-in format packages.
- [x] Positive, small manual-load fixtures for every advertised image family, including Cineon and camera RAW.
- [x] Reusable bounded video reader with RGBA frames, timing and seeking.
- [x] Workbench video opening, preview, play/pause, stepping and seeking.
- [ ] Complete input/allocation policies and reachable dependency security review.
- [x] Reconcile usage, package catalogue, provenance, security and dashboard records.
- [x] Consolidated Debug checks per substantial block; complete Debug suite once at the final checkpoint.
- [x] Runnable identified Debug Workbench and coherent reviewed checkpoint.

Debug builds only, as requested on 1 October 2026. Preserve Curt's prior Workbench manual acceptance. Release publication and Linux/macOS/sanitizer/fuzz validation remain separate gates.

Starting source: d944562640194668377f8832af0c762f34aac3dc. Existing native image readers and the FFmpeg first-frame proof are retained. The starting U++ StreamRaster integration had only EXR. Klick preview/apply works with the text-edit `file` field; project permissions are sufficient.

Image block: 234 Debug checks, three exits at 0. Eleven image formats across ten reader families have
positive integration coverage; manual samples are in build/windows-x64/samples.
See IMAGING_RASTER_VALIDATION.md for exact jobs and artifact identities.

Video block: 22 reusable-reader checks and 166 combined Workbench checks passed
in Debug. MP4/MOV samples share the retained H.264 pixel contract; the MOV
fixture is a QuickTime-brand variant. The actual Debug app also builds.
See IMAGING_VIDEO_VALIDATION.md. The complete 51-target Debug block passed 1,482 checks, zero failures and
51 exits at 0. The identified Debug executable is build/ImagingWorkbench_debug.exe.
Security clearance remains pending; the reviewed checkpoint is identified by git log -1 -- docs/DELIVERY_TASKS.md.
