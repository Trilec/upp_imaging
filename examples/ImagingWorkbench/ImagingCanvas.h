#ifndef _ImagingWorkbench_ImagingCanvas_h_
#define _ImagingWorkbench_ImagingCanvas_h_

#include <CtrlLib/CtrlLib.h>

namespace Upp {

class ImagingCanvas : public Ctrl {
public:
	typedef ImagingCanvas CLASSNAME;

	ImagingCanvas();

	void SetImage(const Image& image);
	void ClearImage();
	void SetFitMode(bool fit);
	bool HasImage() const;
	Size GetSourceSize() const;
	double GetDisplayedScale() const;
	void SetPlaceholderText(const String& text);

	Event<> WhenViewChanged;

protected:
	virtual void Paint(Draw& w) override;
	virtual void Layout() override;

private:
	void UpdateViewState();

	Image image;
	Size source_size;
	Rect image_rect;
	double displayed_scale = 0.0;
	bool fit_mode = true;
	String placeholder = "Stage A: canvas host";
};

} // namespace Upp

#endif
