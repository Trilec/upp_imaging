# Architecture

Imaging separates upstream source ownership, stable native APIs, backend-neutral
U++ contracts, opt-in display plugins and applications. This keeps decoder/build
details out of application-facing image types.

## Package boundaries

| Layer | Ownership |
| --- | --- |
| third_party source/header packages | Explicit pinned source manifests, generated configuration and static registration |
| OpenImageIO, OpenColorIO, openexr/openexr_core and codec packages | Stable direct native APIs and include/link policy |
| ImagingCore | Core-only typed image data, metadata, results and diagnostics |
| ImagingIO | Full-fidelity loading and transactional saving; private OIIO backend |
| ImagingColor | Colour transforms over ImagingCore data; private OCIO backend |
| ImagingAnalysis / ImagingDiagnostics | GUI-independent statistics, histograms, comparisons and reporting |
| Imaging | Framework umbrella |
| ImagingRaster and plugin packages | Bounded U++ StreamRaster decoding to RGBA8 preview |
| ImagingVideo | Separate bounded H.264 MP4/MOV reader returning U++ images and timestamps |
| ImagingWorkbench / ImagingPluginDemo | Interactive applications using the public/native integration paths |

Framework public headers expose Upp::Imaging types rather than OIIO/OCIO types.
Ordinary consumers depend on public packages, not `_src` implementation owners.
ImagingCore and numerical analysis stay independent of GUI controls.

## Display integration

Raster plugins are opt-in .upp dependencies. Each selects the shared ImagingRaster
bridge with an explicit format reader. UiMediaCard receives the resulting U++
Image through SetImage; the control does not own a decoder. The demo's preview
and C++ generator share format/path/fit state. RGBA8 preview is distinct from
the full-fidelity HDR data available through ImagingIO.

## Image and video ownership

Image loading preserves the caller's output on failure. Saving validates format
policy, writes a same-directory temporary payload, verifies readability and
promotes it to the destination. This is not a power-loss durability guarantee.
Workbench exports source data separately from its exposure/gamma/OCIO view.

Video remains separate from ImagingIO and the Imaging umbrella. The static
FFmpeg slice enables native H.264, MOV/MP4 demux, local-file input and scalar
RGBA conversion. Networking, encoding, audio, external codecs and hardware
acceleration are disabled. Workbench queues one upcoming frame rather than
retaining the entire movie. Native scratch/decode time remains outside a strict
whole-process cap; see [INPUT_POLICY.md](INPUT_POLICY.md).

## Native integration constraints

The Windows public zlib/libpng route cooperates with U++ Core's plugin/z to avoid
duplicate symbols. Standalone source providers retain separate test coverage.
Do not collapse these functional package boundaries.

Package-owned wrappers handle pinned OIIO error storage, HEIF plugin teardown
and OCIO registry ownership. Preserve joined-worker and static-lifetime rules;
do not prewarm tests to conceal shutdown failures. Generated native headers
describe the compiled target and feature subset, not arbitrary platforms.

See [maintainer notes](MAINTAINING.md) for layout, format additions and changes
that need validation. Source manifests remain explicit rather than recursive globs.
