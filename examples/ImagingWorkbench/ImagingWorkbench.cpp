#include "ImagingWorkbench.h"

namespace Upp {

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
}

void ImagingWorkbench::PostBuild()
{
	layers_body.SetText("Layers tab placeholder").SetAlign(UiAlign::CENTER, UiAlign::CENTER);
	ocio_body.SetText("OCIO tab placeholder").SetAlign(UiAlign::CENTER, UiAlign::CENTER);
	analysis_body.SetText("Analysis tab placeholder").SetAlign(UiAlign::CENTER, UiAlign::CENTER);

	pageA.Add(layers_body.SizePos());
	pageB.Add(ocio_body.SizePos());
	pageC.Add(analysis_body.SizePos());

	canvas.SetPlaceholderText("Stage A: image canvas placeholder");
	canvas_scroll_panel.Content().Add(canvas.SizePos());

	status.SetText("Stage A ready");
	right_tab.SetActiveTab(0);

#ifdef _DEBUG
	String dump;
	dump << "TopWindow=" << AsString(GetRect()) << '\n';
	dump << "mainColumn=" << AsString(mainColumn.GetRect()) << '\n';
	dump << "main_top_layout=" << AsString(main_top_layout.GetRect()) << '\n';
	dump << "hidder_card=" << AsString(hidder_card.GetRect()) << '\n';
	dump << "drop_controls_layout=" << AsString(drop_controls_layout.GetRect()) << '\n';
	dump << "boxlayout_04=" << AsString(boxlayout_04.GetRect()) << '\n';
	dump << "load_button=" << AsString(load_button.GetRect()) << '\n';
	dump << "save_split_button=" << AsString(save_split_button.GetRect()) << '\n';
	dump << "quit_button=" << AsString(quit_button.GetRect()) << '\n';
	dump << "center_layout=" << AsString(center_layout.GetRect()) << '\n';
	dump << "center_layout_horz=" << AsString(center_layout_horz.GetRect()) << '\n';
	dump << "tool_layout=" << AsString(tool_layout.GetRect()) << '\n';
	dump << "panel=" << AsString(panel.GetRect()) << '\n';
	dump << "boxlayout_03=" << AsString(boxlayout_03.GetRect()) << '\n';
	dump << "canvas_scroll_panel=" << AsString(canvas_scroll_panel.GetRect()) << '\n';
	dump << "right_tab_panel=" << AsString(right_tab_panel.GetRect()) << '\n';
	dump << "right_tab=" << AsString(right_tab.GetRect()) << '\n';
	dump << "pageA=" << AsString(pageA.GetRect()) << '\n';
	dump << "pageB=" << AsString(pageB.GetRect()) << '\n';
	dump << "pageC=" << AsString(pageC.GetRect()) << '\n';
	dump << "status_area=" << AsString(status_area.GetRect()) << '\n';
	dump << "status=" << AsString(status.GetRect()) << '\n';
	dump << "resolution=" << AsString(resolution.GetRect()) << '\n';
	dump << "res_label=" << AsString(res_label.GetRect()) << '\n';
	dump << "memory=" << AsString(memory.GetRect()) << '\n';
	dump << "memory_label=" << AsString(memory_label.GetRect()) << '\n';
	dump << "rgba_layout=" << AsString(rgba_layout.GetRect()) << '\n';
	dump << "rbg_tool=" << AsString(rbg_tool.GetRect()) << '\n';
	dump << "exposure=" << AsString(exposure.GetRect()) << '\n';
	dump << "exposure_slider=" << AsString(exposure_slider.GetRect()) << '\n';
	dump << "exposure_float_edit=" << AsString(exposure_float_edit.GetRect()) << '\n';
	dump << "gamma=" << AsString(gamma.GetRect()) << '\n';
	dump << "gamma_slider=" << AsString(gamma_slider.GetRect()) << '\n';
	dump << "gamma_float_edit=" << AsString(gamma_float_edit.GetRect()) << '\n';
	SaveFile("out/ImagingWorkbench.layout.txt", dump);
#endif
}

void ImagingWorkbench::DoLoad()
{
	Exclamation("Stage A placeholder. Load wiring comes in Stage B.");
}

void ImagingWorkbench::DoSave()
{
	Exclamation("Stage A placeholder. Save wiring comes in Stage B.");
}

void ImagingWorkbench::DoSaveFormat(const Value& data)
{
	Exclamation("Selected save format: " + AsString(data));
}

} // namespace Upp
