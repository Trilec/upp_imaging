# U++ image integration — Windows Debug evidence

Source base: `d944562640194668377f8832af0c762f34aac3dc`. These runs compiled
the uncommitted image integration changes in the worktree; the base SHA alone
does not identify those compiled changes. The coherent checkpoint containing
this record and the source establishes their committed relationship.

Only Debug was built/run for this block, as requested. No Release,
Linux/macOS, sanitizer or fuzz validation is claimed.

| Target | Checks | Exit | Run job | Artifact SHA-256 |
| --- | ---: | ---: | --- | --- |
| imaging_raster_test | 57/0 | 0 | exec-6096BC5891A1C83A89AA23B04DA60050 | `a28ae956471f6076ff01c48fe902bc920c466161185831dd37dc2ea0ff1f0834` |
| plugin_exr_test | 22/0 | 0 | exec-2AC3754BE239694B7911709108902412 | `d116aecadbbbc81e53a00cee2d3ae9d49c0855ff9b1ce9040367f01ce137f769` |
| imaging_workbench_ocio_test | 155/0 | 0 | exec-2524351C867B2E93B3DD96AFD1CAA15B | `85fc96e836b25bda87bacc8e6ceeda858edd1ad8e3c366e8ecf0a5bc65db4fcf` |

234 checks and three clean process exits. The expanded format test generated
EXR, PNG, JXL, HDR, DPX, WebP, TIFF, Cineon and linear DNG; used retained
upstream AVIF/HEIC; checked registered loading, pixel values, straight alpha,
multipart rejection and encoded/decoded limits. Workbench additionally passed
positive Cineon and DNG loading while retaining its previous 151 checks.
The EXR-specific contract remained 22/0. Manual samples were retained at
`build/windows-x64/samples/`.

An initial test mistakenly used libheif's multi-image example as a single-image
fixture. Native inspection verified its extra image; the corrected test asserts
the documented rejection and separately validates the single-image rainbow HEIC.
The final format run had no stderr. Existing ImagingIO DLL-import link warnings
remain in the EXR/Workbench builds; they did not prevent the build or clean exit.

Klick text preview/apply/readback completed with verified receipts.
Build/output execution is working. Retained job storage still reports
`JOB_DIRECTORY_OWNER_ACL_REQUIRED`; the immediate run results above are the
evidence, not a claim that job history recovery works.
