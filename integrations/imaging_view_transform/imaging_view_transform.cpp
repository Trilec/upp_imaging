#include "imaging_view_transform.h"

#include <algorithm>
#include <cmath>

namespace Upp {

ImageViewGeometry BuildImageViewGeometry(Size viewport_size, Size display_size, Size source_size, const ImageViewState& state)
{
	ImageViewGeometry geometry;
	geometry.viewport_size = viewport_size;
	geometry.display_size = display_size;
	geometry.source_size = source_size;
	geometry.viewport_rect = Rectf(0, 0, viewport_size.cx, viewport_size.cy);
	if(viewport_size.cx <= 0 || viewport_size.cy <= 0 || display_size.cx <= 0 || display_size.cy <= 0 || source_size.cx <= 0 || source_size.cy <= 0)
		return geometry;

	geometry.fit_scale = std::min((double)viewport_size.cx / (double)display_size.cx, (double)viewport_size.cy / (double)display_size.cy);
	if(geometry.fit_scale <= 0.0)
		geometry.fit_scale = 1.0;
	geometry.view_scale = geometry.fit_scale * (state.mode == ViewMode::Fit ? 1.0 : std::max(0.05, std::min(state.zoom, 32.0)));
	if(geometry.view_scale <= 0.0)
		geometry.view_scale = geometry.fit_scale;

	Pointf center(viewport_size.cx / 2.0, viewport_size.cy / 2.0);
	Pointf pan = state.mode == ViewMode::Fit ? Pointf(display_size.cx / 2.0, display_size.cy / 2.0) : state.pan;
	double img_left = center.x - pan.x * geometry.view_scale;
	double img_top = center.y - pan.y * geometry.view_scale;
	double img_right = img_left + display_size.cx * geometry.view_scale;
	double img_bottom = img_top + display_size.cy * geometry.view_scale;
	geometry.image_rect = Rectf(img_left, img_top, img_right, img_bottom);
	geometry.effective_zoom = geometry.view_scale * (source_size.cx > 1 && display_size.cx > 1 ? (double)(display_size.cx - 1) / (double)(source_size.cx - 1) : 1.0);
	return geometry;
}

bool ImageViewGeometry::ViewToSource(Pointf view_point, Pointf& source_point) const
{
	if(!IsValid() || image_rect.IsEmpty() || !image_rect.Contains(view_point) || view_scale <= 0.0)
		return false;
	double display_x = (view_point.x - image_rect.left) / view_scale;
	double display_y = (view_point.y - image_rect.top) / view_scale;
	double source_scale_x = source_size.cx > 1 && display_size.cx > 1 ? (double)(source_size.cx - 1) / (double)(display_size.cx - 1) : 0.0;
	double source_scale_y = source_size.cy > 1 && display_size.cy > 1 ? (double)(source_size.cy - 1) / (double)(display_size.cy - 1) : 0.0;
	if(source_size.cx <= 1)
		source_point.x = 0.0;
	else
		source_point.x = std::clamp(display_x * source_scale_x, 0.0, (double)(source_size.cx - 1));
	if(source_size.cy <= 1)
		source_point.y = 0.0;
	else
		source_point.y = std::clamp(display_y * source_scale_y, 0.0, (double)(source_size.cy - 1));
	return true;
}

Pointf ImageViewGeometry::SourceToView(Pointf source_point) const
{
	if(!IsValid() || image_rect.IsEmpty() || view_scale <= 0.0)
		return Pointf();
	double display_x = source_size.cx > 1 && display_size.cx > 1 ? source_point.x * (double)(display_size.cx - 1) / (double)(source_size.cx - 1) : 0.0;
	double display_y = source_size.cy > 1 && display_size.cy > 1 ? source_point.y * (double)(display_size.cy - 1) / (double)(source_size.cy - 1) : 0.0;
	return Pointf(image_rect.left + display_x * view_scale, image_rect.top + display_y * view_scale);
}

Rect SourceViewFitRect(Size source_size, Size viewport_size, bool fit_mode)
{
	if(source_size.cx <= 0 || source_size.cy <= 0 || viewport_size.cx <= 0 || viewport_size.cy <= 0)
		return Rect();
	double fit_scale = 1.0;
	if(fit_mode) {
		fit_scale = std::min((double)viewport_size.cx / (double)source_size.cx,
			                 (double)viewport_size.cy / (double)source_size.cy);
		fit_scale = std::min(fit_scale, 1.0);
	}
	if(fit_scale <= 0.0)
		fit_scale = 1.0;
	Size target((int)std::round(source_size.cx * fit_scale), (int)std::round(source_size.cy * fit_scale));
	if(target.cx <= 0 || target.cy <= 0)
		return Rect();
	Point top_left((viewport_size.cx - target.cx) / 2, (viewport_size.cy - target.cy) / 2);
	return RectC(top_left.x, top_left.y, target.cx, target.cy);
}

bool ViewPointToSourcePoint(Point view_point, Rect source_rect, Size source_size, Point& source_point)
{
	if(source_size.cx <= 0 || source_size.cy <= 0 || source_rect.IsEmpty())
		return false;
	if(!source_rect.Contains(view_point))
		return false;

	int displayed_width = source_rect.GetWidth();
	int displayed_height = source_rect.GetHeight();
	if(displayed_width <= 0 || displayed_height <= 0)
		return false;

	int64 relative_x = view_point.x - source_rect.left;
	int64 relative_y = view_point.y - source_rect.top;
	int64 src_w = std::max<int64>(0, source_size.cx - 1);
	int64 src_h = std::max<int64>(0, source_size.cy - 1);
	int64 disp_w = std::max<int64>(1, displayed_width - 1);
	int64 disp_h = std::max<int64>(1, displayed_height - 1);
	int x = source_size.cx == 1 ? 0 : (int)std::llround((double)relative_x * (double)src_w / (double)disp_w);
	int y = source_size.cy == 1 ? 0 : (int)std::llround((double)relative_y * (double)src_h / (double)disp_h);
	x = std::clamp(x, 0, source_size.cx - 1);
	y = std::clamp(y, 0, source_size.cy - 1);
	source_point = Point(x, y);
	return true;
}

} // namespace Upp
