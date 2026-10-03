# Supported formats and Workbench

Version 1 supports Windows x64 and trusted local files. A filename extension
selects a family; it does not guarantee that every encoding in that family is supported.

| Image family | Workbench / ImagingIO input | Workbench export |
| --- | --- | --- |
| JPEG (.jpg/.jpeg) | 8-bit Gray/RGB | 8-bit lossy RGB |
| PNG | RGB/RGBA and supported PNG layouts | Selected RGB/RGBA |
| JPEG XL (.jxl) | Supported JXL images | Selected RGB/RGBA |
| OpenEXR (.exr) | Supported ordinary image layouts | Original source channels / HDR |
| Radiance HDR (.hdr/.rgbe) | RGBE HDR | Selected HDR RGB |
| DPX (.dpx) | Supported DPX layouts | Selected 16-bit RGB |
| Cineon (.cin) | Supported Cineon images | Input only |
| TIFF (.tif/.tiff) | Supported TIFF layouts/codecs | Selected RGB/RGBA |
| WebP | Supported WebP images | Selected 8-bit RGB/RGBA |
| AVIF / HEIF / HEIC | Supported decoder paths | Input only |
| Camera RAW / DNG | LibRaw-supported subset; synthetic linear DNG fixture coverage | Input only |

JPEG, PNG, JPEG XL, EXR, HDR and DPX are the core selection demonstrated by
ImagingPluginDemo. Additional opt-in raster packages exist for TIFF, WebP,
Cineon, RAW and HEIF; see [U++ usage](FORMAT_QUICKSTART.md).
Not every camera RAW variant, multipart/deep EXR or TIFF compression/layout is
promised. Package READMEs and ImagingIO's FormatPolicy define exact API subsets.

## Saving and HDR

EXR saves the original channels. Other Workbench choices export the selected
source RGB/RGBA group at full resolution. Exposure, display gamma and OCIO
preview transforms are not baked into the saved image. JPEG, HDR and DPX reject
alpha explicitly. Quantized formats cannot preserve arbitrary floating-point HDR
values. Use ImagingIO for full-fidelity typed data; raster previews are RGBA8.
ImagingIO saving uses its own sample/channel policy and does not implicitly
perform every conversion offered by the Workbench.

## Video

MP4 and MOV support **H.264 video only**, without audio. Workbench decodes frames
as needed and keeps one upcoming frame queued; it does not preload the whole
movie. Play/Pause, Next and Restart are in Layers (Space, Right and Home).
The reusable ImagingVideo reader exposes timestamps, EOF/error status and seeking.
Saving a video view exports the current still frame, not a movie.

Other codecs, arbitrary QuickTime encodings, audio and HDR video are unsupported
in version 1. JPEG XR is distinct from EXR and is deferred.

## Using the Workbench

Load selects an explicit image or video family. Layers selects channels/passes.
Fit, mouse-wheel zoom and middle-button pan control the view; exposure/gamma
and OCIO affect the preview. Analysis displays statistics and histograms.
Help / F1 explains supported formats, exports and restrictions.
OCIO YAML configurations, non-XML LUTs and programmatic transforms are supported;
OCIO XML formats are disabled. See [input limits](INPUT_POLICY.md).
