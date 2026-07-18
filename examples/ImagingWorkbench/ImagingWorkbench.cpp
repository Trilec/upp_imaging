#include "ImagingWorkbench.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <sstream>
#include <vector>

namespace Upp {

namespace {

struct PreviewMapping {
	Vector<int> rgb;
	int alpha = -1;
	bool grayscale = false;
};

static int ClampByte(float value)
{
	if(!std::isfinite(value))
		value = 0.0f;
	value = std::clamp(value, 0.0f, 1.0f);
	return (int)(value * 255.0f + 0.5f);
}

static String ShortChannelName(const String& name)
{
	int dot = name.ReverseFind('.');
	return dot >= 0 ? name.Mid(dot + 1) : name;
}

static int FindIndexByLeaf(const Vector<String>& names, const String& value)
{
	String target = ToUpper(value);
	for(int i = 0; i < names.GetCount(); ++i) {
		if(ToUpper(ShortChannelName(names[i])) == target)
			return i;
	}
	return -1;
}

static String ChannelPrefix(const String& name)
{
	int dot = name.ReverseFind('.');
	return dot >= 0 ? name.Left(dot) : String();
}

static bool IsRGBLeaf(const String& name)
{
	String upper = ToUpper(name);
	return upper == "R" || upper == "G" || upper == "B";
}

static bool IsAlphaLeaf(const String& name)
{
	String upper = ToUpper(name);
	return upper == "A" || upper == "ALPHA";
}

static String JoinChannels(const Vector<String>& channels)
{
	String out;
	for(int i = 0; i < channels.GetCount(); ++i) {
		if(i)
			out << ' ';
		out << channels[i];
	}
	return out;
}

static String HumanBytes(int64 bytes)
{
	double value = (double)bytes;
	const char* unit = "B";
	if(value >= 1024.0) { value /= 1024.0; unit = "KB"; }
	if(value >= 1024.0) { value /= 1024.0; unit = "MB"; }
	if(value >= 1024.0) { value /= 1024.0; unit = "GB"; }
	return Format("%.1f %s", value, unit);
}

static String TypeDescText(const OIIO::TypeDesc& type)
{
	std::ostringstream ss;
	ss << type;
	return String(ss.str());
}

static String StringFromView(const OIIO::string_view& view)
{
	return String(view.data(), (int)view.size());
}

static int ResolveAlphaChannel(const OIIO::ImageSpec& spec, const Vector<String>& names)
{
	if(spec.alpha_channel >= 0 && spec.alpha_channel < spec.nchannels)
		return spec.alpha_channel;
	int index = FindIndexByLeaf(names, "A");
	if(index >= 0)
		return index;
	index = FindIndexByLeaf(names, "Alpha");
	if(index >= 0)
		return index;
	return -1;
}

static PreviewMapping ResolvePreviewMapping(const OIIO::ImageSpec& spec,
	                                          String& choice)
{
	Vector<String> names;
	for(const std::string& n : spec.channelnames)
		names.Add(n.c_str());

	PreviewMapping map;
	choice.Clear();

	int r = FindIndexByLeaf(names, "R");
	int g = FindIndexByLeaf(names, "G");
	int b = FindIndexByLeaf(names, "B");
	int a = ResolveAlphaChannel(spec, names);
	if(r >= 0 && g >= 0 && b >= 0) {
		map.rgb.Add(r);
		map.rgb.Add(g);
		map.rgb.Add(b);
		map.alpha = a;
		choice = a >= 0 ? "RGBA" : "RGB";
		return map;
	}

	if(spec.nchannels == 1) {
		map.rgb.Add(0);
		map.grayscale = true;
		choice = names.IsEmpty() ? "greyscale" : names[0] + " as greyscale";
		return map;
	}

	if(spec.nchannels == 2) {
		map.rgb.Add(0);
		map.alpha = 1;
		map.grayscale = true;
		choice = names.IsEmpty() ? "greyscale + alpha" : names[0] + " + Alpha";
		return map;
	}

	map.rgb.Add(0);
	map.grayscale = true;
	choice = names.IsEmpty() ? "greyscale" : names[0] + " as greyscale";
	return map;
}

static String SaveExtensionForFormat(const String& format)
{
	return ToUpper(format) == "PNG" ? String(".png") : String(".exr");
}

static bool ValidateSaveExtension(String& path, const String& format, String& error)
{
	String desired = SaveExtensionForFormat(format);
	String ext = ToLower(GetFileExt(path));
	if(ext.IsEmpty()) {
		path = ForceExt(path, desired);
		return true;
	}
	if(ext != ".exr" && ext != ".png") {
		error = "unsupported output extension";
		return false;
	}
	if(ext != desired) {
		String msg = "The filename extension does not match the selected format.\n\n";
		msg << "Selected format: " << format << "\n";
		msg << "Filename: " << GetFileName(path) << "\n\n";
		msg << "Choose Yes to correct the extension automatically.";
		if(PromptYesNo(msg) != 1) {
			error = "filename extension does not match selected format";
			return false;
		}
		path = ForceExt(path, desired);
	}
	return true;
}

} // namespace

ImagingWorkbench::ImagingWorkbench()
{
	BindActions();
	PostBuild();
}

void ImagingWorkbench::BindActions()
{
	quit_button.WhenAction = [=] { Close(); };
	load_button.WhenAction = [=] { DoLoad(); };
	save_split_button.WhenAction = [=] { DoSave(); };
	save_split_button.WhenSelect = [=](int, const Value& data) { DoSaveFormat(data); };
	fit_view_button.WhenAction = [=] { canvas.SetFitMode(true); UpdateCanvasZoomLabel(); };
}

bool ImagingWorkbench::HotKey(dword key)
{
	if(key == K_CTRL_O) {
		DoLoad();
		return true;
	}
	if(key == K_CTRL_S) {
		DoSave();
		return true;
	}
	return false;
}

void ImagingWorkbench::Paint(Draw& w)
{
	w.DrawRect(GetSize(), Color(25, 25, 25));
}

void ImagingWorkbench::PostBuild()
{
	layers_layout.SetDirection(UiDirection::V).SetGap(DPI(6), DPI(6)).SetInset(DPI(8)).SetWrap(UiBoxWrap::None);
	layers_summary.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	layers_detail.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	layers_tree.SetCustomStyle(UiTheme::ResolveTree());
	layers_tree.SetRootVisible(false);
	layers_tree.SetSelectionMode(UITREESEL_SINGLE);
	layers_tree.ShowConnectorLines(true);
	layers_tree.ShowMetadataMarker(true);
	layers_tree.WhenSelection = [=] { UpdateSelectionSummary(); };
	layers_layout.Add(layers_summary).Fit().MinCross(DPI(0));
	layers_layout.Add(layers_tree).Expand(1).MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	layers_layout.Add(layers_detail).Fit().MinCross(DPI(0));
	pageA.Add(layers_layout.SizePos());

	ocio_body.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	ocio_body.SetText("OCIO controls will be connected after the image viewer and save path.")
		.SetAlign(UiAlign::LEFT, UiAlign::CENTER);
	pageB.Add(ocio_body.SizePos());

	analysis_body.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	analysis_body.SetText("Histogram and pixel analysis will be connected after viewer controls.")
		.SetAlign(UiAlign::LEFT, UiAlign::CENTER);
	pageC.Add(analysis_body.SizePos());

	canvas.SetPlaceholderText("Open an EXR or PNG to begin");
	canvas.WhenViewChanged = [=] { UpdateCanvasZoomLabel(); };
	canvas_scroll_panel.Content().Add(canvas.SizePos());

	load_button.Enable();
	save_split_button.Disable();
	r_too.Disable();
	g_tool.Disable();
	b_tool.Disable();
	a_tool.Disable();
	rbg_tool.Disable();
	exposure.Disable();
	exposure_slider.Disable();
	exposure_float_edit.Disable();
	gamma.Disable();
	gamma_slider.Disable();
	gamma_float_edit.Disable();
	fit_view_button.Disable();

	res_label.SetText("—");
	memory_label.SetText("—");
	label_02.SetText("—");
	status.SetText("Open an EXR or PNG to begin");
	layers_summary.SetText("Open an EXR or PNG to inspect channels and subimages.");
	layers_detail.SetText("No image loaded.");
	right_tab.SetActiveTab(0);
}

void ImagingWorkbench::SetStatus(const String& text)
{
	status.SetText(text);
}

String ImagingWorkbench::FormatBytes(int64 bytes) const
{
	return HumanBytes(bytes);
}

bool ImagingWorkbench::IsRGBChannelName(const String& name)
{
	return IsRGBLeaf(name);
}

bool ImagingWorkbench::IsAlphaChannelName(const String& name)
{
	return IsAlphaLeaf(name);
}

String ImagingWorkbench::GroupPrefix(const String& name)
{
	return ChannelPrefix(name);
}

void ImagingWorkbench::UpdateCanvasZoomLabel()
{
	if(!canvas.HasImage()) {
		label_02.SetText("—");
		return;
	}
	int percent = (int)std::round(canvas.GetDisplayedScale() * 100.0);
	if(percent < 1)
		percent = 1;
	label_02.SetText(Format("%d%%", percent));
}

void ImagingWorkbench::UpdateDisplayState()
{
	if(!source_image.initialized()) {
		resolution_text = "—";
		memory_text = "—";
		res_label.SetText(resolution_text);
		memory_label.SetText(memory_text);
		return;
 	}

	const OIIO::ImageSpec& spec = source_image.spec();
	resolution_text = Format("%d x %d", spec.width, spec.height);
	memory_text = FormatBytes((int64)spec.width * spec.height * std::max(1, spec.nchannels) * (int64)sizeof(float));
	res_label.SetText(resolution_text);
	memory_label.SetText(memory_text);
	UpdateCanvasZoomLabel();
}

void ImagingWorkbench::UpdateSelectionSummary()
{
	if(!layers_tree.GetCursor().IsValid()) {
		layers_detail.SetText(DescribePreviewChoice());
		return;
	}

	const UiTreeNodeRef node = layers_tree.GetCursor();
	const UiModelItem& item = layers_tree.GetModel().Get(node);
	String text = item.text;
	if(!item.right_text.IsEmpty())
		text << "\n" << item.right_text;
	if(!item.description.IsEmpty())
		text << "\n" << item.description;
	layers_detail.SetText(text);
}

String ImagingWorkbench::DescribePreviewChoice() const
{
	return preview_choice.IsEmpty() ? String("Preview: none") : String("Preview: ") + preview_choice;
}

void ImagingWorkbench::ScanSourceMetadata()
{
	channel_groups.Clear();
	subimages.Clear();
	subimage_count = 0;

	std::unique_ptr<OIIO::ImageInput> input(OIIO::ImageInput::open(source_filename.Begin()));
	if(!input) {
		last_error = "Unable to inspect image metadata";
		return;
	}

	for(int subimage = 0; input->seek_subimage(subimage, 0); ++subimage) {
		++subimage_count;
		const OIIO::ImageSpec& spec = input->spec();
		ImageSubimageInfo info;
		info.size = Size(spec.width, spec.height);
		info.pixel_type = TypeDescText(spec.format);
		info.color_space = StringFromView(spec.get_string_attribute("oiio:ColorSpace"));
		info.channel_count = spec.nchannels;
		subimages.Add(info);
		Vector<String> names;
		for(const std::string& name : spec.channelnames)
			names.Add(name.c_str());

		struct TempGroup : Moveable<TempGroup> {
			String prefix;
			String channels_text;
			int channel_count = 0;
		};
		Vector<TempGroup> temp;
		for(const String& name : names) {
			String prefix = GroupPrefix(name);
			String leaf = ShortChannelName(name);
			int index = -1;
			for(int i = 0; i < temp.GetCount(); ++i) {
				if(temp[i].prefix == prefix) {
					index = i;
					break;
				}
			}
			if(index < 0) {
				TempGroup group;
				group.prefix = prefix;
				temp.Add(group);
				index = temp.GetCount() - 1;
			}
			if(!temp[index].channels_text.IsEmpty())
				temp[index].channels_text << ' ';
			temp[index].channels_text << leaf;
			++temp[index].channel_count;
		}

		for(const TempGroup& group : temp) {
			ImageChannelGroup out;
			out.subimage = subimage;
			out.channels_text = group.channels_text;
			out.channel_count = group.channel_count;
			if(!group.prefix.IsEmpty())
				out.name = group.prefix;
			else if(group.channel_count == 1)
				out.name = group.channels_text;
			else
				out.name = "Channels";
			channel_groups.Add(out);
		}
	}

	input->close();
}

void ImagingWorkbench::BuildPreviewImage()
{
	preview_image = Image();
	if(!source_image.initialized()) {
		canvas.ClearImage();
		return;
	}

	const OIIO::ImageSpec& spec = source_image.spec();
	if(spec.width <= 0 || spec.height <= 0 || spec.nchannels <= 0) {
		canvas.ClearImage();
		return;
	}

	Vector<String> names;
	for(const std::string& name : spec.channelnames)
		names.Add(name.c_str());

	PreviewMapping mapping = ResolvePreviewMapping(spec, preview_choice);
	if(mapping.rgb.IsEmpty()) {
		canvas.ClearImage();
		return;
	}

	std::vector<float> pixels((size_t)spec.width * spec.height * spec.nchannels);
	if(!source_image.get_pixels(source_image.roi(), OIIO::TypeDesc::FLOAT, pixels.data(),
		                       spec.nchannels * sizeof(float),
		                       spec.width * spec.nchannels * sizeof(float), OIIO::AutoStride)) {
		last_error = source_image.geterror();
		canvas.ClearImage();
		return;
	}

	ImageBuffer buffer(Size(spec.width, spec.height));
	for(int y = 0; y < spec.height; ++y) {
		for(int x = 0; x < spec.width; ++x) {
			const float* src = pixels.data() + ((size_t)y * spec.width + x) * spec.nchannels;
			RGBA& dst = buffer[y][x];
			if(mapping.grayscale) {
				float gray = src[mapping.rgb[0]];
				dst.r = ClampByte(gray);
				dst.g = ClampByte(gray);
				dst.b = ClampByte(gray);
			}
			else {
				dst.r = ClampByte(src[mapping.rgb[0]]);
				dst.g = ClampByte(src[mapping.rgb[1]]);
				dst.b = ClampByte(src[mapping.rgb[2]]);
			}
			dst.a = mapping.alpha >= 0 ? ClampByte(src[mapping.alpha]) : 255;
		}
	}
	preview_image = buffer;
	canvas.SetImage(preview_image);
	canvas.SetFitMode(true);
}

void ImagingWorkbench::DoSave()
{
	DoSaveFormat(last_save_format);
}

void ImagingWorkbench::DoSaveFormat(const Value& data)
{
	String format = ToUpper(AsString(data));
	if(format != "EXR" && format != "PNG")
		format = "EXR";
	last_save_format = format;

	if(!source_image.initialized()) {
		Exclamation("Load an image before saving.");
		SetStatus("Save failed: no image loaded");
		return;
	}

	FileSel selector;
	if(format == "PNG")
		selector.Type("PNG", "*.png");
	else
		selector.Type("EXR", "*.exr");
	selector.DefaultExt(format == "PNG" ? "png" : "exr");
	String seed = last_saved_filename.IsEmpty() ? source_filename : last_saved_filename;
	if(seed.IsEmpty())
		seed = "image" + SaveExtensionForFormat(format);
	selector.DefaultName(GetFileTitle(seed) + SaveExtensionForFormat(format));
	if(!seed.IsEmpty())
		selector.ActiveDir(GetFileFolder(seed));

	if(!selector.ExecuteSaveAs(format == "PNG" ? "Save PNG" : "Save EXR"))
		return;

	String path = selector.Get();
	String error;
	if(!SaveCurrentImage(path, format, error)) {
		Exclamation("Save failed:\n" + error);
		SetStatus("Save failed: " + error);
		return;
	}

	last_saved_filename = path;
	SetStatus("Saved and verified: " + path);
}

bool ImagingWorkbench::SaveCurrentImage(String& path, const String& format, String& error)
{
	if(!source_image.initialized()) {
		error = "no image loaded";
		return false;
	}

	if(!ValidateSaveExtension(path, format, error))
		return false;

	std::string io_error;
	OIIO::ImageSpec expected_spec;
	std::vector<std::string> expected_names;
	int expected_alpha = -1;
	int expected_channels = 0;
	const OIIO::ImageSpec& source_spec = source_image.spec();

	if(ToUpper(format) == "EXR") {
		expected_spec = source_spec;
		expected_names = source_spec.channelnames;
		expected_channels = source_spec.nchannels;
		expected_alpha = source_spec.alpha_channel;
		if(!UppImaging::SaveImage(path.Begin(), source_image, &io_error)) {
			error = io_error.c_str();
			return false;
		}
	}
	else {
		String choice = preview_choice;
		PreviewMapping mapping = ResolvePreviewMapping(source_spec, choice);
		if(mapping.rgb.IsEmpty()) {
			error = "no displayable preview channels";
			return false;
		}

		std::vector<float> source_pixels((size_t)source_spec.width * source_spec.height * source_spec.nchannels);
		if(!source_image.get_pixels(source_image.roi(), OIIO::TypeDesc::FLOAT, source_pixels.data(),
			                       source_spec.nchannels * sizeof(float),
			                       source_spec.width * source_spec.nchannels * sizeof(float), OIIO::AutoStride)) {
			error = source_image.geterror();
			return false;
		}

		expected_channels = mapping.alpha >= 0 ? 4 : 3;
		expected_alpha = mapping.alpha >= 0 ? expected_channels - 1 : -1;
		expected_names.clear();
		if(mapping.alpha >= 0) {
			expected_names.push_back("R");
			expected_names.push_back("G");
			expected_names.push_back("B");
			expected_names.push_back("A");
		}
		else {
			expected_names.push_back("R");
			expected_names.push_back("G");
			expected_names.push_back("B");
		}

		std::vector<float> output_pixels((size_t)source_spec.width * source_spec.height * expected_channels);
		for(int y = 0; y < source_spec.height; ++y) {
			for(int x = 0; x < source_spec.width; ++x) {
				const float* src = source_pixels.data() + ((size_t)y * source_spec.width + x) * source_spec.nchannels;
				float* dst = output_pixels.data() + ((size_t)y * source_spec.width + x) * expected_channels;
				if(mapping.grayscale) {
					float gray = src[mapping.rgb[0]];
					dst[0] = gray;
					dst[1] = gray;
					dst[2] = gray;
				}
				else {
					dst[0] = src[mapping.rgb[0]];
					dst[1] = src[mapping.rgb[1]];
					dst[2] = src[mapping.rgb[2]];
				}
				if(expected_channels == 4)
					dst[3] = src[mapping.alpha];
			}
		}

		expected_spec = OIIO::ImageSpec(source_spec.width, source_spec.height, expected_channels, OIIO::TypeDesc::FLOAT);
		expected_spec.channelnames = expected_names;
		expected_spec.alpha_channel = expected_alpha;
		String source_cs = StringFromView(source_spec.get_string_attribute("oiio:ColorSpace"));
		if(!source_cs.IsEmpty())
			expected_spec.attribute("oiio:ColorSpace", source_cs.Begin());

		OIIO::ImageBuf png_buf(expected_spec);
		if(!png_buf.set_pixels(png_buf.roi(), OIIO::TypeDesc::FLOAT, output_pixels.data(),
					       OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride)) {
			error = png_buf.geterror();
			return false;
		}

		if(!UppImaging::SaveImage(path.Begin(), png_buf, &io_error)) {
			error = io_error.c_str();
			return false;
		}
	}

	OIIO::ImageBuf reopened;
	if(!UppImaging::LoadImage(path.Begin(), reopened, &io_error)) {
		error = String("saved file cannot be reopened: ") + io_error.c_str();
		return false;
	}

	if(reopened.spec().width != expected_spec.width || reopened.spec().height != expected_spec.height) {
		error = "reopened file dimensions do not match saved output";
		return false;
	}
	if(reopened.spec().nchannels != expected_channels) {
		error = "reopened file channel count does not match saved output";
		return false;
	}
	if(reopened.spec().alpha_channel != expected_alpha) {
		error = "reopened file alpha metadata does not match saved output";
		return false;
	}

	float probe[8] = {};
	reopened.getpixel(0, 0, probe);
	if(!std::isfinite(probe[0])) {
		error = "saved file pixel probe failed";
		return false;
	}

	if(ToUpper(format) == "EXR") {
		if(reopened.spec().channelnames != expected_names) {
			error = "saved EXR channel names changed";
			return false;
		}
		if(reopened.spec().alpha_channel != source_spec.alpha_channel) {
			error = "saved EXR alpha metadata changed";
			return false;
		}
	}

	return true;
}

void ImagingWorkbench::UpdateLayersPage()
{
	UiTreeModel& model = layers_tree.GetInternalModel();
	model.Clear();

	if(!source_image.initialized()) {
		UiModelItem empty("No image loaded");
		empty.description = "Open an EXR or PNG to inspect channels and subimages.";
		model.AddChild(model.Root(), empty);
		layers_summary.SetText("Open an EXR or PNG to inspect channels and subimages.");
		layers_detail.SetText("No image loaded.");
		layers_tree.ClearSelection();
		return;
	}

	const OIIO::ImageSpec& spec = source_image.spec();
	String basename = GetFileName(source_filename);
	String color_space = StringFromView(spec.get_string_attribute("oiio:ColorSpace"));
	if(color_space.IsEmpty())
		color_space = "(none)";

	String channel_text;
	Vector<String> channel_names;
	for(const std::string& name : spec.channelnames)
		channel_names.Add(name.c_str());
	channel_text = JoinChannels(channel_names);

	String source_type = TypeDescText(spec.format);
	String summary;
	summary << basename << "\n";
	summary << resolution_text << "\n";
	summary << spec.nchannels << " channels: " << channel_text << "\n";
	summary << subimage_count << " subimage" << (subimage_count == 1 ? "" : "s") << "\n";
	summary << source_type << " source / FLOAT working buffer\n";
	summary << "Color space: " << color_space << "\n";
	summary << memory_text;
	layers_summary.SetText(summary);

	UiModelItem file_item;
	file_item.text = basename;
	file_item.right_text = subimage_count > 1 ? Format("%d subimages", subimage_count) : String("1 subimage");
	file_item.description = summary;
	UiTreeNodeRef file_node = model.AddChild(model.Root(), file_item);

	for(int subimage = 0; subimage < std::max(1, subimage_count); ++subimage) {
		const ImageSubimageInfo* subinfo = subimages.IsEmpty() ? nullptr : &subimages[std::min(subimage, subimages.GetCount() - 1)];
		String sub_label = subimage_count > 1 ? Format("Subimage %d", subimage) : String("Image");
		UiModelItem sub_item;
		sub_item.text = sub_label;
		if(subinfo) {
			sub_item.right_text = Format("%d x %d / %s", subinfo->size.cx, subinfo->size.cy, subinfo->pixel_type);
			sub_item.description = Format("%d channels / preview %s", subinfo->channel_count, preview_choice);
		}
		else {
		sub_item.right_text = Format("%d x %d / %s", spec.width, spec.height, TypeDescText(spec.format));
			sub_item.description = Format("%d channels / preview %s", spec.nchannels, preview_choice);
		}
		UiTreeNodeRef sub_node = model.AddChild(file_node, sub_item);

		for(const ImageChannelGroup& group : channel_groups) {
			if(group.subimage != subimage)
				continue;
			UiModelItem group_item;
			group_item.text = group.name;
			group_item.right_text = group.channels_text;
			group_item.description = Format("%d channel%s", group.channel_count, group.channel_count == 1 ? "" : "s");
			UiTreeNodeRef group_node = model.AddChild(sub_node, group_item);
			for(const std::string& channel_name : spec.channelnames) {
				String channel = channel_name.c_str();
				if(GroupPrefix(channel) != group.name && !(group.name.IsEmpty() && GroupPrefix(channel).IsEmpty()))
					continue;
				UiModelItem channel_item;
				channel_item.text = channel;
				channel_item.description = Format("subimage %d channel", subimage);
				model.AddChild(group_node, channel_item);
			}
		}
	}

	layers_tree.SetCursor(file_node);
	UpdateSelectionSummary();
}

void ImagingWorkbench::DoLoad()
{
	FileSel selector;
	selector.Type("OpenEXR and PNG", "*.exr;*.png");
	if(!selector.ExecuteOpen("Open image"))
		return;

	String path = selector.Get();
	if(path.IsEmpty())
		return;

	String ext = ToLower(GetFileExt(path));
	if(ext != ".exr" && ext != ".png") {
		Exclamation("Unsupported file type. Use EXR or PNG.");
		SetStatus("Unable to load: unsupported extension");
		return;
	}

	SetStatus("Loading: " + GetFileName(path));

	std::string error;
	OIIO::ImageBuf loaded;
	if(!UppImaging::LoadImage(path.Begin(), loaded, &error)) {
		last_error = error.c_str();
		Exclamation("Unable to open image:\n" + String(error.c_str()));
		SetStatus("Unable to load: " + String(error.c_str()));
		return;
	}

	source_image = loaded;
	source_filename = path;
	last_error.Clear();
	ScanSourceMetadata();
	BuildPreviewImage();
	UpdateDisplayState();
	UpdateLayersPage();
	canvas.SetFitMode(true);
	save_split_button.Enable();
	fit_view_button.Enable();
	SetStatus("Loaded: " + GetFileName(path));
}

} // namespace Upp
