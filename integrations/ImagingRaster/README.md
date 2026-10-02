# U++ raster preview integration

ImagingRaster provides the shared bounded OpenImageIO-to-StreamRaster conversion.
Add an opt-in `plugin/imaging_<format>` package and include its header to use
`StreamRaster::LoadFileAny` or `LoadStringAny`. The existing `plugin/exr`
package uses the same implementation and retains EXRRaster.

Preview produces RGBA8, clamps finite samples to [0,1], maps non-finite samples
to zero, and preserves straight alpha until U++ raster conversion. Named RGB
channels are selected; one channel previews as gray and gray/alpha is supported.
Deep, volume, multipart and mipmapped inputs are rejected. Full-fidelity and
metadata workflows use ImagingIO instead.

Defaults: 64 MiB encoded input, 16,777,216 pixels, 16,384 pixels per dimension,
64 channels, and 256 MiB combined float samples plus RGBA storage. SetLimits
before Open can lower those budgets. Initialized OIIO also imposes 8192 per
axis, so that is the effective default axis limit. Native parser metadata and decoder working
memory are not included in that combined allocation budget. RAW also receives
the native memory limit. Readers without IOProxy (including RAW) use a bounded
temporary file, removed on all normal success/failure exits.

All format packages link the current OpenImageIO static reader family; opting
into registration does not make the underlying codec dependency graph smaller.

These budgets apply to the direct adapters. Other registered readers (including
U++ Draw's built-in PNG reader) retain their own limits; LoadFileAny is not a
policy enforcement boundary for the entire registry.
