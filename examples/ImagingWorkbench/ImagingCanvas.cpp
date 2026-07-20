#include "ImagingCanvas.h"

#include <imaging_view_transform/imaging_view_transform.h>

#include <algorithm>
#include <cmath>

namespace Upp {

ImagingCanvas::ImagingCanvas()
{
	SetMinSize(Size(DPI(1), DPI(1)));
}

void ImagingCanvas::SetImage(const Image& value)
{
	SetDisplayImage(value, IsNull(value) ? Size() : value.GetSize());
}

void ImagingCanvas::SetDisplayImage(const Image& value, Size original_source_size)
{
	image = value;
	proxy_size = IsNull(image) ? Size() : image.GetSize();
	source_size = original_source_size;
	if(source_size.cx <= 0 || source_size.cy <= 0)
		source_size = proxy_size;
	UpdateViewState();
	Refresh();
	WhenViewChanged();
}

void ImagingCanvas::ClearImage()
{
	image = Image();
	proxy_size = Size();
	source_size = Size();
	displayed_scale = 0.0;
	image_rect = Rect();
	Refresh();
	WhenViewChanged();
}

void ImagingCanvas::SetFitMode(bool fit)
{
	fit_mode = fit;
	UpdateViewState();
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
	return displayed_scale;
}

void ImagingCanvas::SetPlaceholderText(const String& text)
{
	placeholder = text;
	Refresh();
}

void ImagingCanvas::Layout()
{
	Ctrl::Layout();
	UpdateViewState();
	WhenViewChanged();
}

void ImagingCanvas::MouseMove(Point p, dword keyflags)
{
	Point source_point;
	if(ViewPointToSourcePoint(p, image_rect, source_size, source_point)) {
		if(WhenSourcePixelMove)
			WhenSourcePixelMove(source_point);
	}
	else if(WhenSourcePixelLeave) {
		WhenSourcePixelLeave();
	}
	Ctrl::MouseMove(p, keyflags);
}

void ImagingCanvas::MouseLeave()
{
	if(WhenSourcePixelLeave)
		WhenSourcePixelLeave();
	Ctrl::MouseLeave();
}

void ImagingCanvas::UpdateViewState()
{
	Size sz = GetSize();
	if(IsNull(image) || proxy_size.cx <= 0 || proxy_size.cy <= 0 || source_size.cx <= 0 || source_size.cy <= 0 || sz.cx <= 0 || sz.cy <= 0) {
		displayed_scale = 0.0;
		image_rect = Rect();
		return;
	}

	double scale = 1.0;
	if(fit_mode) {
		scale = std::min((double)sz.cx / (double)source_size.cx, (double)sz.cy / (double)source_size.cy);
		scale = std::min(scale, 1.0);
	}
	if(scale <= 0.0)
		scale = 1.0;
	displayed_scale = scale;
	Size target((int)std::round(source_size.cx * scale), (int)std::round(source_size.cy * scale));
	if(target.cx <= 0 || target.cy <= 0) {
		image_rect = Rect();
		return;
	}
	Point top_left((sz.cx - target.cx) / 2, (sz.cy - target.cy) / 2);
	image_rect = RectC(top_left.x, top_left.y, target.cx, target.cy);
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
	if(!image_rect.IsEmpty())
		w.DrawImage(image_rect, image);
}

} // namespace Upp
