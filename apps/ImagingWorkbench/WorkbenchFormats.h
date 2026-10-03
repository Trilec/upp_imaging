#ifndef _ImagingWorkbench_WorkbenchFormats_h_
#define _ImagingWorkbench_WorkbenchFormats_h_

namespace Upp {
struct WorkbenchSaveFormat { const char* name; const char* extension; const char* description; };
inline const WorkbenchSaveFormat* WorkbenchSaveFormats(int& count)
{
    static const WorkbenchSaveFormat formats[] = {
        {"EXR", ".exr", "EXR — original channels / HDR"},
        {"PNG", ".png", "PNG — selected RGB/RGBA"},
        {"JPEG", ".jpg", "JPEG — selected RGB, 8-bit lossy"},
        {"JXL", ".jxl", "JPEG XL — selected RGB/RGBA"},
        {"TIFF", ".tif", "TIFF — selected RGB/RGBA"},
        {"WEBP", ".webp", "WebP — selected RGB/RGBA, 8-bit"},
        {"HDR", ".hdr", "Radiance HDR — selected RGB / HDR"},
        {"DPX", ".dpx", "DPX — selected RGB, 16-bit"}
    };
    count = sizeof(formats) / sizeof(formats[0]);
    return formats;
}
inline String WorkbenchFormatExtension(const String& name)
{
    int count; auto formats = WorkbenchSaveFormats(count);
    for(int i = 0; i < count; ++i)
        if(ToUpper(name) == formats[i].name) return formats[i].extension;
    return String();
}
inline void AddWorkbenchOpenFormats(FileSel& selector)
{
    selector.Type("All supported images and H.264 MP4/MOV", "*.mp4;*.mov;*.jpg;*.jpeg;*.exr;*.png;*.jxl;*.hdr;*.rgbe;*.dpx;*.cin;*.webp;*.avif;*.heic;*.heif;*.heics;*.hif;*.tif;*.tiff;*.dng;*.cr2;*.cr3;*.nef;*.arw;*.raf;*.rw2;*.orf;*.pef;*.sr2;*.x3f");
    selector.Type("JPEG (*.jpg, *.jpeg)", "*.jpg;*.jpeg");
    selector.Type("PNG (*.png)", "*.png");
    selector.Type("JPEG XL (*.jxl)", "*.jxl");
    selector.Type("OpenEXR (*.exr)", "*.exr");
    selector.Type("Radiance HDR (*.hdr, *.rgbe)", "*.hdr;*.rgbe");
    selector.Type("DPX / Cineon (*.dpx, *.cin)", "*.dpx;*.cin");
    selector.Type("TIFF (*.tif, *.tiff)", "*.tif;*.tiff");
    selector.Type("WebP (*.webp)", "*.webp");
    selector.Type("AVIF / HEIF (*.avif, *.heic, *.heif)", "*.avif;*.heic;*.heif;*.heics;*.hif");
    selector.Type("Camera RAW / DNG", "*.dng;*.cr2;*.cr3;*.nef;*.arw;*.raf;*.rw2;*.orf;*.pef;*.sr2;*.x3f");
    selector.Type("MP4 video — H.264 only (*.mp4)", "*.mp4");
    selector.Type("MOV video — H.264 only (*.mov)", "*.mov");
    selector.Type("All files (other camera RAW)", "*.*");
}
}
#endif
