# Windows imaging delivery tasks

**Complete: 8 of 8 milestones, 100%.** Scope: trusted-local Windows Debug
Workbench and reusable U++ packages, as selected by Curt on 2 October 2026.

- [x] Shared bounded U++ raster bridge and opt-in format packages, including JPEG.
- [x] Small positive fixtures for JPEG/PNG/JXL/EXR/DPX/HDR and retained extra formats.
- [x] Reusable bounded H.264 MP4/MOV reader with U++ frames, timing and seeking.
- [x] Workbench video preview, playback, stepping and seeking.
- [x] Restricted-input policy, OCIO XML disablement, TIFF allocation limits and bounded dependency dispositions.
- [x] Reconciled usage, package catalogue, security records and overall dashboard scope.
- [x] Final integrated Debug acceptance: 51 targets, 1,512 checks, 51 exits at 0.
- [x] Rebuilt identified Debug Workbench and one coherent reviewed main checkpoint.

See DELIVERY_COMPLETION_20261002.md for the source relationship, test ledger,
artifact identity and cleanup. FORMAT_QUICKSTART.md explains package use.
INPUT_POLICY.md records the approved restrictions and residual native-parser
risks. Workbench manually accepted by Curt; no repeated GUI checklist.

The Debug milestone does not include Release publication, hostile-file
isolation, Linux/macOS, sanitizer or fuzz execution. Those remain separate.
JPEG XR, OCIO XML formats, additional video codecs, audio and HDR video are
deferred; EXR and Radiance HDR image processing are included. No binary is
misrepresented as a newly verified Release payload.
