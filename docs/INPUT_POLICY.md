# Windows trusted-local input policy

On 2 October 2026 Curt selected **trusted local inputs with enforced
restrictions** as the delivery security scope. The supported contract is
stable local files from a known source, including trusted OCIO configs and
their referenced LUTs. This is not a hostile-file service or decoder sandbox.

The required selection is JPEG, PNG, JPEG XL, EXR, DPX and Radiance HDR, plus
H.264 video in MP4/MOV. EXR and floating-point/HDR paths retain scene-referred
values; the display preview applies its view conversion. JPEG is an 8-bit
Gray/RGB, lossy format. JPEG XR is a different format and is deferred. Audio,
other video codecs, arbitrary QuickTime codecs and HDR video are not promised.

## Enforced boundaries

| Entry point / resource | Restriction |
| --- | --- |
| Workbench image and ImagingIO file loading | Existing local regular file, nonempty, at most 64 MiB; reject URI/UNC paths and canonical UNC destinations before opening a decoder |
| U++ ImagingRaster stream loading | At most 64 MiB encoded input; bounded decoded/preview allocations through ImagingRasterLimits |
| Initialized OIIO readers | At most 8192 per axis, 64 channels, 256 MiB declared decoded image size |
| TIFF native allocations | At most 256 MiB per allocation and 512 MiB total per handle, for filename and IOProxy paths; callers may lower these limits |
| OCIO file configs | Existing local regular file, nonempty, at most 4 MiB before YAML parsing |
| Workbench directly selected LUT | Existing local regular file, nonempty, at most 16 MiB before creating the transform |
| OCIO XML formats | CTF/CLF, CC/CCC/CDL and Iridas look are unavailable; registry excludes them and direct XML parser construction throws before Expat parsing |
| ImagingVideo | Local H.264 MP4/MOV only; 256 MiB input, 8192 per axis, 8 Mi pixels; bounded frame allocations, cooperative work/deadline checks and explicit seeking policy |

The installed defaults constrain supported application entry points. Raw
third-party APIs and host changes to OIIO attributes are outside this
contract. Config-referenced resources are trusted by the caller; their sizes
and paths are not all individually checked by the application. Hosts using
the packages must keep that trust boundary and apply their own input policy.

Native metadata, decoder scratch, JXL boxes and ICC processing can allocate
outside application buffers. File size and declared image limits do not cap
whole-process memory or worst-case CPU. Cooperative video deadlines cannot
interrupt an in-progress native decode. Canonical-path preflight is not a
file-race guarantee. Stable, trusted input and these residual native-library
risks are the accepted scope, not proof that malformed files are safe.

The known unresolved Expat CPU issue is addressed by disabling OCIO XML
input, not by claiming a library fix. Standalone Expat remains available to
other native consumers and carries its documented upstream risk.

Dependency dispositions are recorded in THIRD_PARTY_SECURITY.md and
DEPENDENCY_REVIEW_20261002.md. Release publication and Linux/macOS,
sanitizer and fuzz validation remain separate, uncompleted work.
