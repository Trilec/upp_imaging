#include "ImagingWorkbench.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

namespace Upp {

namespace {

struct PreviewMapping {
	Vector<int> rgb;
	int alpha = -1;
	String label;
	bool grayscale = false;
};

static int ClampByte(float value)
{
	if(!std::isfinite(value))
		value = 0.0f;
	value = std::clamp(value, 0.0f, 1.0f);
	return (int)(value * 255.0f + 0.5f);
}

static bool HasAny(const Vector<String>& names, const String& value)
{
	for(const String& name : names)
		if(name == value)
			return true;
	return false;
}

static int FindIndex(const Vector<String>& names, const String& value)
{
	for(int i = 0; i < names.GetCount(); ++i)
		if(names[i] == value)
			return i;
	return -1;
}

static String ShortChannelName(const String& name)
{
	int dot = name.ReverseFind('.');
	return dot >= 0 ? name.Mid(dot + 1) : name;
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

static PreviewMapping ResolvePreviewMapping(const OIIO::ImageSpec& spec,
	                                          const Vector<ImageChannelGroup>& groups,
	                                          String& choice)
{
	Vector<String> names;
	for(const std::string& n : spec.channelnames)
		names.Add(n.c_str());

	PreviewMapping map;
	choice.Clear();

	int r = FindIndex(names, "R");
	int g = FindIndex(names, "G");
	int b = FindIndex(names, "B");
	int a = FindIndex(names, "A");
	if(r >= 0 && g >= 0 && b >= 0) {
		map.rgb.Add(r);
		map.rgb.Add(g);
		map.rgb.Add(b);
		map.alpha = a;
		choice = a >= 0 ? "RGBA" : "RGB";
		return map;
	}

	for(const ImageChannelGroup& group : groups) {
		if(group.subimage != 0)
			continue;
		String prefix = group.name;
		if(prefix.IsEmpty())
			continue;
		int rg = FindIndex(names, prefix + ".R");
		int gg = FindIndex(names, prefix + ".G");
		int bg = FindIndex(names, prefix + ".B");
		if(rg >= 0 && gg >= 0 && bg >= 0) {
			map.rgb.Add(rg);
			map.rgb.Add(gg);
			map.rgb.Add(bg);
			int aa = FindIndex(names, prefix + ".A");
			map.alpha = aa;
			choice = aa >= 0 ? prefix + ".RGBA" : prefix + ".RGB";
			return map;
		}
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
	fit_view_button.WhenAction = [=] { canvas.SetFitMode(true); UpdateCanvasZoomLabel(); };
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
		info.pixel_type = spec.format.c_str();
		info.color_space = spec.get_string_attribute("oiio:ColorSpace", "").c_str();
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

	PreviewMapping mapping = ResolvePreviewMapping(spec, channel_groups, preview_choice);
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
	String color_space = spec.get_string_attribute("oiio:ColorSpace", "").c_str();
	if(color_space.IsEmpty())
		color_space = "(none)";

	String channel_text;
	Vector<String> channel_names;
	for(const std::string& name : spec.channelnames)
		channel_names.Add(name.c_str());
	channel_text = JoinChannels(channel_names);

	String source_type = spec.format.c_str();
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
			sub_item.right_text = Format("%d x %d / %s", spec.width, spec.height, spec.format.c_str());
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
	fit_view_button.Enable();
	SetStatus("Loaded: " + GetFileName(path));
}

} // namespace Upp
