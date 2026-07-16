#include <CtrlLib/CtrlLib.h>
#include <openimageio/OpenImageIO.h>
#include <opencolorio/OpenColorIO.h>

#include <cmath>
#include <string>
#include <vector>

namespace OCIO = OCIO_NAMESPACE;
using namespace Upp;
using namespace OIIO;
using namespace UppImaging;

struct Layer {
    String name;
    bool ellipse = false;
    bool visible = true;
};

static Image ToDisplayImage(const ImageBuf& source)
{
    const ImageSpec& spec = source.spec();
    if(spec.width <= 0 || spec.height <= 0)
        return Image();

    std::vector<unsigned char> pixels((size_t)spec.width * (size_t)spec.height * 4);
    ROI roi(0, spec.width, 0, spec.height, 0, 1, 0, 4);
    if(!source.get_pixels(roi, TypeDesc::UINT8, pixels.data(), 4,
                         (stride_t)spec.width * 4, AutoStride))
        return Image();

    ImageBuffer result(Size(spec.width, spec.height));
    RGBA* dst = ~result;
    for(int i = 0; i < spec.width * spec.height; ++i) {
        RGBA& c = dst[i];
        c.r = pixels[(size_t)i * 4 + 0];
        c.g = pixels[(size_t)i * 4 + 1];
        c.b = pixels[(size_t)i * 4 + 2];
        c.a = pixels[(size_t)i * 4 + 3];
    }
    return result;
}

class Canvas : public Ctrl {
public:
    void SetImage(const Image& image)
    {
        image_ = image;
        Refresh();
    }

    void Paint(Draw& draw) override
    {
        Rect r = GetSize();
        draw.DrawRect(r, SColorFace());
        if(IsNull(image_)) {
            draw.DrawText(r.CenterPoint().x - DPI(95), r.CenterPoint().y,
                          "Open an EXR or PNG", StdFont(), SColorDisabled());
            return;
        }
        Rect inner = r.Deflated(DPI(10));
        Size fit = GetFitSize(image_.GetSize(), inner.GetSize());
        Point center = inner.CenterPoint();
        draw.DrawImage(RectC(center.x - fit.cx / 2, center.y - fit.cy / 2, fit.cx, fit.cy), image_);
    }

private:
    Image image_;
};

class ImagingWorkbench : public TopWindow {
public:
    ImagingWorkbench()
    {
        Title("ImagingWorkbench");
        Sizeable().Zoomable();
        SetRect(0, 0, DPI(1180), DPI(760));

        Add(path_);
        Add(open_);
        Add(save_);
        Add(rectangle_);
        Add(ellipse_);
        Add(fit_);
        Add(actual_);
        Add(input_lut_);
        Add(output_lut_);
        Add(apply_luts_);
        Add(bypass_luts_);
        Add(layers_);
        Add(canvas_);
        Add(status_);

        open_.SetLabel("Open");
        save_.SetLabel("Save As");
        rectangle_.SetLabel("Add Rectangle Layer");
        ellipse_.SetLabel("Add Ellipse Layer");
        fit_.SetLabel("Fit");
        actual_.SetLabel("100%");
        apply_luts_.SetLabel("Apply LUTs");
        bypass_luts_.SetLabel("Bypass LUTs");
        status_.SetText("No image loaded.");

        open_.WhenAction = [this] { OpenImage(); };
        save_.WhenAction = [this] { SaveImageAs(); };
        rectangle_.WhenAction = [this] { AddLayer(false); };
        ellipse_.WhenAction = [this] { AddLayer(true); };
        fit_.WhenAction = [this] { canvas_.Refresh(); };
        actual_.WhenAction = [this] { canvas_.Refresh(); };
        apply_luts_.WhenAction = [this] { ApplyLuts(); };
        bypass_luts_.WhenAction = [this] { bypass_luts_ = !bypass_luts_; UpdatePreview(); };
        layers_.WhenSel = [this] { UpdateStatus(); };
    }

    void Layout() override
    {
        const int pad = DPI(8);
        const int row = DPI(26);
        const int left = DPI(240);
        Rect r = GetSize();
        path_.SetRect(pad, pad, r.GetWidth() - DPI(620), row);
        open_.SetRect(r.GetWidth() - DPI(600), pad, DPI(78), row);
        save_.SetRect(r.GetWidth() - DPI(514), pad, DPI(88), row);
        rectangle_.SetRect(r.GetWidth() - DPI(418), pad, DPI(138), row);
        ellipse_.SetRect(r.GetWidth() - DPI(272), pad, DPI(124), row);
        fit_.SetRect(r.GetWidth() - DPI(140), pad, DPI(62), row);
        actual_.SetRect(r.GetWidth() - DPI(70), pad, DPI(62), row);
        input_lut_.SetRect(pad, pad + row + pad, DPI(260), row);
        output_lut_.SetRect(pad + DPI(268), pad + row + pad, DPI(260), row);
        apply_luts_.SetRect(pad + DPI(536), pad + row + pad, DPI(100), row);
        bypass_luts_.SetRect(pad + DPI(644), pad + row + pad, DPI(100), row);
        layers_.SetRect(pad, pad + row * 2 + pad * 2, left, r.GetHeight() - row * 2 - DPI(64));
        canvas_.SetRect(left + pad * 2, pad + row * 2 + pad * 2, r.GetWidth() - left - pad * 3,
                        r.GetHeight() - row * 2 - DPI(64));
        status_.SetRect(pad, r.GetHeight() - DPI(28), r.GetWidth() - pad * 2, DPI(20));
    }

private:
    void OpenImage()
    {
        FileSel selector;
        selector.Type("Images", "*.exr *.png");
        if(!selector.ExecuteOpen())
            return;
        path_.SetText(~selector);
        String path = path_.GetData();
        std::string error;
        if(!UppImaging::LoadImage(~path, image_, &error)) {
            status_.SetText(Format("Open failed: %s", error.c_str()));
            return;
        }
        layers_.Reset();
        layers_.Add("Base raster", 0);
        input_lut_.Clear();
        output_lut_.Clear();
        bypass_luts_ = false;
        UpdatePreview();
    }

    void SaveImageAs()
    {
        FileSel selector;
        selector.Type("OpenEXR", "*.exr");
        selector.Type("PNG", "*.png");
        if(!selector.ExecuteSaveAs())
            return;
        std::string error;
        if(!UppImaging::SaveImage(~selector, preview_, &error))
            status_.SetText(Format("Save failed: %s", error.c_str()));
        else
            status_.SetText(Format("Saved %s", ~selector));
    }

    void AddLayer(bool ellipse)
    {
        if(image_.spec().width <= 0)
            return;
        Layer layer;
        layer.ellipse = ellipse;
        layer.name = ellipse ? "Ellipse layer" : "Rectangle layer";
        layer.name << Format(" %d", layers_.GetCount());
        layers_.Add(layer.name, ellipse ? 2 : 1);
        UpdatePreview();
    }

    bool ApplyLutFile(ImageBuf& image, const String& path, std::string& error)
    {
        if(path.IsEmpty())
            return true;
        try {
            OCIO::FileTransformRcPtr transform = OCIO::FileTransform::Create();
            transform->setSrc(~path);
            OCIO::ConstCPUProcessorRcPtr cpu = OCIO::Config::Create()
                ->getProcessor(transform)->getDefaultCPUProcessor();
            const int count = image.spec().width * image.spec().height;
            std::vector<float> pixels((size_t)count * 4);
            if(!image.get_pixels(image.roi(), TypeDesc::FLOAT, pixels.data(),
                                 4 * sizeof(float), image.spec().width * 4 * sizeof(float),
                                 AutoStride)) {
                error = image.geterror();
                return false;
            }
            for(int i = 0; i < count; ++i)
                cpu->applyRGB(pixels.data() + (size_t)i * 4);
            return image.set_pixels(image.roi(), TypeDesc::FLOAT, pixels.data(),
                                    4 * sizeof(float), image.spec().width * 4 * sizeof(float),
                                    AutoStride);
        }
        catch(const OCIO::Exception& e) {
            error = e.what();
            return false;
        }
    }

    void ApplyLuts()
    {
        if(bypass_luts_ || image_.spec().width <= 0)
            return;
        std::string error;
        if(!ApplyLutFile(image_, input_lut_.GetData(), error)
           || !ApplyLutFile(image_, output_lut_.GetData(), error)) {
            status_.SetText(Format("LUT failed: %s", error.c_str()));
            return;
        }
        status_.SetText("Input and output LUTs applied.");
        UpdatePreview();
    }

    ImageBuf Compose() const
    {
        ImageBuf result = image_;
        if(result.spec().width <= 0)
            return result;

        const int width = result.spec().width;
        const int height = result.spec().height;
        for(int i = 1; i < layers_.GetCount(); ++i) {
            std::vector<float> pixels((size_t)width * (size_t)height * 4, 0.0f);
            ImageSpec spec(width, height, 4, TypeDesc::FLOAT);
            spec.alpha_channel = 3;
            ImageBuf layer(spec, pixels.data());
            const bool ellipse = (int)layers_.Get(i, 1) == 2;
            if(!ellipse) {
                const float color[] = {1.0f, 0.25f, 0.1f, 0.45f};
                ImageBufAlgo::render_box(layer, width / 8, height / 8, width / 2, height / 2, color, true);
            }
            else {
                const float cx = width * 0.68f;
                const float cy = height * 0.52f;
                const float rx = width * 0.18f;
                const float ry = height * 0.25f;
                for(int y = 0; y < height; ++y)
                    for(int x = 0; x < width; ++x) {
                        const float dx = (x - cx) / rx;
                        const float dy = (y - cy) / ry;
                        if(dx * dx + dy * dy <= 1.0f) {
                            float* p = pixels.data() + (y * width + x) * 4;
                            p[1] = 0.8f;
                            p[2] = 1.0f;
                            p[3] = 0.45f;
                        }
                    }
            }
            result = ImageBufAlgo::over(layer, result);
        }
        return result;
    }

    void UpdatePreview()
    {
        preview_ = Compose();
        canvas_.SetImage(ToDisplayImage(preview_));
        UpdateStatus();
    }

    void UpdateStatus()
    {
        if(image_.spec().width <= 0) {
            status_.SetText("No image loaded.");
            return;
        }
        status_.SetText(Format("%dx%d  channels=%d  zoom=Fit  layers=%d  LUTs=disabled",
                               image_.spec().width, image_.spec().height,
                               image_.spec().nchannels, layers_.GetCount()));
    }

    EditString path_;
    Button open_, save_, rectangle_, ellipse_, fit_, actual_;
    EditString input_lut_, output_lut_;
    Button apply_luts_;
    Option bypass_luts_;
    ArrayCtrl layers_;
    Canvas canvas_;
    Label status_;
    ImageBuf image_;
    ImageBuf preview_;
};

GUI_APP_MAIN
{
    InitializeOpenImageIO();
    ImagingWorkbench().Run();
}
