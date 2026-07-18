#include "ImagingWorkbench.h"

namespace Upp {

ImagingWorkbench::ImagingWorkbench()
{
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
