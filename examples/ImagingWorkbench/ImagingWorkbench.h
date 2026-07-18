#ifndef _ImagingWorkbench_ImagingWorkbench_h_
#define _ImagingWorkbench_ImagingWorkbench_h_

#include "ImagingWorkbench.generated.h"
#include "ImagingCanvas.h"

namespace Upp {

class ImagingWorkbench : public ImagingWorkbenchLayout {
public:
	typedef ImagingWorkbench CLASSNAME;

	ImagingWorkbench();

protected:
	virtual void BindActions() override;
	virtual void PostBuild() override;

private:
	void DoLoad();
	void DoSave();
	void DoSaveFormat(const Value& data);

	ImagingCanvas canvas;
	UiLabel layers_body;
	UiLabel ocio_body;
	UiLabel analysis_body;
};

} // namespace Upp

#endif
