#include "ImagingCanvas.h"

#include <algorithm>
#include <cmath>

namespace Upp {

namespace {

static Rect DrawRectFromGeometry(const Rectf& r)
{
	return RectC((int)std::round(r.left), (int)std::round(r.top), (int)std::round(r.Width()), (int)std::round(r.Height()));
}

}

ImagingCanvas::ImagingCanvas()
{
	SetMinSize(Size(DPI(1), DPI(1)));
}

void ImagingCanvas::SetImage(const Image& value)
{
	SetDisplayImage(value, IsNull(value) ? Size() : value.GetSize(), true);
}

void ImagingCanvas::SetDisplayImage(const Image& value, Size original_source_size, bool reset_view)
{
	EndPan();
	ImageViewState previous_state = view_state;
	ImageViewGeometry previous_geometry = view_geometry;
	Size previous_source = source_size;
	Size previous_display = proxy_size;

	image = value;
	proxy_size = IsNull(image) ? Size() : image.GetSize();
	source_size = original_source_size;
	if(source_size.cx <= 0 || source_size.cy <= 0)
		source_size = proxy_size;

	if(IsNull(image) || proxy_size.cx <= 0 || proxy_size.cy <= 0 || source_size.cx <= 0 || source_size.cy <= 0) {
		view_state = ImageViewState();
		view_geometry = ImageViewGeometry();
		panning = false;
		Refresh();
		WhenViewChanged();
		return;
	}

	if(reset_view || !previous_geometry.IsValid() || previous_display.cx <= 0 || previous_display.cy <= 0 || previous_source.cx <= 0 || previous_source.cy <= 0) {
		view_state.mode = ViewMode::Fit;
		view_state.zoom = 1.0;
		view_state.pan = Pointf(proxy_size.cx / 2.0, proxy_size.cy / 2.0);
	}
	else if(previous_state.mode == ViewMode::Manual) {
		Pointf source_center;
		Pointf viewport_center(previous_geometry.viewport_size.cx / 2.0, previous_geometry.viewport_size.cy / 2.0);
		if(previous_geometry.ViewToSource(viewport_center, source_center)) {
			view_state.mode = ViewMode::Manual;
			view_state.zoom = std::clamp(previous_state.zoom, MIN_ZOOM, MAX_ZOOM);
			view_state.pan = Pointf(
				source_size.cx > 1 && proxy_size.cx > 1 ? source_center.x * (double)(proxy_size.cx - 1) / (double)(source_size.cx - 1) : proxy_size.cx / 2.0,
				source_size.cy > 1 && proxy_size.cy > 1 ? source_center.y * (double)(proxy_size.cy - 1) / (double)(source_size.cy - 1) : proxy_size.cy / 2.0);
		}
		else {
			view_state.mode = ViewMode::Fit;
			view_state.zoom = 1.0;
			view_state.pan = Pointf(proxy_size.cx / 2.0, proxy_size.cy / 2.0);
		}
	}
	else {
		view_state.mode = ViewMode::Fit;
		view_state.zoom = 1.0;
		view_state.pan = Pointf(proxy_size.cx / 2.0, proxy_size.cy / 2.0);
	}

	RecomputeGeometry();
	ClampViewState();
	RecomputeGeometry();
	Refresh();
	WhenViewChanged();
}

void ImagingCanvas::ClearImage()
{
	EndPan();
	image = Image();
	proxy_size = Size();
	source_size = Size();
	view_state = ImageViewState();
	view_geometry = ImageViewGeometry();
	panning = false;
	Refresh();
	WhenViewChanged();
}

void ImagingCanvas::SetFitMode(bool fit)
{
	view_state.mode = fit ? ViewMode::Fit : ViewMode::Manual;
	if(fit) {
		view_state.zoom = 1.0;
		view_state.pan = Pointf(proxy_size.cx / 2.0, proxy_size.cy / 2.0);
	}
	ClampViewState();
	RecomputeGeometry();
	Refresh();
	WhenViewChanged();
}

bool ImagingCanvas::HasImage() const
{
	return !IsNull(image);
}

Size ImagingCanvas::GetSourceSize() const
{
	return source_size;
}

double ImagingCanvas::GetDisplayedScale() const
{
	return view_geometry.effective_zoom;
}

const ImageViewState& ImagingCanvas::GetViewState() const
{
	return view_state;
}

const ImageViewGeometry& ImagingCanvas::GetViewGeometry() const
{
	return view_geometry;
}

void ImagingCanvas::SetViewState(const ImageViewState& state, bool keep_source_center)
{
	if(keep_source_center && view_geometry.IsValid() && state.mode == ViewMode::Manual && view_state.mode == ViewMode::Manual) {
		Pointf center_source;
		if(view_geometry.ViewToSource(Pointf(GetSize().cx / 2.0, GetSize().cy / 2.0), center_source)) {
			view_state = state;
			view_state.pan = Pointf(
				source_size.cx > 1 && proxy_size.cx > 1 ? center_source.x * (double)(proxy_size.cx - 1) / (double)(source_size.cx - 1) : proxy_size.cx / 2.0,
				source_size.cy > 1 && proxy_size.cy > 1 ? center_source.y * (double)(proxy_size.cy - 1) / (double)(source_size.cy - 1) : proxy_size.cy / 2.0);
			ClampViewState();
			RecomputeGeometry();
			Refresh();
			WhenViewChanged();
			return;
		}
	}
	view_state = state;
	ClampViewState();
	RecomputeGeometry();
	Refresh();
	WhenViewChanged();
}

void ImagingCanvas::SetPlaceholderText(const String& text)
{
	placeholder = text;
	Refresh();
}

void ImagingCanvas::Layout()
{
	Ctrl::Layout();
	ClampViewState();
	RecomputeGeometry();
	WhenViewChanged();
}

void ImagingCanvas::MouseMove(Point p, dword keyflags)
{
	if(panning) {
		UpdatePan(p);
		return;
	}
	UpdateProbeFromPoint(p);
	Ctrl::MouseMove(p, keyflags);
}

void ImagingCanvas::MiddleDown(Point p, dword keyflags)
{
	if(!HasImage())
		return;
	BeginPan(p);
	Ctrl::MiddleDown(p, keyflags);
}

void ImagingCanvas::MiddleDrag(Point p, dword keyflags)
{
	if(panning) {
		UpdatePan(p);
		return;
	}
	Ctrl::MiddleDrag(p, keyflags);
}

void ImagingCanvas::MiddleUp(Point p, dword keyflags)
{
	if(panning)
		EndPan();
	Ctrl::MiddleUp(p, keyflags);
}

void ImagingCanvas::MouseWheel(Point p, int zdelta, dword keyflags)
{
	if(HasImage() && zdelta != 0) {
		ZoomAt(p, zdelta > 0 ? WHEEL_STEP : 1.0 / WHEEL_STEP);
		return;
	}
	Ctrl::MouseWheel(p, zdelta, keyflags);
}

void ImagingCanvas::MouseLeave()
{
	if(!panning && WhenSourcePixelLeave)
		WhenSourcePixelLeave();
	Ctrl::MouseLeave();
}

void ImagingCanvas::CancelMode()
{
	EndPan();
	Ctrl::CancelMode();
}

void ImagingCanvas::RecomputeGeometry()
{
	view_geometry = BuildImageViewGeometry(GetSize(), proxy_size, source_size, view_state);
}

void ImagingCanvas::ClampViewState()
{
	if(!HasImage() || proxy_size.cx <= 0 || proxy_size.cy <= 0)
		return;
	view_state.zoom = std::clamp(view_state.zoom, MIN_ZOOM, MAX_ZOOM);
	if(view_state.mode == ViewMode::Fit) {
		view_state.zoom = 1.0;
		view_state.pan = Pointf(proxy_size.cx / 2.0, proxy_size.cy / 2.0);
		return;
	}

	Size viewport = GetSize();
	if(viewport.cx <= 0 || viewport.cy <= 0)
		return;
	double scale = std::min((double)viewport.cx / (double)proxy_size.cx, (double)viewport.cy / (double)proxy_size.cy);
	scale = std::max(scale * view_state.zoom, 1e-6);
	double center_x = proxy_size.cx / 2.0;
	double center_y = proxy_size.cy / 2.0;
	double half_view_x = viewport.cx / (2.0 * scale);
	double half_view_y = viewport.cy / (2.0 * scale);
	if(proxy_size.cx * scale <= viewport.cx)
		view_state.pan.x = center_x;
	else
		view_state.pan.x = std::clamp(view_state.pan.x, half_view_x, proxy_size.cx - half_view_x);
	if(proxy_size.cy * scale <= viewport.cy)
		view_state.pan.y = center_y;
	else
		view_state.pan.y = std::clamp(view_state.pan.y, half_view_y, proxy_size.cy - half_view_y);
}

void ImagingCanvas::UpdateProbeFromPoint(Point p)
{
	Point source_point;
	if(ViewToSource(p, source_point)) {
		if(WhenSourcePixelMove)
			WhenSourcePixelMove(source_point);
	}
	else if(WhenSourcePixelLeave) {
		WhenSourcePixelLeave();
	}
}

bool ImagingCanvas::ViewToSource(Point p, Point& source_point) const
{
	if(!view_geometry.IsValid())
		return false;
	Pointf source;
	if(!view_geometry.ViewToSource(Pointf(p.x, p.y), source))
		return false;
	source_point = Point((int)std::llround(source.x), (int)std::llround(source.y));
	return true;
}

void ImagingCanvas::BeginPan(Point p)
{
	if(panning)
		return;
	panning = true;
	SetCapture();
	pan_start_mouse = p;
	pan_start_pan = view_state.pan;
}

void ImagingCanvas::UpdatePan(Point p)
{
	if(!panning)
		return;
	Size viewport = GetSize();
	if(viewport.cx <= 0 || viewport.cy <= 0 || view_geometry.view_scale <= 0.0)
		return;
	Point delta = p - pan_start_mouse;
	view_state.mode = ViewMode::Manual;
	view_state.zoom = std::clamp(view_state.zoom, MIN_ZOOM, MAX_ZOOM);
	view_state.pan = Pointf(pan_start_pan.x - delta.x / view_geometry.view_scale,
		                   pan_start_pan.y - delta.y / view_geometry.view_scale);
	ClampViewState();
	RecomputeGeometry();
	Refresh();
	WhenViewChanged();
}

void ImagingCanvas::EndPan()
{
	if(!panning)
		return;
	panning = false;
	if(HasCapture())
		ReleaseCapture();
}

void ImagingCanvas::ZoomAt(Point p, double factor)
{
	if(!HasImage() || factor <= 0.0)
		return;
	Point focus = p;
	Pointf source_focus;
	if(!view_geometry.ViewToSource(Pointf(focus.x, focus.y), source_focus)) {
		focus = Point(GetSize().cx / 2, GetSize().cy / 2);
		if(!view_geometry.ViewToSource(Pointf(focus.x, focus.y), source_focus))
			return;
	}
	view_state.mode = ViewMode::Manual;
	view_state.zoom = std::clamp(view_state.zoom * factor, MIN_ZOOM, MAX_ZOOM);
	RecomputeGeometry();
	Pointf source_to_display(
		source_size.cx > 1 && proxy_size.cx > 1 ? source_focus.x * (double)(proxy_size.cx - 1) / (double)(source_size.cx - 1) : proxy_size.cx / 2.0,
		source_size.cy > 1 && proxy_size.cy > 1 ? source_focus.y * (double)(proxy_size.cy - 1) / (double)(source_size.cy - 1) : proxy_size.cy / 2.0);
	Pointf viewport_center(GetSize().cx / 2.0, GetSize().cy / 2.0);
	view_state.pan = Pointf(
		source_to_display.x - (focus.x - viewport_center.x) / view_geometry.view_scale,
		source_to_display.y - (focus.y - viewport_center.y) / view_geometry.view_scale);
	ClampViewState();
	RecomputeGeometry();
	Refresh();
	WhenViewChanged();
	UpdateProbeFromPoint(p);
}

void ImagingCanvas::Paint(Draw& w)
{
	Size sz = GetSize();
	w.DrawRect(sz, Color(24, 24, 24));
	const int tile = 16;
	for(int y = 0; y < sz.cy; y += tile)
		for(int x = 0; x < sz.cx; x += tile)
			if(((x / tile) + (y / tile)) & 1)
				w.DrawRect(x, y, tile, tile, Color(34, 34, 34));
	if(IsNull(image)) {
		w.DrawText(20, 20, placeholder, StdFont(), Color(224, 224, 224));
		return;
	}
	if(!view_geometry.image_rect.IsEmpty())
		w.DrawImage(DrawRectFromGeometry(view_geometry.image_rect), image);
}

} // namespace Upp
