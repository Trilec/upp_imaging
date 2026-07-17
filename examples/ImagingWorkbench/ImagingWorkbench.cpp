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
		std::vector<float> pixels(spec.width * spec.height * spec.nchannels);
		ROI roi = image.roi();
		if(!image.get_pixels(roi, TypeDesc::FLOAT, pixels.data(),
		                     spec.nchannels * sizeof(float),
		                     spec.width * spec.nchannels * sizeof(float), AutoStride)) {
			error = image.geterror();
			return false;
		}
		OCIO::PackedImageDesc desc(pixels.data(), spec.width, spec.height, spec.nchannels);
		cpu->apply(desc);
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
	ParentCtrl toolbar, layers_area, info_area, property_area, status_area;
	ArrayCtrl layer_list;
	Button up, down, remove, hide;
	Label info, status;
	ImageCanvas canvas;

	ImageBuf source;
	Image preview;
	String filename, input_lut_path, output_lut_path;
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

	ImageBuf MakeShape(const Layer& layer, Size sz) const
	{
		ImageSpec spec(sz.cx, sz.cy, 4, TypeDesc::FLOAT);
		spec.alpha_channel = 3;
		std::vector<float> pixels((size_t)sz.cx * sz.cy * 4, 0.0f);
		ImageBuf result(spec, pixels.data());
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
						float* p = pixels.data() + ((size_t)y * sz.cx + x) * 4;
						p[0] = color[0]; p[1] = color[1]; p[2] = color[2]; p[3] = color[3];
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
		String text = "File: " + filename + "\n";
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
		if(!BuildWorking(output, error, png)) {
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
		filename = path;
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
		if(row <= 1 || row + delta <= 0 || row + delta >= layers.GetCount()) return;
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
