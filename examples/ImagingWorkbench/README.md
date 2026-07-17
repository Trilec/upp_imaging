# ImagingWorkbench

Small interactive EXR and PNG workbench built on the stable `oiio` and
`opencolorio` application packages.

Build with:

```text
E:\upp-18468\umk.exe GitHubOut ImagingWorkbench CLANGx64 -br out\ImagingWorkbench.exe
```

The MVP supports image inspection, `.cube` input/output LUTs, rectangle and
ellipse layers, and EXR/PNG save and reopen validation. Multipart and deep EXR
editing, painting, undo, and drag-and-drop layer ordering are intentionally
deferred.
