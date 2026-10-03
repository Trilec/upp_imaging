#ifndef _ImagingPluginDemo_UsageCode_h_
#define _ImagingPluginDemo_UsageCode_h_
#include <algorithm>
#include <Ui/Ui.h>
#include <plugin/imaging_jpeg/imaging_jpeg.h>
#include <plugin/imaging_png/imaging_png.h>
#include <plugin/imaging_jxl/imaging_jxl.h>
#include <plugin/exr/exr.h>
#include <plugin/imaging_hdr/imaging_hdr.h>
#include <plugin/imaging_dpx/imaging_dpx.h>

namespace Upp {
struct ImagingDemoFormat { const char* title; const char* package; const char* header; const char* type; const char* suffix; };
inline const ImagingDemoFormat& DemoFormat(int index)
{
    static const ImagingDemoFormat formats[] = {
        {"JPEG", "plugin/imaging_jpeg", "plugin/imaging_jpeg/imaging_jpeg.h", "ImagingJPEGRaster", "jpg"},
        {"PNG", "plugin/imaging_png", "plugin/imaging_png/imaging_png.h", "ImagingPNGRaster", "png"},
        {"JPEG XL", "plugin/imaging_jxl", "plugin/imaging_jxl/imaging_jxl.h", "ImagingJXLRaster", "jxl"},
        {"EXR", "plugin/exr", "plugin/exr/exr.h", "EXRRaster", "exr"},
        {"Radiance HDR", "plugin/imaging_hdr", "plugin/imaging_hdr/imaging_hdr.h", "ImagingHDRRaster", "hdr"},
        {"DPX", "plugin/imaging_dpx", "plugin/imaging_dpx/imaging_dpx.h", "ImagingDPXRaster", "dpx"}
    };
    return formats[std::clamp(index, 0, 5)];
}
inline Image DemoLoad(int format, const String& path)
{
    FileIn input(path);
    if(!input.IsOpen()) return Image();
    switch(format) {
    case 0: { ImagingJPEGRaster reader; return reader.Open(input) ? reader.GetImage() : Image(); }
    case 1: { ImagingPNGRaster reader; return reader.Open(input) ? reader.GetImage() : Image(); }
    case 2: { ImagingJXLRaster reader; return reader.Open(input) ? reader.GetImage() : Image(); }
    case 3: { EXRRaster reader; return reader.Open(input) ? reader.GetImage() : Image(); }
    case 4: { ImagingHDRRaster reader; return reader.Open(input) ? reader.GetImage() : Image(); }
    case 5: { ImagingDPXRaster reader; return reader.Open(input) ? reader.GetImage() : Image(); }
    }
    return Image();
}
inline String ImagingUsageCode(int format, const String& path, bool cover)
{
    const auto& f = DemoFormat(format);
    String code;
    code << "// .upp uses: Ui, " << f.package << "\n"
         << "#include <Ui/Ui.h>\n#include <" << f.header << ">\n"
         << "using namespace Upp;\n\nGUI_APP_MAIN\n{\n"
         << "    FileIn file(" << AsCString(path) << ");\n"
         << "    " << f.type << " raster;\n"
         << "    if(!file.IsOpen() || !raster.Open(file)) { SetExitCode(1); return; }\n"
         << "    Image image = raster.GetImage();\n"
         << "    if(image.IsEmpty()) { SetExitCode(1); return; }\n"
         << "    TopWindow window;\n    UiMediaCard card; // both outlive window.Run()\n"
         << "    window.Title(\"U++ Imaging plugin preview\").Sizeable().Zoomable();\n"
         << "    window.SetRect(0, 0, 800, 600);\n"
         << "    card.SetHeader(" << AsCString(f.title) << ");\n"
         << "    card.SetMediaFit(UiMediaFit::" << (cover ? "Cover" : "Contain") << ");\n"
         << "    card.SetImage(image);\n    window.Add(card.SizePos());\n"
         << "    if(!CommandLine().IsEmpty() && CommandLine()[0] == \"--check\") return;\n"
         << "    window.Run();\n}\n";
    return code;
}
}
#endif
