#include "UsageCode.h"
using namespace Upp;

// Focused loading example; the full UiMediaCard style inspector lives in upp_Ui.
class ImagingPluginDemo : public TopWindow {
    UiTitleCard title;
    UiMediaCard card;
    UiButton open, copy, theme, help;
    DropList format, fit;
    DocEdit code;
    String path;
    void UpdateCode() {
        code.SetData(ImagingUsageCode((int)format.GetData(), path, (int)fit.GetData() == 1));
        card.SetMediaFit((int)fit.GetData() == 1 ? UiMediaFit::Cover : UiMediaFit::Contain);
    }
    void Load() {
        FileSel files;
        const auto& f = DemoFormat((int)format.GetData());
        files.Type(f.title, String("*.") + f.suffix);
        if((int)format.GetData() == 0) files.Type("JPEG (*.jpeg)", "*.jpeg");
        if(!files.ExecuteOpen("Load trusted local image")) return;
        Image image = DemoLoad((int)format.GetData(), files.Get());
        if(image.IsEmpty()) { Exclamation("Unable to decode this image within the input limits."); return; }
        path = files.Get();
        card.SetImage(image).SetHeader(f.title, GetFileName(path));
        UpdateCode();
    }
public:
    ImagingPluginDemo() {
        Title("U++ Imaging 1.0 — Plugin / Media Card Demo").Sizeable().Zoomable();
        SetRect(0, 0, DPI(1100), DPI(720));
        SetMinSize(Size(DPI(760), DPI(480)));
        title.SetTitle("IMAGING PLUGIN DEMO").SetSubTitle("Load a real file into UiMediaCard; copy the public-API code");
        Add(title); Add(open); Add(copy); Add(theme); Add(help); Add(format); Add(fit); Add(card); Add(code);
        for(int i = 0; i < 6; ++i) format.Add(i, DemoFormat(i).title);
        format.SetData(0); fit.Add(0, "Contain").Add(1, "Cover"); fit.SetData(0);
        open.SetText("Load image"); copy.SetText("Copy C++"); theme.SetText("Light / Dark"); help.SetText("Help");
        code.SetReadOnly(); card.SetEmptyCue("Select a format, then load a trusted local image");
        open.WhenAction = [this] { Load(); };
        copy.WhenAction = [this] { WriteClipboardText(AsString(code.GetData())); };
        format.WhenAction = [this] { path.Clear(); card.ClearImage(); UpdateCode(); };
        fit.WhenAction = [this] { UpdateCode(); };
        theme.WhenAction = [this] { auto ctx = UiTheme::GetContext(); ctx.mode = ctx.mode == UiThemeMode::Dark ? UiThemeMode::Light : UiThemeMode::Dark; UiTheme::Set(ctx); Refresh(); };
        help.WhenAction = [] { PromptOK("Add the chosen plugin package to .upp uses, include its header, open a FileIn with its raster class and call UiMediaCard::SetImage. The code pane shows a complete minimal program. Trusted local files only; previews are RGBA8. Use ImagingIO for full-fidelity HDR data. The full media-card style demo is in upp_Ui/examples/UiMediaCardDemo."); };
        UpdateCode();
    }
    void Layout() override {
        int w = GetSize().cx, h = GetSize().cy, rail = min(DPI(460), w / 2);
        title.SetRect(DPI(8), DPI(8), max(0, w - DPI(16)), DPI(62));
        format.SetRect(DPI(8), DPI(78), DPI(130), DPI(28)); fit.SetRect(DPI(146), DPI(78), DPI(100), DPI(28));
        open.SetRect(DPI(254), DPI(78), DPI(100), DPI(28)); copy.SetRect(DPI(362), DPI(78), DPI(100), DPI(28));
        theme.SetRect(DPI(470), DPI(78), DPI(110), DPI(28)); help.SetRect(DPI(588), DPI(78), DPI(65), DPI(28));
        card.SetRect(DPI(8), DPI(116), max(0, w - rail - DPI(24)), max(0, h - DPI(124)));
        code.SetRect(w - rail - DPI(8), DPI(116), rail, max(0, h - DPI(124)));
    }
};
GUI_APP_MAIN
{
    const auto& args = CommandLine();
    if(args.GetCount() == 5 && args[0] == "--emit") {
        SetExitCode(SaveFile(args[1], ImagingUsageCode(ScanInt(args[2]), args[3], args[4] == "cover")) ? 0 : 1);
        return;
    }
    ImagingPluginDemo().Run();
}
