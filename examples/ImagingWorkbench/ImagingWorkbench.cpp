#include "ImagingWorkbench.h"

#include <imaging_tone_conversion/imaging_tone_conversion.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <sstream>
#include <vector>

namespace Upp {

namespace {

static int ClampByte(float value)
{
	if(!std::isfinite(value))
		value = 0.0f;
	value = std::clamp(value, 0.0f, 1.0f);
	return (int)(value * 255.0f + 0.5f);
}

static String ShortChannelNameText(const String& name)
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

static bool ContainsString(const Vector<String>& values, const String& value)
{
	for(const String& item : values) {
		if(item == value)
			return true;
	}
	return false;
}

static void SetDropdownValues(UiDropdown& drop, const Vector<String>& values, const String& selected_value)
{
	drop.Clear();
	if(values.IsEmpty()) {
		drop.Add("none", String());
		drop.Select(0);
		return;
	}
	int selected_index = 0;
	for(int i = 0; i < values.GetCount(); ++i) {
		drop.Add(values[i], values[i]);
		if(values[i] == selected_value)
			selected_index = i;
	}
	drop.Select(selected_index);
}

static String ChannelViewName(ChannelView view)
{
	switch(view) {
	case ChannelView::Red:   return "R";
	case ChannelView::Green: return "G";
	case ChannelView::Blue:  return "B";
	case ChannelView::Alpha: return "A";
	default:                 return "RGB";
	}
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

static Size ComputeProxySize(Size source_size)
{
	if(source_size.cx <= 0 || source_size.cy <= 0)
		return Size();
	double long_edge_scale = 1.0;
	int long_edge = std::max(source_size.cx, source_size.cy);
	if(long_edge > 2048)
		long_edge_scale = 2048.0 / (double)long_edge;
	double area_scale = 1.0;
	double area = (double)source_size.cx * (double)source_size.cy;
	if(area > 2000000.0)
		area_scale = std::sqrt(2000000.0 / area);
	double scale = std::min(long_edge_scale, area_scale);
	scale = std::min(scale, 1.0);
	int w = std::max(1, (int)std::round(source_size.cx * scale));
	int h = std::max(1, (int)std::round(source_size.cy * scale));
	return Size(w, h);
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

static UiToolButton::Style MakeWorkbenchChannelToolStyle(Color face)
{
	UiToolButton::Style s = UiTheme::ResolveToolButton(UiRole::Standard);
	s.metrics.face_enabled = true;
	s.palette.face[ST_NORMAL] = UiFill::Solid(face);
	s.palette.face[ST_HOT] = UiFill::Solid(Blend(face, White(), 10));
	s.palette.face[ST_PRESSED] = UiFill::Solid(Blend(face, Black(), 14));
	s.palette.face[ST_DISABLED] = UiFill::Solid(face);
	s.metrics.frame_enabled = true;
	for(int i = 0; i < 4; ++i)
		s.palette.frame[i] = Color(82, 82, 82);
	s.palette.frame[ST_HOT] = Color(100, 100, 100);
	s.palette.frame[ST_PRESSED] = Color(96, 124, 171);
	s.palette.ink[ST_DISABLED] = Color(158, 158, 158);
	s.metrics.frame_width = DPI(2);
	s.metrics.radius = DPI(4);
	s.metrics.shadow.enabled = false;
	return s;
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
	rbg_tool.WhenAction = [=] { ApplyChannelView(ChannelView::RGB); };
	r_too.WhenAction = [=] { ApplyChannelView(ChannelView::Red); };
	g_tool.WhenAction = [=] { ApplyChannelView(ChannelView::Green); };
	b_tool.WhenAction = [=] { ApplyChannelView(ChannelView::Blue); };
	a_tool.WhenAction = [=] { ApplyChannelView(ChannelView::Alpha); };
	exposure_slider.WhenChanging = [=] { ApplyExposureStops(exposure_slider.GetValue(), false); };
	exposure_slider.WhenAction = [=] { ApplyExposureStops(exposure_slider.GetValue(), true); };
	exposure_float_edit.WhenAction = [=] { ApplyExposureStops(exposure_float_edit.GetValue(), true); };
	gamma_slider.WhenChanging = [=] { ApplyDisplayGamma(gamma_slider.GetValue(), false); };
	gamma_slider.WhenAction = [=] { ApplyDisplayGamma(gamma_slider.GetValue(), true); };
	gamma_float_edit.WhenAction = [=] { ApplyDisplayGamma(gamma_float_edit.GetValue(), true); };
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
	rbg_tool.SetCheckable(true);
	r_too.SetCheckable(true);
	g_tool.SetCheckable(true);
	b_tool.SetCheckable(true);
	a_tool.SetCheckable(true);
	rbg_tool.SetCustomStyle(MakeWorkbenchChannelToolStyle(Color(48, 48, 48)));
	r_too.SetCustomStyle(MakeWorkbenchChannelToolStyle(Color(48, 48, 48)));
	g_tool.SetCustomStyle(MakeWorkbenchChannelToolStyle(Color(48, 48, 48)));
	b_tool.SetCustomStyle(MakeWorkbenchChannelToolStyle(Color(48, 48, 48)));
	a_tool.SetCustomStyle(MakeWorkbenchChannelToolStyle(Color(48, 48, 48)));
	exposure_slider.SetRange(-4.0, 4.0).SetStep(0.1).SetValue(0.0);
	exposure_float_edit.MinMax(-4.0, 4.0).Step(0.1).Precision(1).ShowSpin(true);
	exposure_float_edit.SetValue(0.0);
	exposure_scale = 1.0f;
	gamma_slider.SetRange(0.5, 3.5).SetStep(0.1).SetValue(1.0);
	gamma_float_edit.MinMax(0.5, 3.5).Step(0.1).Precision(1).ShowSpin(true);
	gamma_float_edit.SetValue(1.0);
	OcioPreview::UpdateDisplayGamma(preview_gamma, display_gamma);

	ocio_body.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	ocio_enable_label.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	ocio_config_label.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	ocio_source_label.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	ocio_display_label.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	ocio_view_label.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	ocio_error.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	ocio_body.SetText("OCIO display preview").SetAlign(UiAlign::LEFT, UiAlign::CENTER);
	ocio_enable_label.SetText("Enabled").NoWantFocus().IgnoreMouse();
	ocio_config_label.SetText("Config").NoWantFocus().IgnoreMouse();
	ocio_source_label.SetText("Source").NoWantFocus().IgnoreMouse();
	ocio_display_label.SetText("Display").NoWantFocus().IgnoreMouse();
	ocio_view_label.SetText("View").NoWantFocus().IgnoreMouse();
	ocio_error.SetText("OCIO preview is off.").SetAlign(UiAlign::LEFT, UiAlign::CENTER);
	ocio_enable_drop.Add("Off", 0);
	ocio_enable_drop.Add("On", 1);
	ocio_enable_drop.Select(0);
	ocio_config_drop.Add("Off / none", String());
	for(const String& name : OcioPreview::GetBuiltinConfigNames())
		ocio_config_drop.Add(name, name);
	ocio_config_drop.Select(0);
	ocio_source_drop.Add("none", String());
	ocio_source_drop.Select(0);
	ocio_display_drop.Add("none", String());
	ocio_display_drop.Select(0);
	ocio_view_drop.Add("none", String());
	ocio_view_drop.Select(0);

	layers_layout.SetDirection(UiDirection::V).SetGap(DPI(6), DPI(6)).SetInset(DPI(8)).SetWrap(UiBoxWrap::None);
	layers_summary.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	layers_detail.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	layers_tree.SetCustomStyle(UiTheme::ResolveTree());
	layers_tree.SetRootVisible(false);
	layers_tree.SetSelectionMode(UITREESEL_SINGLE);
	layers_tree.ShowConnectorLines(true);
	layers_tree.ShowMetadataMarker(true);
	layers_tree.WhenSelection = [=] { UpdatePreviewSelection(); };
	layers_layout.Add(layers_summary).Fit().MinCross(DPI(0));
	layers_layout.Add(layers_tree).Expand(1).MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	layers_layout.Add(layers_detail).Fit().MinCross(DPI(0));
	pageA.Add(layers_layout.SizePos());

	ocio_layout.SetDirection(UiDirection::V).SetGap(DPI(6), DPI(6)).SetInset(DPI(8)).SetWrap(UiBoxWrap::None);
	ocio_layout.Add(ocio_body).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	ocio_layout.Add(ocio_enable_label).Fit().MinCross(DPI(0));
	ocio_layout.Add(ocio_enable_drop).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	ocio_layout.Add(ocio_config_label).Fit().MinCross(DPI(0));
	ocio_layout.Add(ocio_config_drop).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	ocio_layout.Add(ocio_source_label).Fit().MinCross(DPI(0));
	ocio_layout.Add(ocio_source_drop).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	ocio_layout.Add(ocio_display_label).Fit().MinCross(DPI(0));
	ocio_layout.Add(ocio_display_drop).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	ocio_layout.Add(ocio_view_label).Fit().MinCross(DPI(0));
	ocio_layout.Add(ocio_view_drop).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	ocio_layout.Add(ocio_error).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	pageB.Add(ocio_layout.SizePos());

	analysis_body.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	analysis_body.SetText("Histogram and pixel analysis will be connected after viewer controls.")
		.SetAlign(UiAlign::LEFT, UiAlign::CENTER);
	pageC.Add(analysis_body.SizePos());

	canvas.SetPlaceholderText("Open an EXR or PNG to begin");
	canvas.WhenSourcePixelMove = [=](Point p) { UpdateProbe(p); };
	canvas.WhenSourcePixelLeave = [=] { ClearProbe(); };
	canvas_scroll_panel.Content().Add(canvas.SizePos());

	ocio_enable_drop.WhenSelectData = [=](const Value&) { UpdateOcioControls(); };
	ocio_config_drop.WhenSelectData = [=](const Value&) { UpdateOcioControls(); };
	ocio_source_drop.WhenSelectData = [=](const Value&) { UpdateOcioControls(); };
	ocio_display_drop.WhenSelectData = [=](const Value&) { UpdateOcioControls(); };
	ocio_view_drop.WhenSelectData = [=](const Value&) { UpdateOcioControls(); };

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
	xy_info.SetText("—");
	color_info.SetText("—");
	status.SetText("Open an EXR or PNG to begin");
	layers_summary.SetText("Open an EXR or PNG to inspect channels and subimages.");
	layers_detail.SetText("No image loaded.");
	right_tab.SetActiveTab(0);
	UpdateOcioControls();
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

String ImagingWorkbench::DescribeChannelView() const
{
	return ChannelViewName(channel_view);
}

String ImagingWorkbench::DescribeSelectedGroup() const
{
	if(selected_preview_group < 0 || selected_preview_group >= preview_groups.GetCount())
		return String();
	const PreviewGroup& group = preview_groups[selected_preview_group];
	String text = group.name;
	if(text.IsEmpty())
		text = group.channels_text;
	if(text.IsEmpty())
		text = "Preview";
	return text;
}

String ImagingWorkbench::DescribePreviewChoice() const
{
	if(selected_preview_group < 0 || selected_preview_group >= preview_groups.GetCount())
		return String("Preview: none");
	const PreviewGroup& group = preview_groups[selected_preview_group];
	String out = group.name.IsEmpty() ? group.channels_text : group.name;
	if(out.IsEmpty())
		out = "Preview";
	out << " / " << DescribeChannelView();
	return out;
}

String ImagingWorkbench::GetImageColorSpace() const
{
	if(!source_image.initialized())
		return String();
	return StringFromView(source_image.spec().get_string_attribute("oiio:ColorSpace"));
}

String ImagingWorkbench::GetOcioSummary() const
{
	if(!ocio_enabled)
		return "OCIO: off";
	const String config_name = ocio_config_name;
	if(config_name.IsEmpty())
		return "OCIO: on / none";
	String source = ocio_source_name;
	if(!source.IsEmpty())
		source << (ocio_source_from_metadata ? " (metadata)" : " (default)");
	return Format("OCIO: %s / Source: %s / %s / %s", config_name, source, ocio_display_name, ocio_view_name);
}

void ImagingWorkbench::UpdateOcioControls()
{
	if(ocio_controls_updating)
		return;
	ocio_controls_updating = true;

	ocio_enabled = (int)ocio_enable_drop.GetSelectedData() != 0;
	ocio_config_drop.Enable(ocio_enabled);
	ocio_source_drop.Enable(false);
	ocio_display_drop.Enable(false);
	ocio_view_drop.Enable(false);
	ocio_config_name = AsString(ocio_config_drop.GetSelectedData());
	ocio_source_name.Clear();
	ocio_display_name.Clear();
	ocio_view_name.Clear();
	ocio_source_from_metadata = false;
	ocio_error_text.Clear();
	ocio_processor.Clear();
	ocio_processor_valid = false;

	if(!ocio_enabled) {
		SetDropdownValues(ocio_source_drop, Vector<String>(), String());
		SetDropdownValues(ocio_display_drop, Vector<String>(), String());
		SetDropdownValues(ocio_view_drop, Vector<String>(), String());
		ocio_error.SetText("OCIO preview is off.");
	}
	else if(!source_image.initialized()) {
		SetDropdownValues(ocio_source_drop, Vector<String>(), String());
		SetDropdownValues(ocio_display_drop, Vector<String>(), String());
		SetDropdownValues(ocio_view_drop, Vector<String>(), String());
		ocio_error_text = "Load an image before enabling OCIO preview.";
		ocio_error.SetText(ocio_error_text);
	}
	else if(ocio_config_name.IsEmpty()) {
		SetDropdownValues(ocio_source_drop, Vector<String>(), String());
		SetDropdownValues(ocio_display_drop, Vector<String>(), String());
		SetDropdownValues(ocio_view_drop, Vector<String>(), String());
		ocio_error_text = "OCIO preview config is not selected.";
		ocio_error.SetText(ocio_error_text);
	}
	else {
		String load_error;
		OCIO::ConstConfigRcPtr config;
		if(!OcioPreview::LoadBuiltinConfig(ocio_config_name, config, load_error)) {
			ocio_error_text = load_error;
			ocio_error.SetText(ocio_error_text);
		}
		else {
			Vector<String> source_names = OcioPreview::GetColorSpaceNames(config);
			String image_color_space = GetImageColorSpace();
			String source = image_color_space;
			ocio_source_from_metadata = ContainsString(source_names, image_color_space);
			if(!ocio_source_from_metadata)
				source = OcioPreview::GetDefaultSourceColorSpace(config);
			if(source.IsEmpty() && !source_names.IsEmpty())
				source = source_names[0];
			SetDropdownValues(ocio_source_drop, source_names, source);
			ocio_source_name = AsString(ocio_source_drop.GetSelectedData());
			if(ocio_source_name.IsEmpty() && !source_names.IsEmpty())
				ocio_source_name = source_names[0];

			Vector<String> display_names = OcioPreview::GetDisplayNames(config);
			String display = OcioPreview::GetDefaultDisplay(config);
			if(display.IsEmpty() && !display_names.IsEmpty())
				display = display_names[0];
			SetDropdownValues(ocio_display_drop, display_names, display);
			ocio_display_name = AsString(ocio_display_drop.GetSelectedData());
			if(ocio_display_name.IsEmpty() && !display_names.IsEmpty())
				ocio_display_name = display_names[0];

			Vector<String> view_names = OcioPreview::GetViewNames(config, ocio_display_name);
			String view = OcioPreview::GetDefaultView(config, ocio_display_name);
			if(view.IsEmpty() && !view_names.IsEmpty())
				view = view_names[0];
			SetDropdownValues(ocio_view_drop, view_names, view);
			ocio_view_name = AsString(ocio_view_drop.GetSelectedData());
			if(ocio_view_name.IsEmpty() && !view_names.IsEmpty())
				ocio_view_name = view_names[0];

			using Clock = std::chrono::steady_clock;
			auto processor_started = Clock::now();
			ocio_processor_valid = ocio_processor.Update(config, ocio_config_name, ocio_source_name, ocio_display_name, ocio_view_name, String(), String(), ocio_error_text);
			preview_timing.processor_ms = std::chrono::duration<double, std::milli>(Clock::now() - processor_started).count();
			if(!ocio_processor_valid)
				ocio_error.SetText(ocio_error_text);
			else {
				ocio_error_text.Clear();
				ocio_error.SetText("OCIO preview ready.");
			}
		}
		ocio_source_drop.Enable(ocio_processor_valid);
		ocio_display_drop.Enable(ocio_processor_valid);
		ocio_view_drop.Enable(ocio_processor_valid);
	}

	ocio_body.SetText(GetOcioSummary());
	ocio_controls_updating = false;
	UpdateSelectionSummary();
	SchedulePreviewRender(true);
}

String ImagingWorkbench::FormatProbeValue(float value) const
{
	if(!std::isfinite(value))
		return String("—");
	return Format("%.3f", (double)value);
}

bool ImagingWorkbench::IsViewAvailable(ChannelView view) const
{
	if(selected_preview_group < 0 || selected_preview_group >= preview_groups.GetCount())
		return false;
	const PreviewGroup& group = preview_groups[selected_preview_group];
	switch(view) {
	case ChannelView::RGB:
		return group.HasRGB() || group.HasSingle();
	case ChannelView::Red:
	case ChannelView::Green:
	case ChannelView::Blue:
		return group.HasRGB();
	case ChannelView::Alpha:
		return group.HasAlpha();
	}
	return false;
}

void ImagingWorkbench::UpdateViewerControls()
{
	bool have_group = selected_preview_group >= 0 && selected_preview_group < preview_groups.GetCount();
	PreviewGroup group;
	if(have_group)
		group = preview_groups[selected_preview_group];

	rbg_tool.Enable(have_group && IsViewAvailable(ChannelView::RGB));
	r_too.Enable(have_group && IsViewAvailable(ChannelView::Red));
	g_tool.Enable(have_group && IsViewAvailable(ChannelView::Green));
	b_tool.Enable(have_group && IsViewAvailable(ChannelView::Blue));
	a_tool.Enable(have_group && IsViewAvailable(ChannelView::Alpha));
	exposure.Enable(have_group);
	exposure_slider.Enable(have_group);
	exposure_float_edit.Enable(have_group);
	gamma.Enable(have_group);
	gamma_slider.Enable(have_group);
	gamma_float_edit.Enable(have_group);
	fit_view_button.Enable(canvas.HasImage());

	if(!have_group)
		channel_view = ChannelView::RGB;
	else if(!IsViewAvailable(channel_view)) {
		if(group.HasRGB() || group.HasSingle())
			channel_view = ChannelView::RGB;
		else if(group.HasAlpha())
			channel_view = ChannelView::Alpha;
	}

	syncing_view_controls = true;
	rbg_tool.SetChecked(channel_view == ChannelView::RGB);
	r_too.SetChecked(channel_view == ChannelView::Red);
	g_tool.SetChecked(channel_view == ChannelView::Green);
	b_tool.SetChecked(channel_view == ChannelView::Blue);
	a_tool.SetChecked(channel_view == ChannelView::Alpha);
	exposure_slider.SetValue(exposure_stops);
	exposure_float_edit.SetValue(exposure_stops);
	gamma_slider.SetValue(display_gamma);
	gamma_float_edit.SetValue(display_gamma);
	syncing_view_controls = false;
}

void ImagingWorkbench::ApplyChannelView(ChannelView view)
{
	if(syncing_view_controls || channel_view == view)
		return;
	channel_view = view;
	syncing_view_controls = true;
	rbg_tool.SetChecked(channel_view == ChannelView::RGB);
	r_too.SetChecked(channel_view == ChannelView::Red);
	g_tool.SetChecked(channel_view == ChannelView::Green);
	b_tool.SetChecked(channel_view == ChannelView::Blue);
	a_tool.SetChecked(channel_view == ChannelView::Alpha);
	syncing_view_controls = false;
	SchedulePreviewRender(true);
	UpdateSelectionSummary();
	SetStatus("Preview: " + DescribePreviewChoice());
}

void ImagingWorkbench::ApplyExposureStops(double value, bool immediate)
{
	value = std::clamp(value, -4.0, 4.0);
	if(syncing_view_controls || exposure_stops == value)
		return;
	exposure_stops = value;
	exposure_scale = (float)std::pow(2.0, exposure_stops);
	syncing_view_controls = true;
	exposure_slider.SetValue(exposure_stops);
	exposure_float_edit.SetValue(exposure_stops);
	syncing_view_controls = false;
	SchedulePreviewRender(immediate);
}

void ImagingWorkbench::ApplyDisplayGamma(double value, bool immediate)
{
	value = std::clamp(value, 0.5, 3.5);
	if(syncing_view_controls || display_gamma == value)
		return;
	display_gamma = value;
	OcioPreview::UpdateDisplayGamma(preview_gamma, display_gamma);
	syncing_view_controls = true;
	gamma_slider.SetValue(display_gamma);
	gamma_float_edit.SetValue(display_gamma);
	syncing_view_controls = false;
	SchedulePreviewRender(immediate);
}

void ImagingWorkbench::ClearProbe()
{
	xy_info.SetText("—");
	color_info.SetText("—");
}

void ImagingWorkbench::UpdateProbe(Point image_point)
{
	if(selected_preview_group < 0 || selected_preview_group >= preview_groups.GetCount()) {
		ClearProbe();
		return;
	}

	const OIIO::ImageSpec& spec = source_image.spec();
	if(spec.width <= 0 || spec.height <= 0 || spec.nchannels <= 0) {
		ClearProbe();
		return;
	}

	probe_source_pixel.SetCount(spec.nchannels);
	if(image_point.x < 0 || image_point.y < 0 || image_point.x >= spec.width || image_point.y >= spec.height) {
		ClearProbe();
		return;
	}

	source_image.getpixel(image_point.x, image_point.y, 0, OIIO::span<float>(probe_source_pixel.Begin(), probe_source_pixel.GetCount()));
	const PreviewGroup& group = preview_groups[selected_preview_group];
	auto channel_at = [&](int index) -> float {
		return index >= 0 && index < probe_source_pixel.GetCount() ? probe_source_pixel[index] : 0.0f;
	};
	float src_r = group.HasRGB() ? channel_at(group.red) : channel_at(group.single_channel);
	float src_g = group.HasRGB() ? channel_at(group.green) : channel_at(group.single_channel);
	float src_b = group.HasRGB() ? channel_at(group.blue) : channel_at(group.single_channel);
	float src_a = group.HasAlpha() ? channel_at(group.alpha) : 1.0f;

	float tone_r = ApplyToneExposureGamma(src_r, exposure_stops, display_gamma);
	float tone_g = ApplyToneExposureGamma(src_g, exposure_stops, display_gamma);
	float tone_b = ApplyToneExposureGamma(src_b, exposure_stops, display_gamma);
	float out_r = tone_r;
	float out_g = tone_g;
	float out_b = tone_b;
	float out_a = src_a;

	switch(channel_view) {
	case ChannelView::Red:
		out_r = out_g = out_b = tone_r;
		break;
	case ChannelView::Green:
		out_r = out_g = out_b = tone_g;
		break;
	case ChannelView::Blue:
		out_r = out_g = out_b = tone_b;
		break;
	case ChannelView::Alpha:
		out_r = out_g = out_b = src_a;
		out_a = src_a;
		break;
	case ChannelView::RGB:
	default:
		if(group.HasSingle())
			out_r = out_g = out_b = ApplyToneExposureGamma(channel_at(group.single_channel), exposure_stops, display_gamma);
		break;
	}

	xy_info.SetText(Format("%d, %d", image_point.x, image_point.y));
	color_info.SetText(Format("%s, %s, %s, %s",
		FormatProbeValue(out_r),
		FormatProbeValue(out_g),
		FormatProbeValue(out_b),
		FormatProbeValue(out_a)));
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
	if(selected_preview_group < 0 || selected_preview_group >= preview_groups.GetCount()) {
		layers_detail.SetText("No preview group selected.");
		return;
	}

	const PreviewGroup& group = preview_groups[selected_preview_group];
	String text;
	text << "Selected: " << DescribeSelectedGroup() << "\n";
	if(group.subimage >= 0 && group.subimage < subimages.GetCount()) {
		const ImageSubimageInfo& info = subimages[group.subimage];
		text << Format("Subimage %d: %d x %d / %s / %d channels\n",
			group.subimage, info.size.cx, info.size.cy, info.pixel_type, info.channel_count);
	}
	text << "Group channels: " << group.channels_text << "\n";
	text << "View: " << DescribeChannelView() << "\n";
	text << Format("Exposure: %.1f stops\n", exposure_stops);
	text << Format("Gamma: %.1f", display_gamma);
	layers_detail.SetText(text);
}

void ImagingWorkbench::UpdatePreviewSelection()
{
	UiTreeNodeRef node = layers_tree.GetCursor();
	if(!node.IsValid()) {
		selected_preview_group = -1;
		UpdateViewerControls();
		canvas.ClearImage();
		UpdateSelectionSummary();
		return;
	}

	const UiModelItem& item = layers_tree.GetModel().Get(node);
	int index = IsNull(item.data) ? -1 : (int)item.data;
	if(index < 0 || index >= preview_groups.GetCount())
		return;

	if(selected_preview_group != index) {
		selected_preview_group = index;
		UpdateViewerControls();
		SchedulePreviewRender(true);
	}
	UpdateSelectionSummary();
	SetStatus("Preview: " + DescribePreviewChoice());
}

void ImagingWorkbench::RecordTiming(const String& phase, double milliseconds, const PreviewProxy* proxy)
{
	String text = Format("%s %.1f ms", phase, milliseconds);
	if(proxy && proxy->IsValid())
		text << Format(" | proxy %d x %d (%d ch, %.1f MB)", proxy->proxy_size.cx, proxy->proxy_size.cy, proxy->channel_count,
			(double)proxy->pixels.GetCount() * sizeof(float) / (1024.0 * 1024.0));
	timing_summary = text;
	SetStatus(text);
}

void ImagingWorkbench::SchedulePreviewRender(bool immediate)
{
	if(immediate) {
		preview_render_coalescer.RequestImmediate();
		RenderPreviewFromProxy();
		UpdateSelectionSummary();
		SetStatus("Preview: " + DescribePreviewChoice());
		return;
	}
	if(!preview_render_coalescer.RequestDeferred())
		return;
	int callback_id = preview_render_coalescer.scheduled_id;
	SetTimeCallback(24, [this, callback_id] {
		bool render_now = false;
		preview_render_coalescer.ShouldRunCallback(callback_id, render_now);
		if(render_now)
			RenderPreviewFromProxy();
	});
}

void ImagingWorkbench::BuildSelectedGroupProxy()
{
	if(selected_preview_group < 0 || selected_preview_group >= preview_groups.GetCount())
		return;

	const PreviewGroup& group = preview_groups[selected_preview_group];
	Size source_size(source_image.spec().width, source_image.spec().height);
	if(source_size.cx <= 0 || source_size.cy <= 0)
		return;

	Size proxy_size = ComputeProxySize(source_size);
	if(proxy_size.cx <= 0 || proxy_size.cy <= 0)
		return;

	for(int i = 0; i < proxy_cache.GetCount(); ++i) {
		PreviewProxy& cached = proxy_cache[i];
		if(cached.group_index == selected_preview_group && cached.source_size == source_size && cached.proxy_size == proxy_size) {
			if(i > 0)
				proxy_cache.Swap(0, i);
			return;
		}
	}

	std::vector<int> channel_order;
	channel_order.reserve(4);
	PreviewProxy proxy;
	proxy.group_index = selected_preview_group;
	proxy.source_size = source_size;
	proxy.proxy_size = proxy_size;
	proxy.red = group.red;
	proxy.green = group.green;
	proxy.blue = group.blue;
	proxy.alpha = group.alpha;
	proxy.single_channel = group.single_channel;
	proxy.has_alpha = group.HasAlpha();

	if(group.HasRGB()) {
		channel_order.push_back(group.red);
		channel_order.push_back(group.green);
		channel_order.push_back(group.blue);
		proxy.channel_count = 3;
		if(group.HasAlpha()) {
			channel_order.push_back(group.alpha);
			proxy.channel_count = 4;
		}
	}
	else if(group.HasSingle()) {
		channel_order.push_back(group.single_channel);
		proxy.channel_count = 1;
		if(group.HasAlpha()) {
			channel_order.push_back(group.alpha);
			proxy.channel_count = 2;
		}
	}
	else if(group.HasAlpha()) {
		channel_order.push_back(group.alpha);
		proxy.channel_count = 1;
	}

	if(channel_order.empty())
		return;

	using Clock = std::chrono::steady_clock;
	auto started = Clock::now();

	OIIO::ImageBuf selected = OIIO::ImageBufAlgo::channels(source_image, (int)channel_order.size(),
		OIIO::cspan<int>(channel_order.data(), (int)channel_order.size()));
	if(selected.has_error()) {
		last_error = selected.geterror();
		return;
	}

	OIIO::ImageSpec proxy_spec(proxy_size.cx, proxy_size.cy, (int)channel_order.size(), OIIO::TypeDesc::FLOAT);
	proxy_spec.channelnames.clear();
	proxy_spec.alpha_channel = group.HasAlpha() ? (int)channel_order.size() - 1 : -1;
	proxy_spec.z_channel = -1;
	proxy_spec.channelformats.clear();
	for(int i = 0; i < (int)channel_order.size(); ++i)
		proxy_spec.channelnames.push_back(source_image.spec().channelnames[channel_order[i]]);
	OIIO::ImageBuf resized(proxy_spec);
	if(!OIIO::ImageBufAlgo::resize(resized, selected, { { "filtername", "triangle" } })) {
		last_error = resized.geterror();
		return;
	}

	proxy.pixels.SetCount((size_t)proxy_size.cx * proxy_size.cy * proxy.channel_count);
	if(!resized.get_pixels(resized.roi(), OIIO::TypeDesc::FLOAT, proxy.pixels.Begin(),
				       proxy.channel_count * sizeof(float), proxy_size.cx * proxy.channel_count * sizeof(float), OIIO::AutoStride)) {
		last_error = resized.geterror();
		return;
	}

	auto elapsed = std::chrono::duration<double, std::milli>(Clock::now() - started).count();
	proxy_cache.Add(pick(proxy));
	int last = proxy_cache.GetCount() - 1;
	if(last > 0)
		proxy_cache.Swap(0, last);
	if(proxy_cache.GetCount() > 2)
		proxy_cache.Remove(2);
	RecordTiming("proxy build", elapsed, &proxy_cache[0]);
}

void ImagingWorkbench::RenderPreviewFromProxy()
{
	if(selected_preview_group < 0 || selected_preview_group >= preview_groups.GetCount()) {
		canvas.ClearImage();
		return;
	}

	BuildSelectedGroupProxy();
	const PreviewProxy* proxy = nullptr;
	for(const PreviewProxy& cached : proxy_cache) {
		if(cached.group_index == selected_preview_group) {
			proxy = &cached;
			break;
		}
	}
	if(!proxy || !proxy->IsValid()) {
		canvas.ClearImage();
		return;
	}

	using Clock = std::chrono::steady_clock;
	auto started = Clock::now();

	preview_image = Image();
	ImageBuffer buffer(Size(proxy->proxy_size.cx, proxy->proxy_size.cy));
	const float* proxy_pixels = proxy->pixels.Begin();
	const PreviewGroup& group = preview_groups[selected_preview_group];
	const bool use_ocio = ocio_enabled && ocio_processor_valid && group.HasRGB() && channel_view != ChannelView::Alpha;
	preview_timing.buffer_ms = 0.0;
	preview_timing.ocio_ms = 0.0;
	preview_timing.convert_ms = 0.0;
	preview_timing.publish_ms = 0.0;
	OcioPreview::UpdateDisplayGamma(preview_gamma, display_gamma);

	if(use_ocio) {
		preview_pixels.SetCount((size_t)proxy->proxy_size.cx * proxy->proxy_size.cy * 4);
		auto buffer_started = Clock::now();
		for(int y = 0; y < proxy->proxy_size.cy; ++y) {
			for(int x = 0; x < proxy->proxy_size.cx; ++x) {
				const float* src = proxy_pixels + ((size_t)y * proxy->proxy_size.cx + x) * proxy->channel_count;
				auto* dst = preview_pixels.Begin() + ((size_t)y * proxy->proxy_size.cx + x) * 4;
				float src_r = src[group.red];
				float src_g = src[group.green];
				float src_b = src[group.blue];
				dst[0] = OcioPreview::ApplySceneExposure(src_r, exposure_scale);
				dst[1] = OcioPreview::ApplySceneExposure(src_g, exposure_scale);
				dst[2] = OcioPreview::ApplySceneExposure(src_b, exposure_scale);
				dst[3] = group.HasAlpha() ? src[group.alpha] : 1.0f;
			}
		}
		preview_timing.buffer_ms = std::chrono::duration<double, std::milli>(Clock::now() - buffer_started).count();
		auto ocio_started = Clock::now();
		String ocio_preview_error;
		if(!OcioPreview::ApplyOcioProcessor(ocio_processor.cpu, preview_pixels.Begin(), proxy->proxy_size.cx, proxy->proxy_size.cy, 4, ocio_preview_error)) {
			ocio_error_text = ocio_preview_error;
			ocio_error.SetText(ocio_error_text);
		}
		else {
			ocio_error_text.Clear();
			ocio_error.SetText(GetOcioSummary());
		}
		preview_timing.ocio_ms = std::chrono::duration<double, std::milli>(Clock::now() - ocio_started).count();
	}
	else
		preview_pixels.Clear();

	auto convert_started = Clock::now();
	auto gamma_byte = [&](float value) -> byte {
		return OcioPreview::ApplyDisplayGammaToByte(value, preview_gamma);
	};
	auto render_row = [&](int y) {
		for(int x = 0; x < proxy->proxy_size.cx; ++x) {
			const size_t index = (size_t)y * proxy->proxy_size.cx + x;
			byte out_r = 0;
			byte out_g = 0;
			byte out_b = 0;
			byte out_a = 255;
			if(use_ocio) {
				const float* src = preview_pixels.Begin() + index * 4;
				float src_a = src[3];
				out_a = ClampByte(src_a);
				switch(channel_view) {
				case ChannelView::Red:
					out_r = out_g = out_b = gamma_byte(src[0]);
					break;
				case ChannelView::Green:
					out_r = out_g = out_b = gamma_byte(src[1]);
					break;
				case ChannelView::Blue:
					out_r = out_g = out_b = gamma_byte(src[2]);
					break;
				case ChannelView::Alpha:
					out_r = out_g = out_b = ClampByte(src_a);
					out_a = 255;
					break;
				case ChannelView::RGB:
				default:
					out_r = gamma_byte(src[0]);
					out_g = gamma_byte(src[1]);
					out_b = gamma_byte(src[2]);
					break;
				}
			}
			else {
				const float* src = proxy_pixels + index * proxy->channel_count;
				float src_a = proxy->has_alpha ? src[proxy->channel_count - 1] : 1.0f;
				out_a = ClampByte(src_a);
				float src_r = proxy->channel_count >= 3 ? src[0] : src[0];
				float src_g = proxy->channel_count >= 3 ? src[1] : src[0];
				float src_b = proxy->channel_count >= 3 ? src[2] : src[0];
				float exp_r = OcioPreview::ApplySceneExposure(src_r, exposure_scale);
				float exp_g = OcioPreview::ApplySceneExposure(src_g, exposure_scale);
				float exp_b = OcioPreview::ApplySceneExposure(src_b, exposure_scale);
				float exp_gray = OcioPreview::ApplySceneExposure(src[0], exposure_scale);
				switch(channel_view) {
				case ChannelView::Red:
					out_r = out_g = out_b = gamma_byte(exp_r);
					break;
				case ChannelView::Green:
					out_r = out_g = out_b = gamma_byte(exp_g);
					break;
				case ChannelView::Blue:
					out_r = out_g = out_b = gamma_byte(exp_b);
					break;
				case ChannelView::Alpha:
					out_r = out_g = out_b = ClampByte(src_a);
					out_a = 255;
					break;
				case ChannelView::RGB:
				default:
					if(proxy->channel_count == 1 || proxy->channel_count == 2) {
						byte gray = gamma_byte(exp_gray);
						out_r = out_g = out_b = gray;
					}
					else {
						out_r = gamma_byte(exp_r);
						out_g = gamma_byte(exp_g);
						out_b = gamma_byte(exp_b);
					}
					break;
				}
			}

			RGBA& dst = buffer[y][x];
			dst.r = out_r;
			dst.g = out_g;
			dst.b = out_b;
			dst.a = out_a;
			}
		};
	int pixel_count = proxy->proxy_size.cx * proxy->proxy_size.cy;
#ifndef _DEBUG
	if(pixel_count >= 1200000) {
		OIIO::parallel_for((int64)0, (int64)proxy->proxy_size.cy, [&](int64 y) { render_row((int)y); }, OIIO::paropt(0, OIIO::paropt::SplitDir::Y, 1));
	}
	else {
		for(int y = 0; y < proxy->proxy_size.cy; ++y)
			render_row(y);
	}
#else
		for(int y = 0; y < proxy->proxy_size.cy; ++y)
			render_row(y);
#endif
	preview_timing.convert_ms = std::chrono::duration<double, std::milli>(Clock::now() - convert_started).count();
	auto publish_started = Clock::now();

	preview_image = buffer;
	canvas.SetDisplayImage(preview_image, proxy->source_size);
	UpdateCanvasZoomLabel();
	preview_timing.publish_ms = std::chrono::duration<double, std::milli>(Clock::now() - publish_started).count();
	auto elapsed = std::chrono::duration<double, std::milli>(Clock::now() - started).count();
	preview_timing.total_ms = elapsed;
	RecordTiming(channel_view == ChannelView::RGB ? (ocio_enabled ? "OCIO RGB render" : "RGB render") : channel_view == ChannelView::Red ? (ocio_enabled ? "OCIO R render" : "R render") : channel_view == ChannelView::Green ? (ocio_enabled ? "OCIO G render" : "G render") : channel_view == ChannelView::Blue ? (ocio_enabled ? "OCIO B render" : "B render") : "A render", elapsed, proxy);
}

void ImagingWorkbench::ScanSourceMetadata()
{
	preview_groups.Clear();
	selected_preview_group = -1;
	proxy_cache.Clear();
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

		if(subimage == 0) {
			Vector<String> names;
			for(const std::string& name : spec.channelnames)
				names.Add(name.c_str());

			struct TempGroup : Moveable<TempGroup> {
				String name;
				String channels_text;
				int subimage = 0;
				int channel_count = 0;
				int red = -1;
				int green = -1;
				int blue = -1;
				int alpha = -1;
				int single_channel = -1;
			};

			Vector<TempGroup> prefix_groups;
			auto find_or_add_prefix = [&](const String& prefix) -> int {
				for(int i = 0; i < prefix_groups.GetCount(); ++i)
					if(prefix_groups[i].name == prefix)
						return i;
				TempGroup group;
				group.name = prefix;
				group.subimage = subimage;
				prefix_groups.Add(group);
				return prefix_groups.GetCount() - 1;
			};

			for(int i = 0; i < names.GetCount(); ++i) {
				String prefix = GroupPrefix(names[i]);
				if(prefix.IsEmpty())
					continue;
				String leaf = ShortChannelNameText(names[i]);
				TempGroup& group = prefix_groups[find_or_add_prefix(prefix)];
				if(!group.channels_text.IsEmpty())
					group.channels_text << ' ';
				group.channels_text << leaf;
				++group.channel_count;
				String upper_leaf = ToUpper(leaf);
				if(upper_leaf == "R") group.red = i;
				else if(upper_leaf == "G") group.green = i;
				else if(upper_leaf == "B") group.blue = i;
				else if(IsAlphaChannelName(leaf)) group.alpha = i;
				else if(group.single_channel < 0) group.single_channel = i;
			}

			auto find_unprefixed_leaf = [&](const String& leaf) -> int {
				for(int i = 0; i < names.GetCount(); ++i)
					if(GroupPrefix(names[i]).IsEmpty() && ToUpper(ShortChannelNameText(names[i])) == ToUpper(leaf))
						return i;
				return -1;
			};

			int r = find_unprefixed_leaf("R");
			int g = find_unprefixed_leaf("G");
			int b = find_unprefixed_leaf("B");
			int a = -1;
			if(spec.alpha_channel >= 0 && spec.alpha_channel < names.GetCount() && GroupPrefix(names[spec.alpha_channel]).IsEmpty())
				a = spec.alpha_channel;
			if(a < 0)
				a = find_unprefixed_leaf("A");
			if(a < 0)
				a = find_unprefixed_leaf("ALPHA");

			if(r >= 0 && g >= 0 && b >= 0) {
				PreviewGroup group;
				group.subimage = subimage;
				group.red = r;
				group.green = g;
				group.blue = b;
				group.alpha = a;
				group.channel_count = 3 + (a >= 0 ? 1 : 0);
				group.channels_text = a >= 0 ? "R G B A" : "R G B";
				group.name = a >= 0 ? "RGBA" : "RGB";
				preview_groups.Add(group);
			}

			Vector<int> color_left;
			Vector<int> alpha_left;
			Vector<int> used;
			used.SetCount(names.GetCount(), 0);
			if(r >= 0) used[r] = 1;
			if(g >= 0) used[g] = 1;
			if(b >= 0) used[b] = 1;
			if(a >= 0) used[a] = 1;
			for(int i = 0; i < names.GetCount(); ++i) {
				if(used[i] || !GroupPrefix(names[i]).IsEmpty())
					continue;
				if(IsAlphaChannelName(ShortChannelNameText(names[i])))
					alpha_left.Add(i);
				else
					color_left.Add(i);
			}

			if(color_left.GetCount() == 1 && alpha_left.GetCount() >= 1) {
				int idx = color_left[0];
				PreviewGroup group;
				group.subimage = subimage;
				group.single_channel = idx;
				group.alpha = alpha_left[0];
				group.channel_count = 2;
				group.channels_text = ShortChannelNameText(names[idx]) + " " + ShortChannelNameText(names[alpha_left[0]]);
				group.name = ShortChannelNameText(names[idx]) + " + Alpha";
				preview_groups.Add(group);
				color_left.Clear();
				alpha_left.Clear();
			}

			for(int idx : color_left) {
				PreviewGroup group;
				group.subimage = subimage;
				group.single_channel = idx;
				group.channel_count = 1;
				group.channels_text = ShortChannelNameText(names[idx]);
				group.name = group.channels_text;
				preview_groups.Add(group);
			}

			if(color_left.IsEmpty() && alpha_left.GetCount() == 1) {
				int idx = alpha_left[0];
				PreviewGroup group;
				group.subimage = subimage;
				group.single_channel = idx;
				group.alpha = idx;
				group.channel_count = 1;
				group.channels_text = ShortChannelNameText(names[idx]);
				group.name = group.channels_text;
				preview_groups.Add(group);
			}

			for(const TempGroup& group : prefix_groups) {
				bool color_like = group.red >= 0 && group.green >= 0 && group.blue >= 0;
				for(int i = 0; i < names.GetCount(); ++i) {
					if(GroupPrefix(names[i]) != group.name)
						continue;
					String upper_leaf = ToUpper(ShortChannelNameText(names[i]));
					if(upper_leaf != "R" && upper_leaf != "G" && upper_leaf != "B" && upper_leaf != "A" && upper_leaf != "ALPHA") {
						color_like = false;
						break;
					}
				}

				if(color_like) {
					PreviewGroup out;
					out.subimage = subimage;
					out.name = group.name;
					out.channels_text = group.channels_text;
					out.channel_count = group.channel_count;
					out.red = group.red;
					out.green = group.green;
					out.blue = group.blue;
					out.alpha = group.alpha;
					if(out.name.IsEmpty())
						out.name = out.HasAlpha() ? "RGBA" : "RGB";
					preview_groups.Add(out);
				}
				else {
					for(int i = 0; i < names.GetCount(); ++i) {
						if(GroupPrefix(names[i]) != group.name)
							continue;
						PreviewGroup out;
						out.subimage = subimage;
						out.single_channel = i;
						out.channel_count = 1;
						out.channels_text = ShortChannelNameText(names[i]);
						out.name = group.name + "." + out.channels_text;
						preview_groups.Add(out);
					}
				}
			}
		}
	}

	input->close();
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
		if(selected_preview_group < 0 || selected_preview_group >= preview_groups.GetCount()) {
			error = "no preview group selected";
			return false;
		}

		const PreviewGroup& group = preview_groups[selected_preview_group];
		if(group.subimage < 0 || group.subimage >= subimages.GetCount()) {
			error = "selected preview group is unavailable";
			return false;
		}

		std::vector<float> source_pixels((size_t)source_spec.width * source_spec.height * source_spec.nchannels);
		if(!source_image.get_pixels(source_image.roi(), OIIO::TypeDesc::FLOAT, source_pixels.data(),
			                       source_spec.nchannels * sizeof(float),
			                       source_spec.width * source_spec.nchannels * sizeof(float), OIIO::AutoStride)) {
			error = source_image.geterror();
			return false;
		}

		int src_r = -1;
		int src_g = -1;
		int src_b = -1;
		int src_a = -1;
		if(group.HasRGB()) {
			src_r = group.red;
			src_g = group.green;
			src_b = group.blue;
		}
		else if(group.HasSingle()) {
			src_r = src_g = src_b = group.single_channel;
		}

		if(src_r < 0 || src_g < 0 || src_b < 0) {
			error = "selected preview group has no exportable color channels";
			return false;
		}

		src_a = group.HasAlpha() ? group.alpha : -1;
		expected_channels = src_a >= 0 ? 4 : 3;
		expected_alpha = src_a >= 0 ? expected_channels - 1 : -1;
		expected_names.clear();
		if(src_a >= 0) {
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
				dst[0] = src[src_r];
				dst[1] = src[src_g];
				dst[2] = src[src_b];
				if(expected_channels == 4)
					dst[3] = src[src_a];
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
		selected_preview_group = -1;
		UpdateViewerControls();
		ClearProbe();
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
	file_item.group_header = true;
	UiTreeNodeRef file_node = model.AddChild(model.Root(), file_item);
	UiTreeNodeRef first_group_node;

	for(int subimage = 0; subimage < std::max(1, subimage_count); ++subimage) {
		const ImageSubimageInfo* subinfo = subimages.IsEmpty() ? nullptr : &subimages[std::min(subimage, subimages.GetCount() - 1)];
		String sub_label = subimage_count > 1 ? Format("Subimage %d", subimage) : String("Image");
		UiModelItem sub_item;
		sub_item.text = sub_label;
		sub_item.group_header = true;
		if(subinfo) {
			sub_item.right_text = Format("%d x %d / %s", subinfo->size.cx, subinfo->size.cy, subinfo->pixel_type);
			sub_item.description = Format("%d channels", subinfo->channel_count);
		}
		else {
			sub_item.right_text = Format("%d x %d / %s", spec.width, spec.height, TypeDescText(spec.format));
			sub_item.description = Format("%d channels", spec.nchannels);
		}
		UiTreeNodeRef sub_node = model.AddChild(file_node, sub_item);

		if(subimage == 0) {
			for(int group_index = 0; group_index < preview_groups.GetCount(); ++group_index) {
				const PreviewGroup& group = preview_groups[group_index];
				if(group.subimage != subimage)
					continue;
				UiModelItem group_item;
				group_item.text = group.name;
				group_item.right_text = group.channels_text;
				group_item.description = Format("%d channel%s", group.channel_count, group.channel_count == 1 ? "" : "s");
				group_item.data = group_index;
				UiTreeNodeRef group_node = model.AddChild(sub_node, group_item);
				if(!first_group_node.IsValid())
					first_group_node = group_node;
				for(const std::string& channel_name : spec.channelnames) {
					String channel = channel_name.c_str();
					String prefix = GroupPrefix(channel);
					String leaf = ShortChannelNameText(channel);
					bool belongs = false;
					if(!group.name.IsEmpty() && prefix == group.name)
						belongs = true;
					else if(group.HasRGB() && prefix.IsEmpty() && (leaf == "R" || leaf == "G" || leaf == "B" || leaf == "A" || leaf == "ALPHA"))
						belongs = true;
					else if(group.HasSingle() && prefix.IsEmpty() && group.single_channel >= 0 && channel == String(spec.channelnames[group.single_channel].c_str()))
						belongs = true;
					if(!belongs)
						continue;
					UiModelItem channel_item;
					channel_item.text = channel;
					channel_item.description = Format("subimage %d channel", subimage);
					channel_item.enabled = false;
					model.AddChild(group_node, channel_item);
				}
			}
		}
		else {
			UiModelItem preview_na;
			preview_na.text = "Preview not yet supported";
			preview_na.description = "Metadata only; selectable preview is limited to subimage 0.";
			preview_na.enabled = false;
			model.AddChild(sub_node, preview_na);
		}
	}

	if(first_group_node.IsValid())
		layers_tree.SelectNode(first_group_node);
	else {
		layers_tree.ClearSelection();
		selected_preview_group = -1;
		UpdateViewerControls();
		ClearProbe();
		canvas.ClearImage();
		UpdateSelectionSummary();
	}
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

	String error;
	if(!LoadImageFile(path, error, true)) {
		Exclamation("Unable to open image:\n" + error);
		SetStatus("Unable to load: " + error);
		return;
	}

	SetStatus("Loaded: " + GetFileName(path));
}

bool ImagingWorkbench::LoadImageFile(const String& path, String& error, bool populate_ui)
{
	String ext = ToLower(GetFileExt(path));
	if(ext != ".exr" && ext != ".png") {
		error = "unsupported extension";
		return false;
	}

	SetStatus("Loading: " + GetFileName(path));

	using Clock = std::chrono::steady_clock;
	auto load_started = Clock::now();
	std::string io_error;
	OIIO::ImageBuf loaded;
	if(!UppImaging::LoadImage(path.Begin(), loaded, &io_error)) {
		error = io_error.c_str();
		last_error = error;
		return false;
	}
	auto load_ms = std::chrono::duration<double, std::milli>(Clock::now() - load_started).count();

	source_image = loaded;
	source_filename = path;
	probe_source_pixel.SetCount(source_image.spec().nchannels);
	last_error.Clear();
	ScanSourceMetadata();
	UpdateDisplayState();
	UpdateOcioControls();
	if(populate_ui)
		UpdateLayersPage();
	else if(!preview_groups.IsEmpty())
		selected_preview_group = 0;
	save_split_button.Enable();
	fit_view_button.Enable();
	RecordTiming("file load", load_ms);
	return true;
}

} // namespace Upp
