# ImagingColor

`ImagingColor` is the backend-neutral colour-processing layer for `Upp::Imaging`.
Its public API uses only `ImagingCore` types and simple U++ value types. OpenColorIO
2.5.2 is a private implementation detail and no `OCIO::*` type appears in the
public header.

The initial implementation supports:

- built-in and file-based OCIO configurations;
- backend-neutral configuration inspection (colour spaces, displays, looks and defaults);
- colour-space transforms by source and destination colour-space name;
- display/view transforms by source colour space, display and view;
- RGB and RGBA images;
- named `MultiChannel` images containing distinct `R`, `G` and `B` channels;
- UInt8, UInt16, Float16 and Float32 sample storage;
- transactional output: a failed transform leaves the caller's previous output unchanged;
- exact preservation of alpha and every non-RGB channel;
- preservation of data windows, sample type, channel layout, channel names and metadata.

Integer samples are normalized to `[0, 1]` for OCIO processing and clamped back
to their original integer range. Float16 and Float32 retain floating-point
semantics. Float16 conversion is performed privately by ImagingColor so the
package dependency boundary remains exactly `ImagingCore + OpenColorIO`.

Gray and GrayAlpha inputs are intentionally rejected because applying an RGB
colour transform and collapsing it back to a single luminance channel would be
an implicit policy decision. MultiChannel images without an unambiguous named
RGB triplet are also rejected.

Stable diagnostic codes used by this package include:

- `IMGCOLOR_CONFIG` — configuration discovery/load/validation;
- `IMGCOLOR_SELECTION` — missing or invalid transform selections;
- `IMGCOLOR_CHANNELS` — unsupported or ambiguous channel layout;
- `IMGCOLOR_SAMPLE` — unsupported sample storage;
- `IMGCOLOR_PROCESSOR` — OCIO processor construction;
- `IMGCOLOR_PIXELS` — pixel conversion or OCIO application failure.
