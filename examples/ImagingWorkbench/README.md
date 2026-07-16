# ImagingWorkbench

Small GUI workbench for loading, compositing, and saving OpenImageIO images.

It uses only `CtrlLib`, `openimageio`, and `opencolorio`; it supports EXR/PNG
open/save, generated rectangle and ellipse layers, and forward `.cube` LUT
application. Multipart/deep EXR editing, channel remapping, and background
processing remain deferred.
