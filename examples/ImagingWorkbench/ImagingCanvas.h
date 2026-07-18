#ifndef _ImagingWorkbench_ImagingCanvas_h_
#define _ImagingWorkbench_ImagingCanvas_h_

#include <CtrlLib/CtrlLib.h>

namespace Upp {

class ImagingCanvas : public Ctrl {
public:
	typedef ImagingCanvas CLASSNAME;

	ImagingCanvas();

	void SetPlaceholderText(const String& text);

protected:
	virtual void Paint(Draw& w) override;

private:
	String placeholder = "Stage A: canvas host";
};

} // namespace Upp

#endif
