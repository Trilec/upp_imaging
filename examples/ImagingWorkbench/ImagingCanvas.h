#ifndef _ImagingWorkbench_ImagingCanvas_h_
#define _ImagingWorkbench_ImagingCanvas_h_

#include <CtrlLib/CtrlLib.h>

#include <imaging_view_transform/imaging_view_transform.h>

namespace Upp {

class ImagingCanvas : public Ctrl {
public:
	typedef ImagingCanvas CLASSNAME;

	ImagingCanvas();

	void SetImage(const Image& image);
	void SetDisplayImage(const Image& image, Size original_source_size, bool reset_view = false);
	void ClearImage();
	void SetFitMode(bool fit);
	bool HasImage() const;
	Size GetSourceSize() const;
	double GetDisplayedScale() const;
	const ImageViewState& GetViewState() const;
	const ImageViewGeometry& GetViewGeometry() const;
	void SetViewState(const ImageViewState& state, bool keep_source_center = false);
	void SetPlaceholderText(const String& text);

	Event<> WhenViewChanged;
	Event<Point> WhenSourcePixelMove;
	Event<> WhenSourcePixelLeave;

protected:
	virtual void Paint(Draw& w) override;
	virtual void Layout() override;
	virtual void MouseMove(Point p, dword keyflags) override;
	virtual void MiddleDown(Point p, dword keyflags) override;
	virtual void MiddleDrag(Point p, dword keyflags) override;
	virtual void MiddleUp(Point p, dword keyflags) override;
	virtual void MouseWheel(Point p, int zdelta, dword keyflags) override;
	virtual void MouseLeave() override;
	virtual void CancelMode() override;

private:
	static constexpr double MIN_ZOOM = 0.05;
	static constexpr double MAX_ZOOM = 32.0;
	static constexpr double WHEEL_STEP = 1.1;
	void RecomputeGeometry();
	void ClampViewState();
	void UpdateProbeFromPoint(Point p);
	bool ViewToSource(Point p, Point& source_point) const;
	void BeginPan(Point p);
	void UpdatePan(Point p);
	void EndPan();
	void ZoomAt(Point p, double factor);

	Image image;
	Size proxy_size;
	Size source_size;
	ImageViewState view_state;
	ImageViewGeometry view_geometry;
	bool panning = false;
	Point pan_start_mouse;
	Pointf pan_start_pan;
	String placeholder = "Stage A: canvas host";
};

} // namespace Upp

#endif
