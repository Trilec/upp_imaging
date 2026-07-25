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

Navigation behavior:
- new PNG and EXR loads start in Fit mode
- wheel zoom enters Manual mode and keeps the source point beneath the cursor stable until edge clamping is required
- a middle-button drag enters Manual mode; when the image is smaller than the viewport, pan bounds keep it centred
- releasing the middle button or receiving cancellation ends the pan gesture and releases capture safely
- Fit restores the centred fitted view
- OCIO, Look, LUT, exposure, gamma, and channel-display changes preserve the current navigation state
- pass changes preserve a valid manual source-relative centre when dimensions allow

Known limits:
- no histogram or analysis tools
- no JPEG or TIFF support
- multipart EXR remains metadata-only for later subimages
- Fit resets on new image load; manual navigation is preserved across preview changes when dimensions allow

Next milestones:
- analysis functionality
