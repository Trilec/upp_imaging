# ImagingPluginDemo — U++ Imaging 1.0

This focused example loads JPEG, PNG, JXL, EXR, Radiance HDR or DPX through its
explicit U++ raster plugin into the production UiMediaCard. Select the format,
Load image, choose Contain/Cover and copy the matching complete C++ program.
The preview and generator use the same path, format and fit settings.

Add the selected plugin and Ui to .upp uses. Member/local controls own their
lifetimes; UiMediaCard displays a U++ Image and does not itself decode files.
The generated code opens FileIn, uses the concrete raster reader, calls
GetImage and SetImage, and owns the window/card through Run(). Preview is RGBA8;
use ImagingIO for full-fidelity HDR. See docs/FORMAT_QUICKSTART.md.

The full media-card style/PropertyEditor example remains
upp_Ui/examples/UiMediaCardDemo; this demo demonstrates the imaging provider.
It has no audio or video playback. Input policy remains trusted local files.

Build with GitHubOut / ImagingPluginDemo / CLANGx64 +GUI. Generated output can
be captured with --emit <main.cpp> <format-index 0..5> <local-image> <contain|cover>.
Compile that file unchanged with Ui and the selected plugin, then run --check
to verify decoding and public-API setup without opening the interactive window.
