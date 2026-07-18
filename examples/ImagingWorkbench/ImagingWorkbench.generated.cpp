#include "ImagingWorkbench.generated.h"

namespace Upp {

static UiToolButton::Style MakeChannelToolStyle(Color face)
{
	UiToolButton::Style s = UiTheme::ResolveToolButton(UiRole::Standard);
	s.metrics.face_enabled = true;
	s.palette.face[ST_NORMAL] = UiFill::Solid(face);
	s.palette.face[ST_HOT] = UiFill::Solid(Blend(face, White(), 24));
	s.palette.face[ST_PRESSED] = UiFill::Solid(Blend(face, Black(), 16));
	s.palette.face[ST_DISABLED] = UiFill::Solid(Blend(face, SColorFace(), 90));
	s.metrics.frame_enabled = true;
	for(int i = 0; i < 4; ++i)
		s.palette.frame[i] = Color(85, 85, 85);
	s.metrics.frame_width = DPI(2);
	s.metrics.radius = DPI(4);
	s.metrics.shadow.enabled = false;
	return s;
}

static UiBaseEdit::Style MakeFloatEditStyle(bool dark_text = true)
{
	UiBaseEdit::Style s = UiTheme::ResolveEdit(UiRole::Standard);
	s.metrics.face_enabled = false;
	s.metrics.frame_enabled = true;
	for(int i = 0; i < 4; ++i)
		s.palette.frame[i] = Color(74, 74, 74);
	s.metrics.frame_width = DPI(0);
	s.metrics.radius = DPI(0);
	s.metrics.shadow.enabled = false;
	if(dark_text)
		s.palette.ink[ST_NORMAL] = Color(224, 224, 224);
	return s;
}

static UiScrollPanel::Style MakeCanvasScrollStyle()
{
	UiScrollPanel::Style s = UiTheme::ResolveScrollPanel(UiRole::Subtle);
	s.metrics.face_enabled = true;
	Color face = Color(34, 34, 36);
	s.palette.face[ST_NORMAL] = UiFill::Solid(face);
	s.palette.face[ST_HOT] = UiFill::Solid(Blend(face, White(), 24));
	s.palette.face[ST_PRESSED] = UiFill::Solid(Blend(face, Black(), 16));
	s.palette.face[ST_DISABLED] = UiFill::Solid(Blend(face, SColorFace(), 90));
	s.metrics.frame_enabled = true;
	for(int i = 0; i < 4; ++i)
		s.palette.frame[i] = Color(84, 84, 88);
	s.metrics.frame_width = DPI(0);
	s.metrics.radius = DPI(0);
	s.metrics.shadow.enabled = false;
	return s;
}

ImagingWorkbenchLayout::ImagingWorkbenchLayout()
{
	InitWindow();
	InitThemeContext();
	BuildControls();
	ApplyAppearanceOverrides();
	BuildLayout();
	BindActions();
	PostBuild();
}

void ImagingWorkbenchLayout::InitWindow()
{
	Title("ImagingWorkbench").Sizeable().Zoomable();
	SetMinSize(Size(DPI(1138), DPI(631)));
}

void ImagingWorkbenchLayout::InitThemeContext()
{
}

void ImagingWorkbenchLayout::BuildControls()
{
	mainColumn.SetDirection(UiDirection::V).SetGap(DPI(0), DPI(4)).SetInset(DPI(8)).SetWrap(UiBoxWrap::None);
	main_top_layout.SetDirection(UiDirection::H).SetGap(DPI(3), DPI(0)).SetInset(DPI(0)).SetWrap(UiBoxWrap::None);
	center_layout.SetDirection(UiDirection::V).SetGap(DPI(0), DPI(0)).SetInset(DPI(0)).SetWrap(UiBoxWrap::None);
	center_layout_horz.SetDirection(UiDirection::H).SetGap(DPI(8), DPI(8)).SetInset(DPI(0)).SetWrap(UiBoxWrap::None);
	tool_layout.SetDirection(UiDirection::V).SetGap(DPI(0), DPI(0)).SetInset(DPI(0)).SetWrap(UiBoxWrap::None);
	boxlayout_03.SetDirection(UiDirection::H).SetGap(DPI(8), DPI(8)).SetInset(DPI(4)).SetWrap(UiBoxWrap::None);
	boxlayout_04.SetDirection(UiDirection::H).SetGap(DPI(8), DPI(8)).SetInset(DPI(8)).SetWrap(UiBoxWrap::Flow).SetWrapAutoResize(true);
	rgba_layout.SetDirection(UiDirection::H).SetGap(DPI(2), DPI(2)).SetInset(DPI(1)).SetWrap(UiBoxWrap::None);
	drop_controls_layout.SetGridSize(7, 2).SetMinCellSize(Size(DPI(10), DPI(10))).SetGap(DPI(0)).SetInset(DPI(0));
	right_tab.SetCustomStyle(UiTheme::ResolveTab(UiRole::Accent, UITAB_UNDERLINE));
	right_tab.SetVisual(UITAB_UNDERLINE).SetPlacement(UiAlign::TOP).SetExpandTabs(false).EnableCloseButtons(false).EnableDragHandles(false);
	right_tab.SetTabFont(SansSerifZ(11)).SetTabIconSize(DPI(16)).SetTabIconSide(UiAlign::LEFT);
	right_tab_panel.SetSizeMin(DPI(276), DPI(0));
	right_tab_panel.SetInset(DPI(0));
	canvas_scroll_panel.SetInset(DPI(0));
	canvas_scroll_panel.SetScrollMode(UIPANELSCROLL_AUTO);

	hidder_card.SetTitle("WORKBENCH").SetSubTitle("Demo of OpenImageIO and OpenColorIO").SetContentInset(DPI(0)).SetMediaGap(DPI(10)).SetMediaReserve(DPI(24)).SetMediaMin(DPI(15)).SetMediaAutoFit(false).SetMediaSide(UiAlign::LEFT).SetMediaAlign(UiAlign::CENTER, UiAlign::CENTER).ShowTitleLine(false).ShowCardLine(true).SetTextAlign(UiAlign::LEFT, UiAlign::CENTER);
	hidder_card.SetMedia(ICON_BRAND_NEWLOGO_V5_48(), Size(DPI(18), DPI(18)));
	quit_button.SetText("Quit").SetContentInset(DPI(6)).SetContentGap(DPI(4));
	quit_button.SetAlign(UiAlign::CENTER, UiAlign::CENTER);
	quit_button.SetIconSide(UiAlign::LEFT);
	quit_button.SetIcon(ICON_ACTION_CANCEL_48()).SetIconSize(DPI(15), DPI(15));
	load_button.SetText("Load").SetContentInset(DPI(6)).SetContentGap(DPI(4));
	load_button.SetAlign(UiAlign::CENTER, UiAlign::CENTER);
	load_button.SetIconSide(UiAlign::LEFT);
	save_split_button.SetText("Save").SetContentInset(DPI(6)).SetContentGap(DPI(4));
	save_split_button.SetSplitWidth(DPI(30));
	save_split_button.SetSplitContentGap(DPI(4));
	save_split_button.SetSplitIconSize(DPI(16));
	save_split_button.SetPopupMinWidth(DPI(220));
	save_split_button.SetAlign(UiAlign::CENTER, UiAlign::CENTER);
	save_split_button.SetIconSide(UiAlign::LEFT);
	save_split_button.Add("EXR", "EXR").Add("PNG", "PNG").Add("JPG", "JPG");

	resolution.SetText("Resolution").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	memory.SetText("Memory").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	exposure.SetText("Exp:").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false).SetIcon(ICON_DESIGN_TUNE_48(), UiIconRenderMode::MonoTint).SetIconSize(DPI(9), DPI(9));
	gamma.SetText("Gamma").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false).SetIcon(ICON_DESIGN_TUNE_48(), UiIconRenderMode::MonoTint).SetIconSize(DPI(9), DPI(9));
	res_label.SetText("1280x720").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	memory_label.SetText("14.1 MB").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);

	r_too.SetText("R").SetContentInset(DPI(4)).SetContentGap(DPI(4)).SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT);
	g_tool.SetText("G").SetContentInset(DPI(4)).SetContentGap(DPI(4)).SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT);
	b_tool.SetText("B").SetContentInset(DPI(4)).SetContentGap(DPI(4)).SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT);
	a_tool.SetText("A").SetContentInset(DPI(4)).SetContentGap(DPI(4)).SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT);
	rbg_tool.SetText("RGB").SetContentInset(DPI(4)).SetContentGap(DPI(4)).SetAlign(UiAlign::CENTER, UiAlign::CENTER).SetIconSide(UiAlign::LEFT);
	exposure_slider.SetRange(0, 100).SetValue(50);
	exposure_float_edit.MinMax(0, 100).Step(0.1).Precision(2).ShowSpin(true);
	exposure_float_edit.SetValue(0.0);
	gamma_slider.SetRange(0, 100).SetValue(50);
	gamma_float_edit.MinMax(0, 100).Step(0.1).Precision(2).ShowSpin(true);
	gamma_float_edit.SetValue(1.0);

	label.SetText("Zoom:").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	label_02.SetText("62%").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	fit_view_button.SetText("").SetContentInset(DPI(4)).SetContentGap(DPI(4)).SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetIcon(ICON_DESIGN_ARROWS_OUTPUT_48()).SetIconSize(DPI(15), DPI(15));
	xy_label.SetText("XY:").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	xy_info.SetText("100,100").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	colour_label.SetText("Color:").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	color_info.SetText("1.00, 0.23, 0.57").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	status.SetText("Ready").SetAlign(UiAlign::LEFT, UiAlign::CENTER).SetIconSide(UiAlign::LEFT).SetContentGap(DPI(6)).SetIconScaleToContent(false);
	status_area.SetCustomStyle(UiTheme::ResolvePanel(UiRole::Subtle));
	status_area.SetSizeMin(Size(DPI(0), DPI(24)));

	right_tab.Add(pageA, "Layers");
	right_tab.Add(pageB, "OCIO");
	right_tab.Add(pageC, "Analysis");
	right_tab.SetActiveTab(0);
}

void ImagingWorkbenchLayout::ApplyAppearanceOverrides()
{
	hidder_card.SetCustomStyle(UiTheme::ResolveTitleCard(UiRole::Standard));
	quit_button.SetCustomStyle(UiTheme::ResolveButton(UiRole::Alert));
	load_button.SetCustomStyle(UiTheme::ResolveButton(UiRole::Accent));
	save_split_button.SetCustomStyle(UiTheme::ResolveButton(UiRole::Accent));
	resolution.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	memory.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	exposure.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	gamma.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	label.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	xy_label.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	colour_label.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
	r_too.SetCustomStyle(MakeChannelToolStyle(Color(46, 46, 46)));
	g_tool.SetCustomStyle(MakeChannelToolStyle(Color(48, 48, 48)));
	b_tool.SetCustomStyle(MakeChannelToolStyle(Color(48, 48, 48)));
	a_tool.SetCustomStyle(MakeChannelToolStyle(Color(43, 43, 43)));
	rbg_tool.SetCustomStyle(MakeChannelToolStyle(Color(43, 43, 43)));
	exposure_float_edit.SetCustomStyle(MakeFloatEditStyle());
	gamma_float_edit.SetCustomStyle(MakeFloatEditStyle());
	canvas_scroll_panel.SetCustomStyle(MakeCanvasScrollStyle());
}

void ImagingWorkbenchLayout::BuildLayout()
{
	Add(mainColumn);
	mainColumn.Add(main_top_layout).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	main_top_layout.Add(hidder_card).Fit().MinCross(DPI(37)).AlignSelf(UiBoxLayout::Align::Stretch);
	main_top_layout.Add(drop_controls_layout).Expand(1).MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	{
		auto spacer = main_top_layout.AddSpacer(1);
		spacer.Fit().MinMain(DPI(12));
		spacer.MinCross(DPI(10)).AlignSelf(UiBoxLayout::Align::Stretch);
		spacer.LineEnabled(true).LineOrientation(UiSpacerLineOrientation::Vertical).LineAlign(UiCrossAlign::Start).LineThickness(DPI(2)).LineDash(SOLID).LineInset(DPI(2)).LineColorEnabled(true).LineColor(Color(52, 52, 52));
	}
	main_top_layout.Add(boxlayout_04).Expand(1).MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	boxlayout_04.Add(load_button).Fixed(DPI(80)).MinCross(DPI(0));
	boxlayout_04.Add(save_split_button).Fixed(DPI(80)).MinCross(DPI(0));
	boxlayout_04.Add(quit_button).Fixed(DPI(78)).MinCross(DPI(0));

	mainColumn.Add(center_layout).Expand(1).MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	center_layout.Add(center_layout_horz).Expand(1).MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	center_layout_horz.Add(tool_layout).Expand(1).MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	tool_layout.Add(boxlayout_03).Fit().MinCross(DPI(0));
	tool_layout.Add(canvas_scroll_panel).Expand(1).MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	center_layout.Add(right_tab_panel).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	right_tab_panel.Add(right_tab.SizePos());

	boxlayout_03.Add(label).Fit().MinCross(DPI(0));
	boxlayout_03.Add(label_02).Fit().MinCross(DPI(0));
	boxlayout_03.Add(fit_view_button).Fit().MinCross(DPI(0));
	{
		auto spacer = boxlayout_03.AddSpacer(1);
		spacer.Fit().MinMain(DPI(10));
		spacer.MinCross(DPI(10)).AlignSelf(UiBoxLayout::Align::Stretch);
		spacer.LineEnabled(true).LineOrientation(UiSpacerLineOrientation::Vertical).LineAlign(UiCrossAlign::Center).LineThickness(DPI(2)).LineDash(SOLID).LineInset(DPI(0)).LineColorEnabled(true).LineColor(Color(52, 52, 52));
	}
	boxlayout_03.Add(xy_label).Fit().MinCross(DPI(0));
	boxlayout_03.Add(xy_info).Fit().MinCross(DPI(0));
	{
		auto spacer = boxlayout_03.AddSpacer(1);
		spacer.Fit().MinMain(DPI(10));
		spacer.MinCross(DPI(10)).AlignSelf(UiBoxLayout::Align::Stretch);
		spacer.LineEnabled(true).LineOrientation(UiSpacerLineOrientation::Vertical).LineAlign(UiCrossAlign::Center).LineThickness(DPI(2)).LineDash(SOLID).LineInset(DPI(0)).LineColorEnabled(true).LineColor(Color(52, 52, 52));
	}
	boxlayout_03.Add(colour_label).Fit().MinCross(DPI(0));
	boxlayout_03.Add(color_info).Fit().MinCross(DPI(0));

	// Top control grid exactly follows the designer node order.
	drop_controls_layout.Add(resolution, 0, 0, false, false);
	drop_controls_layout.Add(res_label, 0, 1, true, false);
	drop_controls_layout.Add(memory, 0, 2, false, false);
	drop_controls_layout.Add(memory_label, 0, 3, true, false);
	drop_controls_layout.Add(rgba_layout, 0, 4, true, false);
	drop_controls_layout.Add(exposure, 0, 5, false, false);
	drop_controls_layout.Add(exposure_slider, 1, 5, true, false);
	drop_controls_layout.Add(exposure_float_edit, 1, 6, true, false);
	drop_controls_layout.Add(gamma, 1, 0, false, false);
	drop_controls_layout.Add(gamma_slider, 1, 1, true, false);
	drop_controls_layout.Add(gamma_float_edit, 1, 2, true, false);
	drop_controls_layout.Add(rbg_tool, 1, 4, true, false);

	rgba_layout.Add(r_too).Expand(1).MinCross(DPI(0));
	rgba_layout.Add(g_tool).Expand(1).MinCross(DPI(0));
	rgba_layout.Add(b_tool).Expand(1).MinCross(DPI(0));
	rgba_layout.Add(a_tool).Expand(1).MinCross(DPI(0));

	// Contract placeholders resolved through real tab page content.
	pageA.SetMinSize(Size(DPI(220), DPI(140)));
	pageB.SetMinSize(Size(DPI(220), DPI(140)));
	pageC.SetMinSize(Size(DPI(220), DPI(140)));
	mainColumn.Add(status_area).Fit().MinCross(DPI(0)).AlignSelf(UiBoxLayout::Align::Stretch);
	status_area.Add(status.SizePos());
}

} // namespace Upp
