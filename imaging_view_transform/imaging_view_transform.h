#ifndef _imaging_view_transform_imaging_view_transform_h_
#define _imaging_view_transform_imaging_view_transform_h_

#include <Core/Core.h>

namespace Upp {

Rect SourceViewFitRect(Size source_size, Size viewport_size, bool fit_mode = true);
bool ViewPointToSourcePoint(Point view_point, Rect source_rect, Size source_size, Point& source_point);

} // namespace Upp

#endif
