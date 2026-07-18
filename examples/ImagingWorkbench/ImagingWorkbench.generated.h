#ifndef _ImagingWorkbench_ImagingWorkbench_generated_h_
#define _ImagingWorkbench_ImagingWorkbench_generated_h_

#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

namespace Upp {

class ImagingWorkbenchLayout : public TopWindow {
public:
	typedef ImagingWorkbenchLayout CLASSNAME;

	ImagingWorkbenchLayout();

protected:
	virtual void BindActions() {}
	virtual void PostBuild() {}

	void InitWindow();
	void InitThemeContext();
	void BuildControls();
	void ApplyAppearanceOverrides();
	void BuildLayout();

	UiBoxLayout mainColumn;
	UiTitleCard hidder_card;
	UiButton quit_button;
	UiBoxLayout main_top_layout;
	UiButton load_button;
	UiSplitButton save_split_button;
	UiGridLayout drop_controls_layout;
	UiLabel resolution;
	UiLabel memory;
	UiLabel exposure;
	UiLabel gamma;
	UiLabel res_label;
	UiLabel memory_label;
	UiBoxLayout rgba_layout;
	UiToolButton r_too;
	UiToolButton g_tool;
	UiToolButton b_tool;
	UiToolButton a_tool;
	UiToolButton rbg_tool;
	UiSlider exposure_slider;
	UiFloatEdit exposure_float_edit;
	UiFloatEdit gamma_float_edit;
	UiSlider gamma_slider;
	UiBoxLayout center_layout;
	UiBoxLayout center_layout_horz;
	UiBoxLayout tool_layout;
	UiPanel panel;
	UiScrollPanel canvas_scroll_panel;
	UiBoxLayout boxlayout_03;
	UiLabel label;
	UiLabel label_02;
	UiPanel right_tab_panel;
	UiTab right_tab;
	ParentCtrl pageA;
	ParentCtrl pageB;
	ParentCtrl pageC;
	UiLabel xy_label;
	UiLabel xy_info;
	UiLabel colour_label;
	UiLabel color_info;
	UiToolButton fit_view_button;
	UiBoxLayout boxlayout_04;
	UiPanel status_area;
	UiLabel status;
};

} // namespace Upp

#endif
