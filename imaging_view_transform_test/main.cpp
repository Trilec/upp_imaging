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

	std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
	return result.failed ? 1 : 0;
}
