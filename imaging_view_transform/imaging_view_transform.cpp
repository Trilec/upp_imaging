#include "imaging_view_transform.h"

#include <algorithm>
#include <cmath>

namespace Upp {

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
