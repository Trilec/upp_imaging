#ifndef _imaging_view_transform_imaging_view_transform_h_
#define _imaging_view_transform_imaging_view_transform_h_

#include <Core/Core.h>

namespace Upp {

enum class ViewMode {
	Fit,
	Manual,
};

struct ImageViewState {
	ViewMode mode = ViewMode::Fit;
	double zoom = 1.0;
	Pointf pan;
};

struct ImageViewGeometry {
	Size viewport_size;
	Size display_size;
	Size source_size;
	Rectf viewport_rect;
	Rectf image_rect;
	double fit_scale = 0.0;
	double view_scale = 0.0;
	double effective_zoom = 0.0;

	bool IsValid() const { return viewport_size.cx > 0 && viewport_size.cy > 0 && display_size.cx > 0 && display_size.cy > 0 && source_size.cx > 0 && source_size.cy > 0; }
	bool ViewToSource(Pointf view_point, Pointf& source_point) const;
	Pointf SourceToView(Pointf source_point) const;
};

ImageViewGeometry BuildImageViewGeometry(Size viewport_size, Size display_size, Size source_size, const ImageViewState& state);

Rect SourceViewFitRect(Size source_size, Size viewport_size, bool fit_mode = true);
bool ViewPointToSourcePoint(Point view_point, Rect source_rect, Size source_size, Point& source_point);

} // namespace Upp

#endif
