#include <algorithm>
#include <cmath>
#include <cstdio>

#include <imaging_view_transform/imaging_view_transform.h>

using namespace Upp;

struct Result {
	int passed = 0;
	int failed = 0;
};

static bool Check(Result& result, bool condition, const char* label)
{
	std::printf("%s: %s\n", label, condition ? "PASS" : "FAIL");
	if(condition)
		++result.passed;
	else
		++result.failed;
	return condition;
}

static bool CheckPoint(Result& result, const Point& got, const Point& want, const char* label)
{
	return Check(result, got == want, label);
}

static bool CheckNear(Result& result, double got, double want, double eps, const char* label)
{
	return Check(result, std::fabs(got - want) <= eps, label);
}

static bool CheckPointNear(Result& result, const Pointf& got, const Pointf& want, double eps, const char* label)
{
	return Check(result, std::fabs(got.x - want.x) <= eps && std::fabs(got.y - want.y) <= eps, label);
}

static Pointf CursorCenteredPan(Pointf source_focus, Point cursor, Pointf viewport_center, const ImageViewGeometry& geometry, double new_view_scale)
{
	Pointf display_focus(
		geometry.source_size.cx > 1 && geometry.display_size.cx > 1 ? source_focus.x * (double)(geometry.display_size.cx - 1) / (double)(geometry.source_size.cx - 1) : 0.0,
		geometry.source_size.cy > 1 && geometry.display_size.cy > 1 ? source_focus.y * (double)(geometry.display_size.cy - 1) / (double)(geometry.source_size.cy - 1) : 0.0);
	return Pointf(display_focus.x - (cursor.x - viewport_center.x) / new_view_scale,
		          display_focus.y - (cursor.y - viewport_center.y) / new_view_scale);
}

int main()
{
	Result result;

	Rect fit1 = SourceViewFitRect(Size(1024, 768), Size(1024, 768));
	Point p;
	Check(result, fit1 == RectC(0, 0, 1024, 768), "PASS fit 1024x768 into 1024x768");
	Check(result, ViewPointToSourcePoint(Point(0, 0), fit1, Size(1024, 768), p) && p == Point(0, 0), "PASS top-left mapping");
	Check(result, ViewPointToSourcePoint(Point(1023, 767), fit1, Size(1024, 768), p) && p == Point(1023, 767), "PASS bottom-right mapping");
	Check(result, ViewPointToSourcePoint(Point(512, 384), fit1, Size(1024, 768), p) && p == Point(512, 384), "PASS centre mapping");

	Rect fit2 = SourceViewFitRect(Size(1024, 768), Size(512, 384));
	Check(result, fit2 == RectC(0, 0, 512, 384), "PASS fit 1024x768 into 512x384");
	Check(result, ViewPointToSourcePoint(Point(0, 0), fit2, Size(1024, 768), p) && p == Point(0, 0), "PASS scaled top-left mapping");
	Check(result, ViewPointToSourcePoint(Point(511, 383), fit2, Size(1024, 768), p) && p == Point(1023, 767), "PASS scaled bottom-right mapping");

	Rect fit3 = SourceViewFitRect(Size(1024, 768), Size(333, 250));
	Check(result, fit3 == RectC(0, 0, 333, 250), "PASS fit 1024x768 into 333x250");
	Check(result, ViewPointToSourcePoint(Point(166, 125), fit3, Size(1024, 768), p) && p.x >= 510 && p.x <= 514 && p.y >= 382 && p.y <= 386, "PASS rounded centre mapping");

	Rect fit4 = SourceViewFitRect(Size(8000, 4000), Size(1000, 500));
	Check(result, fit4 == RectC(0, 0, 1000, 500), "PASS fit 8000x4000 into 1000x500");
	Check(result, ViewPointToSourcePoint(Point(0, 0), fit4, Size(8000, 4000), p) && p == Point(0, 0), "PASS large top-left mapping");
	Check(result, ViewPointToSourcePoint(Point(999, 499), fit4, Size(8000, 4000), p) && p == Point(7999, 3999), "PASS large bottom-right mapping");

	Rect letterbox = RectC(100, 50, 1000, 500);
	Check(result, !ViewPointToSourcePoint(Point(99, 50), letterbox, Size(8000, 4000), p), "PASS outside-image rejection left");
	Check(result, !ViewPointToSourcePoint(Point(1100, 550), letterbox, Size(8000, 4000), p), "PASS outside-image rejection bottom-right");

	Check(result, SourceViewFitRect(Size(1, 768), Size(512, 384)) == RectC(255, 0, 1, 384), "PASS one-pixel-wide fit");
	Check(result, SourceViewFitRect(Size(1024, 1), Size(512, 384)) == RectC(0, 191, 512, 1), "PASS one-pixel-high fit");
	Check(result, SourceViewFitRect(Size(1, 1), Size(512, 384)) == RectC(255, 191, 1, 1), "PASS one-pixel source fit");
	Check(result, ViewPointToSourcePoint(Point(255, 191), RectC(255, 191, 1, 1), Size(1, 1), p) && p == Point(0, 0), "PASS one-pixel source mapping");

	Rect resize_fit = SourceViewFitRect(Size(1024, 768), Size(640, 360));
	Check(result, resize_fit == RectC(80, 0, 480, 360), "PASS resize-derived fit");

	ImageViewState fit_state;
	ImageViewGeometry fit_geom = BuildImageViewGeometry(Size(600, 400), Size(1000, 500), Size(4000, 2000), fit_state);
	Check(result, fit_geom.IsValid(), "PASS fit geometry valid");
	CheckNear(result, fit_geom.view_scale, 0.6, 1e-9, "PASS fit view scale");
	CheckNear(result, fit_geom.image_rect.left, 0.0, 1e-9, "PASS fit left aligned");
	CheckNear(result, fit_geom.image_rect.top, 50.0, 1e-9, "PASS fit vertically centered");
	Pointf source;
	Check(result, fit_geom.ViewToSource(Pointf(0.0, 50.0), source), "PASS fit top-left view maps");
	CheckPointNear(result, source, Pointf(0.0, 0.0), 1e-6, "PASS fit top-left source");
	Pointf source_corner(3599.0, 1799.0);
	Pointf view_corner = fit_geom.SourceToView(source_corner);
	Check(result, view_corner.x > fit_geom.image_rect.left + fit_geom.image_rect.Width() * 0.7, "PASS fit bottom-right view maps");
	Check(result, view_corner.y > fit_geom.image_rect.top + fit_geom.image_rect.Height() * 0.7, "PASS fit bottom-right source");

	ImageViewGeometry portrait_geom = BuildImageViewGeometry(Size(600, 400), Size(500, 1000), Size(2000, 4000), fit_state);
	Check(result, portrait_geom.IsValid(), "PASS portrait geometry valid");
	CheckNear(result, portrait_geom.image_rect.left, 200.0, 1e-9, "PASS portrait letterbox left");
	CheckNear(result, portrait_geom.image_rect.top, 0.0, 1e-9, "PASS portrait top aligned");

	ImageViewState manual_state;
	manual_state.mode = ViewMode::Manual;
	manual_state.zoom = 2.0;
	manual_state.pan = Pointf(1000.0, 500.0);
	ImageViewGeometry manual_geom = BuildImageViewGeometry(Size(600, 400), Size(1000, 500), Size(4000, 2000), manual_state);
	Check(result, manual_geom.IsValid(), "PASS manual geometry valid");
	Pointf manual_source(2500.0, 1000.0);
	Pointf manual_view = manual_geom.SourceToView(manual_source);
	Pointf manual_roundtrip;
	Check(result, manual_geom.ViewToSource(manual_view, manual_roundtrip), "PASS manual source-to-view mapping");
	CheckPointNear(result, manual_roundtrip, manual_source, 1.0, "PASS manual round trip");

	Point cursor(123, 87);
	Pointf viewport_center(300.0, 200.0);
	Pointf cursor_source;
	Check(result, manual_geom.ViewToSource(Pointf(cursor.x, cursor.y), cursor_source), "PASS cursor source capture");
	ImageViewState zoomed_state = manual_state;
	zoomed_state.zoom = std::clamp(zoomed_state.zoom * 1.1, 0.05, 32.0);
	zoomed_state.pan = CursorCenteredPan(cursor_source, cursor, viewport_center, manual_geom, manual_geom.fit_scale * zoomed_state.zoom);
	ImageViewGeometry zoomed_geom = BuildImageViewGeometry(Size(600, 400), Size(1000, 500), Size(4000, 2000), zoomed_state);
	Pointf cursor_roundtrip;
	Check(result, zoomed_geom.ViewToSource(Pointf(cursor.x, cursor.y), cursor_roundtrip), "PASS cursor-centred zoom maps");
	CheckPointNear(result, cursor_roundtrip, cursor_source, 1.0, "PASS cursor-centred zoom invariant");

	ImageViewState min_state = manual_state;
	min_state.zoom = 0.001;
	ImageViewGeometry min_geom = BuildImageViewGeometry(Size(600, 400), Size(1000, 500), Size(4000, 2000), min_state);
	CheckNear(result, min_geom.view_scale / min_geom.fit_scale, 0.05, 1e-9, "PASS minimum zoom clamp");
	ImageViewState max_state = manual_state;
	max_state.zoom = 1000.0;
	ImageViewGeometry max_geom = BuildImageViewGeometry(Size(600, 400), Size(1000, 500), Size(4000, 2000), max_state);
	CheckNear(result, max_geom.view_scale / max_geom.fit_scale, 32.0, 1e-9, "PASS maximum zoom clamp");

	ImageViewGeometry reduced_geom = BuildImageViewGeometry(Size(600, 400), Size(500, 250), Size(2000, 1000), fit_state);
	Pointf reduced_source;
	Check(result, reduced_geom.ViewToSource(reduced_geom.SourceToView(Pointf(0.0, 0.0)), reduced_source), "PASS reduced proxy top-left maps");
	CheckPointNear(result, reduced_source, Pointf(0.0, 0.0), 1e-6, "PASS reduced proxy source origin");
	Pointf reduced_corner(1799.0, 899.0);
	Pointf reduced_view = reduced_geom.SourceToView(reduced_corner);
	Check(result, reduced_view.x > reduced_geom.image_rect.left + reduced_geom.image_rect.Width() * 0.7, "PASS reduced proxy bottom-right maps");
	Check(result, reduced_view.y > reduced_geom.image_rect.top + reduced_geom.image_rect.Height() * 0.7, "PASS reduced proxy full source mapping");

	std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
	return result.failed ? 1 : 0;
}
