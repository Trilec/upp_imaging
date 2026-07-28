# ImagingWorkbench

Small interactive EXR and PNG workbench built on the stable `oiio` and
`opencolorio` application packages.

Build with:

```text
E:\upp-18468\umk.exe GitHubOut ImagingWorkbench CLANGx64 -br out\ImagingWorkbench.exe
```

Current scope:
- EXR and PNG loading
- EXR and PNG saving
- channel and pass selection
- RGB, R, G, B, and A display modes
- exposure and gamma inspection controls
- OCIO config and display preview
- Fit display with cursor-centred wheel zoom and middle-button pan
- zoom range is clamped from 5% to 3200% of Fit
- exact full-resolution source probing through the current view transform
- bounded preview proxies for rendering
- source-referred histogram analysis on the Analysis tab

Navigation behavior:
- new PNG and EXR loads start in Fit mode
- wheel zoom enters Manual mode and keeps the source point beneath the cursor stable until edge clamping is required
- a middle-button drag enters Manual mode; when the image is smaller than the viewport, pan bounds keep it centred
- releasing the middle button or receiving cancellation ends the pan gesture and releases capture safely
- Fit restores the centred fitted view
- OCIO, Look, LUT, exposure, gamma, and channel-display changes preserve the current navigation state
- pass changes preserve a valid manual source-relative centre when dimensions allow

Histogram behavior:
- the histogram is computed from the selected cached preview proxy
- the histogram is source-referred: it is calculated before exposure, gamma, OCIO, Look and LUT are applied
- the visible range is 0.0 to 1.0 with 256 bins by default
- exactly 0.0 falls into the first bin
- exactly 1.0 falls into the final bin
- values strictly below 0.0 are counted as below-range and included in the per-channel minimum/mean
- values strictly above 1.0 are counted as above-range and included in the per-channel maximum/mean
- NaN and positive or negative infinity are counted as non-finite and do not affect minimum, maximum or mean
- each channel shows its own name, classified sample count, minimum, maximum, mean, and non-zero below-range / above-range / non-finite counts
- channels with no finite values show min/max/mean as —
- colour swatches in the stats area are clickable: toggle individual channels, or click "All" to restore all overlaid traces
- inactive channels are dimmed and excluded from the graph normalization
- the histogram is recomputed only when the source, subimage, pass, group or proxy pixels change
- the histogram is not recomputed for exposure, gamma, OCIO, Look, LUT, zoom, pan, Fit, canvas resize, probe movement or channel-toggle clicks

Histogram probe marker:
- a thin vertical marker is drawn for each visible and available channel at the cursor's source value
- the marker uses full-resolution source probe values (not the bounded proxy)
- the marker maps the source value into the 0.0–1.0 histogram range
- 0.0 draws at the left edge, 1.0 draws at the right edge, intermediate values interpolate linearly
- values below 0.0 clamp to the left edge with a small left-pointing tick
- values above 1.0 clamp to the right edge with a small right-pointing tick
- in-range values are marked with a small downward triangle at the top of the graph
- NaN and positive or negative infinity do not draw a normal marker; the stats area shows a compact "NaN/Inf" probe note
- hidden histogram channels hide their markers
- the marker follows the same selected pass and channel mapping as the histogram
- exposure, gamma and OCIO do not move the source-referred markers
- markers clear when the cursor leaves the image, when a new image is loaded, when the pass or group changes, or when the image is cleared

Known limits:
- histogram graph uses linear scaling and filled polygon traces
- no logarithmic graph scale
- no waveform or vectorscope
- no JPEG or TIFF support in ImagingWorkbench
- multipart EXR remains metadata-only for later subimages
- Fit resets on new image load; manual navigation is preserved across preview changes when dimensions allow

Next milestones:
- histogram graph polish
