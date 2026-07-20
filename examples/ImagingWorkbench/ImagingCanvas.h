#ifndef _ImagingWorkbench_ImagingCanvas_h_
#define _ImagingWorkbench_ImagingCanvas_h_

#include <CtrlLib/CtrlLib.h>

namespace Upp {

class ImagingCanvas : public Ctrl {
public:
	typedef ImagingCanvas CLASSNAME;

	ImagingCanvas();

	void SetImage(const Image& image);
	void SetDisplayImage(const Image& image, Size original_source_size);
	void ClearImage();
	void SetFitMode(bool fit);
	bool HasImage() const;
	Size GetSourceSize() const;
	double GetDisplayedScale() const;
	void SetPlaceholderText(const String& text);
	bool ViewToImage(Point view_point, Point& image_point) const;

	Event<> WhenViewChanged;
	Event<Point> WhenImageMouseMove;
	Event<> WhenImageMouseLeave;

protected:
	virtual void Paint(Draw& w) override;
	virtual void Layout() override;
	virtual void MouseMove(Point p, dword keyflags) override;
	virtual void MouseLeave() override;

private:
	void UpdateViewState();

	Image image;
	Size display_size;
	Size source_size;
	Rect image_rect;
	double displayed_scale = 0.0;
	bool fit_mode = true;
	String placeholder = "Stage A: canvas host";
};

} // namespace Upp

#endif
