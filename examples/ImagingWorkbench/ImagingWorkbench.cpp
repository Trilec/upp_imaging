#include <CtrlLib/CtrlLib.h>
#include <oiio/OIIO.h>
#include <opencolorio/OpenColorIO.h>

#include <algorithm>
#include <cmath>
#include <vector>

using namespace Upp;
using namespace OIIO;
namespace OCIO = OCIO_NAMESPACE;

namespace {

enum LayerKind {
	LAYER_BASE,
	LAYER_RECTANGLE,
	LAYER_ELLIPSE,
};

struct Layer : Moveable<Layer> {
	String name;
	LayerKind kind = LAYER_BASE;
	bool visible = true;
	float opacity = 1.0f;
	Color color = Color(255, 96, 64);
	Point position;
	Size size;
};

static int ToByte(float value)
{
	return (int)std::clamp(value * 255.0f + 0.5f, 0.0f, 255.0f);
}

static bool ApplyLut(ImageBuf& image, const String& filename,
	                  OCIO::TransformDirection direction, std::string& error)
{
	if(filename.IsEmpty())
		return true;
	try {
		OCIO::FileTransformRcPtr transform = OCIO::FileTransform::Create();
		transform->setSrc(filename.Begin());
		transform->setDirection(direction);
		OCIO::ConstProcessorRcPtr processor = OCIO::Config::Create()->getProcessor(transform);
		OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
		const ImageSpec& spec = image.spec();
		if(spec.nchannels != 1 && spec.nchannels != 3 && spec.nchannels != 4) {
			error = "LUT processing supports only 1, 3, or 4 channel images";
			return false;
		}
		std::vector<float> pixels(spec.width * spec.height * spec.nchannels);
		ROI roi = image.roi();
		if(!image.get_pixels(roi, TypeDesc::FLOAT, pixels.data(),
		                     spec.nchannels * sizeof(float),
		                     spec.width * spec.nchannels * sizeof(float), AutoStride)) {
			error = image.geterror();
			return false;
		}
		OCIO::PackedImageDesc desc(pixels.data(), spec.width, spec.height, spec.nchannels);
		std::vector<float> alpha;
		if(spec.nchannels == 4) {
			alpha.resize((size_t)spec.width * spec.height);
			for(size_t i = 0; i < alpha.size(); ++i)
				alpha[i] = pixels[i * 4 + 3];
		}
		cpu->apply(desc);
		if(spec.nchannels == 4)
			for(size_t i = 0; i < alpha.size(); ++i)
				pixels[i * 4 + 3] = alpha[i];
		if(!image.set_pixels(roi, TypeDesc::FLOAT, pixels.data(),
		                     spec.nchannels * sizeof(float),
		                     spec.width * spec.nchannels * sizeof(float), AutoStride)) {
			error = image.geterror();
			return false;
		}
		return true;
	}
	catch(const OCIO::Exception& e) {
		error = e.what();
		return false;
	}
}

class ImageCanvas : public Ctrl {
	Image image;
	String empty_text = "Open an EXR or PNG to begin";
	bool fit_mode = true;

	void Paint(Draw& draw) override
	{
		Size sz = GetSize();
		draw.DrawRect(sz, Color(34, 36, 40));
		const int tile = 16;
		for(int y = 0; y < sz.cy; y += tile)
			for(int x = 0; x < sz.cx; x += tile)
				if(((x / tile) + (y / tile)) & 1)
					draw.DrawRect(x, y, tile, tile, Color(46, 48, 52));
		if(image.IsEmpty()) {
			draw.DrawText(20, 20, empty_text, StdFont(), White());
			return;
		}
		Size source = image.GetSize();
		double scale = fit_mode ? std::min((double)sz.cx / source.cx, (double)sz.cy / source.cy) : 1.0;
		if(scale <= 0)
			return;
		Size target((int)(source.cx * scale), (int)(source.cy * scale));
		Rect dest((sz.cx - target.cx) / 2, (sz.cy - target.cy) / 2,
		          (sz.cx + target.cx) / 2, (sz.cy + target.cy) / 2);
		draw.DrawImage(dest, image);
	}

public:
	void SetImage(const Image& value)
	{
		image = value;
		Refresh();
	}

	void SetFit(bool fit)
	{
		fit_mode = fit;
		Refresh();
	}
};

class Workbench : public TopWindow {
	typedef Workbench CLASSNAME;
	Button open, save, input_lut, output_lut, bypass, rectangle, ellipse, fit, actual;
	Option bake_output_lut;
	ParentCtrl toolbar, layers_area, info_area, property_area, status_area;
	ArrayCtrl layer_list;
	Button up, down, remove, hide;
	Label info, status;
	Label property_caption;
	EditString layer_name;
	Option layer_visible;
	ColorPusher layer_color;
	EditDoubleSpin layer_opacity;
	EditIntSpin layer_x, layer_y, layer_width, layer_height;
	ImageCanvas canvas;

	ImageBuf source;
	Image preview;
	String filename, last_saved_filename, input_lut_path, output_lut_path;
	bool input_enabled = true;
	bool output_enabled = true;
	Vector<Layer> layers;

	void Layout() override
	{
		Size sz = GetSize();
		toolbar.SetRect(0, 0, sz.cx, 42);
		layers_area.SetRect(0, 42, 232, sz.cy - 24);
		info_area.SetRect(sz.cx - 250, 42, sz.cx, sz.cy - 24);
		canvas.SetRect(232, 42, sz.cx - 250, sz.cy - 24);
		status_area.SetRect(0, sz.cy - 24, sz.cx, sz.cy);
		open.SetRect(8, 7, 62, 34);
		save.SetRect(68, 7, 132, 34);
		input_lut.SetRect(138, 7, 222, 34);
		output_lut.SetRect(228, 7, 320, 34);
		bypass.SetRect(326, 7, 410, 34);
		rectangle.SetRect(416, 7, 505, 34);
		ellipse.SetRect(511, 7, 590, 34);
		fit.SetRect(596, 7, 646, 34);
		actual.SetRect(652, 7, 708, 34);
		layer_list.SetRect(8, 48, 224, 260);
		up.SetRect(8, 270, 78, 296);
		down.SetRect(84, 270, 154, 296);
		hide.SetRect(160, 270, 224, 296);
		remove.SetRect(8, 302, 224, 328);
		info.SetRect(8, 50, 242, 260);
		property_caption.SetRect(8, 268, 242, 302);
		layer_name.SetRect(8, 308, 242, 334);
		layer_visible.SetRect(8, 338, 242, 364);
		layer_color.SetRect(8, 368, 242, 394);
		layer_opacity.SetRect(8, 398, 242, 424);
		layer_x.SetRect(8, 428, 122, 454);
		layer_y.SetRect(128, 428, 242, 454);
		layer_width.SetRect(8, 458, 122, 484);
		layer_height.SetRect(128, 458, 242, 484);
		bake_output_lut.SetRect(8, 490, 242, 516);
		status.SetRect(8, 2, sz.cx - 8, 22);
	}

	void SetStatus(const String& text)
	{
		status.SetText(text);
	}

	void RefreshLayerList()
	{
		layer_list.Reset();
		for(const Layer& layer : layers)
			layer_list.Add(layer.name, layer.visible ? "Visible" : "Hidden");
	}

	void UpdateProperties()
	{
		int row = layer_list.GetCursor();
		bool shape = row > 0 && row < layers.GetCount();
		layer_name.Enable(shape);
		layer_visible.Enable(shape);
		layer_color.Enable(shape);
		layer_opacity.Enable(shape);
		layer_x.Enable(shape);
		layer_y.Enable(shape);
		layer_width.Enable(shape);
		layer_height.Enable(shape);
		if(!shape)
			return;
		const Layer& layer = layers[row];
		layer_name.SetData(layer.name);
		layer_visible.Set(layer.visible);
		layer_color.SetData(layer.color);
		layer_opacity.SetData(layer.opacity);
		layer_x.SetData(layer.position.x);
		layer_y.SetData(layer.position.y);
		layer_width.SetData(layer.size.cx);
		layer_height.SetData(layer.size.cy);
	}

	void ApplyProperties()
	{
		int row = layer_list.GetCursor();
		if(row <= 0 || row >= layers.GetCount())
			return;
		Layer& layer = layers[row];
		layer.name = AsString(layer_name.GetData());
		layer.visible = layer_visible.Get();
		layer.color = Color(layer_color.GetData());
		layer.opacity = (float)std::clamp((double)layer_opacity.GetData(), 0.0, 1.0);
		layer.position.x = (int)layer_x.GetData();
		layer.position.y = (int)layer_y.GetData();
		layer.size.cx = std::max(1, (int)layer_width.GetData());
		layer.size.cy = std::max(1, (int)layer_height.GetData());
		RefreshLayerList();
		layer_list.SetCursor(row);
		Rebuild();
	}

	ImageBuf MakeShape(const Layer& layer, Size sz) const
	{
		ImageSpec spec(sz.cx, sz.cy, 4, TypeDesc::FLOAT);
		spec.alpha_channel = 3;
		ImageBuf result(spec);
		float color[4] = { layer.color.GetR() / 255.0f,
		                   layer.color.GetG() / 255.0f,
		                   layer.color.GetB() / 255.0f,
		                   layer.opacity };
		if(layer.kind == LAYER_RECTANGLE) {
			ImageBufAlgo::render_box(result, layer.position.x, layer.position.y,
			                         layer.position.x + layer.size.cx,
			                         layer.position.y + layer.size.cy, color, true);
		}
		else {
			for(int y = layer.position.y; y < layer.position.y + layer.size.cy; ++y)
				for(int x = layer.position.x; x < layer.position.x + layer.size.cx; ++x) {
					double dx = (x - layer.position.x - layer.size.cx / 2.0) /
					            std::max(1.0, layer.size.cx / 2.0);
					double dy = (y - layer.position.y - layer.size.cy / 2.0) /
					            std::max(1.0, layer.size.cy / 2.0);
					if(dx * dx + dy * dy <= 1.0 && x >= 0 && y >= 0 && x < sz.cx && y < sz.cy) {
						result.setpixel(x, y, color);
					}
				}
		}
		return result;
	}

	bool BuildWorking(ImageBuf& working, std::string& error, bool include_output) const
	{
		if(!source.initialized()) {
			error = "no image is loaded";
			return false;
		}
		working = source;
		if(input_enabled && !ApplyLut(working, input_lut_path,
		                              OCIO::TRANSFORM_DIR_FORWARD, error))
			return false;
		for(int i = 1; i < layers.GetCount(); ++i)
			if(layers[i].visible)
				working = ImageBufAlgo::over(MakeShape(layers[i],
				                                     Size(working.spec().width, working.spec().height)), working);
		if(include_output && output_enabled && !ApplyLut(working, output_lut_path,
		                                                  OCIO::TRANSFORM_DIR_FORWARD, error))
			return false;
		return true;
	}

	void UpdateInfo()
	{
		if(!source.initialized()) {
			info.SetText("No image loaded");
			return;
		}
		const ImageSpec& spec = source.spec();
		String text = "Source: " + filename + "\n";
		text << "Last saved: " << last_saved_filename << "\n";
		text << "Size: " << spec.width << " x " << spec.height << "\n";
		text << "Channels: " << spec.nchannels << "\n";
		text << "Type: " << spec.format.c_str() << "\n";
		text << "Subimages: " << source.nsubimages() << "\n";
		text << "ColorSpace: " << OIIO::c_str(spec.get_string_attribute("oiio:ColorSpace", "(none)")) << "\n\n";
		text << "Channel names:\n";
		for(const String& name : spec.channelnames)
			text << "  " << name << "\n";
		info.SetText(text);
	}

	void Rebuild()
	{
		std::string error;
		ImageBuf working;
		if(!BuildWorking(working, error, true)) {
			SetStatus("Processing error: " + String(error.c_str()));
			return;
		}
		ImageSpec spec = working.spec();
		std::vector<float> pixels((size_t)spec.width * spec.height * spec.nchannels);
		if(!working.get_pixels(working.roi(), TypeDesc::FLOAT, pixels.data(),
		                       spec.nchannels * sizeof(float),
		                       spec.width * spec.nchannels * sizeof(float), AutoStride)) {
			SetStatus("Preview error: " + String(working.geterror().c_str()));
			return;
		}
		ImageBuffer buffer(spec.width, spec.height);
		for(int y = 0; y < spec.height; ++y)
			for(int x = 0; x < spec.width; ++x) {
				const float* p = pixels.data() + ((size_t)y * spec.width + x) * spec.nchannels;
				RGBA& out = buffer[y][x];
				out.r = ToByte(p[0]); out.g = ToByte(spec.nchannels > 1 ? p[1] : p[0]);
				out.b = ToByte(spec.nchannels > 2 ? p[2] : p[0]);
				out.a = ToByte(spec.alpha_channel >= 0 ? p[spec.alpha_channel] : 1.0f);
			}
		preview = buffer;
		canvas.SetImage(preview);
		SetStatus("Ready: " + filename);
	}

	void Open()
	{
		FileSel select;
		select.Type("Images", "*.exr;*.png");
		if(!select.ExecuteOpen("Open image"))
			return;
		std::string error;
		ImageBuf loaded;
		if(!UppImaging::LoadImage(select.Get().Begin(), loaded, &error)) {
			Exclamation("Unable to open image:\n" + String(error.c_str()));
			return;
		}
		filename = select.Get();
		source = loaded;
		layers.Clear();
		Layer base;
		base.name = "Raster base";
		layers.Add(base);
		RefreshLayerList();
		UpdateInfo();
		Rebuild();
	}

	void Save()
	{
		if(!source.initialized()) {
			Exclamation("Open an image before saving.");
			return;
		}
		FileSel select;
		select.Type("OpenEXR", "*.exr").Type("PNG", "*.png");
		if(!select.ExecuteSaveAs("Save image"))
			return;
		std::string error;
		String path = select.Get();
		bool png = ToLower(GetFileExt(path)) == ".png";
		ImageBuf output;
		if(!BuildWorking(output, error, png || bake_output_lut.Get())) {
			Exclamation("Unable to process image:\n" + String(error.c_str()));
			return;
		}
		if(!UppImaging::SaveImage(path.Begin(), output, &error)) {
			Exclamation("Unable to save image:\n" + String(error.c_str()));
			return;
		}
		ImageBuf reopened;
		if(!UppImaging::LoadImage(path.Begin(), reopened, &error)) {
			Exclamation("Saved image could not be reopened:\n" + String(error.c_str()));
			return;
		}
		last_saved_filename = path;
		UpdateInfo();
		SetStatus("Saved and reopened: " + path);
	}

	void SelectLut(bool input)
	{
		FileSel select;
		select.Type("Color LUT", "*.cube");
		if(!select.ExecuteOpen(input ? "Select input LUT" : "Select output LUT"))
			return;
		if(input) input_lut_path = select.Get();
		else output_lut_path = select.Get();
		Rebuild();
	}

	void AddShape(LayerKind kind)
	{
		if(!source.initialized()) return;
		Layer layer;
		layer.kind = kind;
		layer.name = kind == LAYER_RECTANGLE ? "Rectangle" : "Ellipse";
		Size sz(source.spec().width, source.spec().height);
		layer.size = kind == LAYER_RECTANGLE ? Size(sz.cx / 3, sz.cy / 3) : Size(sz.cx / 4, sz.cy / 3);
		layer.position = kind == LAYER_RECTANGLE ? Point(sz.cx / 3, sz.cy / 3) : Point(sz.cx / 2, sz.cy / 3);
		layer.color = kind == LAYER_RECTANGLE ? Color(255, 96, 64) : Color(64, 144, 255);
		layer.opacity = 0.55f;
		layers.Add(layer);
		RefreshLayerList();
		Rebuild();
	}

	void MoveLayer(int delta)
	{
		int row = layer_list.GetCursor();
		if(row <= 0 || row + delta <= 0 || row + delta >= layers.GetCount()) return;
		Swap(layers[row], layers[row + delta]);
		RefreshLayerList();
		layer_list.SetCursor(row + delta);
		Rebuild();
	}

	void ToggleLayer()
	{
		int row = layer_list.GetCursor();
		if(row <= 0 || row >= layers.GetCount()) return;
		layers[row].visible = !layers[row].visible;
		RefreshLayerList();
		layer_list.SetCursor(row);
		Rebuild();
	}

	void RemoveLayer()
	{
		int row = layer_list.GetCursor();
		if(row <= 0 || row >= layers.GetCount()) return;
		layers.Remove(row);
		RefreshLayerList();
		Rebuild();
	}

	void ToggleLuts()
	{
		input_enabled = output_enabled = !(input_enabled || output_enabled);
		Rebuild();
	}

	void RefreshCanvas()
	{
		canvas.Refresh();
	}

	void FitCanvas()
	{
		canvas.SetFit(true);
	}

	void ActualCanvas()
	{
		canvas.SetFit(false);
	}

public:
	Workbench()
	{
		Title("ImagingWorkbench").Sizeable().Zoomable();
		Add(toolbar); Add(layers_area); Add(info_area); Add(canvas); Add(status_area);
		toolbar.Add(open.SetLabel("Open")); toolbar.Add(save.SetLabel("Save As"));
		toolbar.Add(input_lut.SetLabel("Input LUT")); toolbar.Add(output_lut.SetLabel("Output LUT"));
		toolbar.Add(bypass.SetLabel("Bypass LUTs")); toolbar.Add(rectangle.SetLabel("Add Rectangle"));
		toolbar.Add(ellipse.SetLabel("Add Ellipse")); toolbar.Add(fit.SetLabel("Fit"));
		toolbar.Add(actual.SetLabel("100%"));
		layers_area.Add(layer_list); layers_area.Add(up.SetLabel("Move Up")); layers_area.Add(down.SetLabel("Move Down"));
		layers_area.Add(hide.SetLabel("Hide/Show")); layers_area.Add(remove.SetLabel("Delete"));
		info_area.Add(info); status_area.Add(status);
		info_area.Add(property_caption.SetText("Selected shape properties"));
		info_area.Add(layer_name); info_area.Add(layer_visible.SetLabel("Visible"));
		info_area.Add(layer_color); info_area.Add(layer_opacity);
		info_area.Add(layer_x); info_area.Add(layer_y); info_area.Add(layer_width); info_area.Add(layer_height);
		info_area.Add(bake_output_lut.SetLabel("Bake output LUT for EXR"));
		layer_name.SetText("Name");
		layer_color.WithText(true).WithHex(true);
		layer_opacity.MinMax(0, 1);
		layer_x.MinMax(-100000, 100000); layer_y.MinMax(-100000, 100000);
		layer_width.MinMax(1, 100000); layer_height.MinMax(1, 100000);
		layer_list.WhenSel = THISBACK(UpdateProperties);
		layer_name.WhenAction = THISBACK(ApplyProperties);
		layer_visible.WhenAction = THISBACK(ApplyProperties);
		layer_color.WhenAction = THISBACK(ApplyProperties);
		layer_opacity.WhenAction = THISBACK(ApplyProperties);
		layer_x.WhenAction = THISBACK(ApplyProperties); layer_y.WhenAction = THISBACK(ApplyProperties);
		layer_width.WhenAction = THISBACK(ApplyProperties); layer_height.WhenAction = THISBACK(ApplyProperties);
		open <<= THISBACK(Open); save <<= THISBACK(Save);
		input_lut <<= THISBACK1(SelectLut, true); output_lut <<= THISBACK1(SelectLut, false);
		bypass <<= THISBACK(ToggleLuts);
		rectangle <<= THISBACK1(AddShape, LAYER_RECTANGLE); ellipse <<= THISBACK1(AddShape, LAYER_ELLIPSE);
		fit <<= THISBACK(FitCanvas); actual <<= THISBACK(ActualCanvas);
		up <<= THISBACK1(MoveLayer, -1); down <<= THISBACK1(MoveLayer, 1);
		hide <<= THISBACK(ToggleLayer); remove <<= THISBACK(RemoveLayer);
		SetStatus("Ready");
	}
};

}

GUI_APP_MAIN
{
	Workbench().Run();
}
