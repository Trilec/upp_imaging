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
- Fit display
- exact full-resolution float probing
- bounded preview proxies for rendering

Known limits:
- no OCIO display pipeline yet
- no histogram or analysis tools
- no wheel zoom or panning
- no JPEG or TIFF support
- multipart EXR remains metadata-only for later subimages

Next milestones:
- real OCIO configuration and display processing
- analysis functionality
