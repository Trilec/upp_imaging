#include <Core/Core.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <vector>

#define private public
#define protected public
#include "../examples/ImagingWorkbench/ImagingWorkbench.h"
#include "../examples/ImagingWorkbench/ImagingCanvas.h"
#undef protected
#undef private

#include <imaging_view_transform/imaging_view_transform.h>
#include <oiio/OIIO.h>

using namespace Upp;
using namespace UppImaging;

static std::filesystem::path TempRoot()
{
	return std::filesystem::temp_directory_path() / "upp_imaging_bench";
}

static std::vector<float> MakePixels(int width, int height, int channels, float seed)
{
	std::vector<float> pixels((size_t)width * height * channels);
	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			float* p = pixels.data() + ((size_t)y * width + x) * channels;
			float fx = (float)x / std::max(1, width - 1);
			float fy = (float)y / std::max(1, height - 1);
			p[0] = seed + fx;
			if(channels > 1) p[1] = seed + fy;
			if(channels > 2) p[2] = seed + 0.25f + 0.5f * fx;
			if(channels > 3) p[3] = 0.3f + 0.7f * fy;
		}
	}
	return pixels;
}

static bool WriteFixture(const std::filesystem::path& path, int width, int height, int channels,
					 const std::vector<std::string>& names, int alpha_channel,
					 const std::vector<float>& pixels)
{
	OIIO::ImageSpec spec(width, height, channels, OIIO::TypeDesc::FLOAT);
	spec.channelnames = names;
	spec.alpha_channel = alpha_channel;
	OIIO::ImageBuf image(spec, (void*)pixels.data());
	std::string error;
	return SaveImage(path.string().c_str(), image, &error);
}

static double Median(std::vector<double> values)
{
	if(values.empty())
		return 0.0;
	std::sort(values.begin(), values.end());
	return values[values.size() / 2];
}

static std::vector<double> Measure(ImagingWorkbench& wb, int runs)
{
	using Clock = std::chrono::steady_clock;
	std::vector<double> times;
	times.reserve(runs);
	for(int i = 0; i < runs; ++i) {
		auto start = Clock::now();
		wb.RenderPreviewFromProxy();
		times.push_back(std::chrono::duration<double, std::milli>(Clock::now() - start).count());
	}
	return times;
}

static void PrintStats(const char* build, const char* source, const char* proxy, const char* mode, const std::vector<double>& times)
{
	std::vector<double> sorted = times;
	std::sort(sorted.begin(), sorted.end());
	std::printf("TIMING build=%s source=%s proxy=%s mode=%s runs=%d min=%.2f median=%.2f max=%.2f\n",
		build, source, proxy, mode, (int)sorted.size(), sorted.front(), Median(sorted), sorted.back());
}

static const char* BuildLabel()
{
#ifdef _DEBUG
	return "Debug";
#else
	return "Release";
#endif
}

int main()
{
	InitializeOpenImageIO();
	std::filesystem::path root = TempRoot();
	std::filesystem::remove_all(root);
	std::filesystem::create_directories(root);

	const std::filesystem::path small = root / "small.exr";
	const std::filesystem::path large = root / "large.exr";
	WriteFixture(small, 1000, 700, 4, { "R", "G", "B", "A" }, 3, MakePixels(1000, 700, 4, 0.1f));
	WriteFixture(large, 2000, 1000, 4, { "R", "G", "B", "A" }, 3, MakePixels(2000, 1000, 4, 0.2f));

	String error;
	ImagingWorkbench small_wb;
	String small_path = small.string().c_str();
	small_wb.LoadImageFile(small_path, error, true);
	for(ChannelView view : { ChannelView::RGB, ChannelView::Red, ChannelView::Green, ChannelView::Blue, ChannelView::Alpha }) {
		small_wb.ApplyChannelView(view);
		PrintStats(BuildLabel(), "1000x700", "1000x700", view == ChannelView::RGB ? "RGB" : view == ChannelView::Red ? "R" : view == ChannelView::Green ? "G" : view == ChannelView::Blue ? "B" : "A",
			Measure(small_wb, 5));
	}
	small_wb.ApplyExposureStops(2.0, true);
	PrintStats(BuildLabel(), "1000x700", "1000x700", "exposure", Measure(small_wb, 5));
	small_wb.ApplyDisplayGamma(2.2, true);
	PrintStats(BuildLabel(), "1000x700", "1000x700", "gamma", Measure(small_wb, 5));

	ImagingWorkbench large_wb;
	String large_path = large.string().c_str();
	large_wb.LoadImageFile(large_path, error, true);
	for(ChannelView view : { ChannelView::RGB, ChannelView::Red, ChannelView::Green, ChannelView::Blue, ChannelView::Alpha }) {
		large_wb.ApplyChannelView(view);
		PrintStats(BuildLabel(), "2000x1000", "2000x1000", view == ChannelView::RGB ? "RGB" : view == ChannelView::Red ? "R" : view == ChannelView::Green ? "G" : view == ChannelView::Blue ? "B" : "A",
			Measure(large_wb, 5));
	}
	large_wb.ApplyExposureStops(2.0, true);
	PrintStats(BuildLabel(), "2000x1000", "2000x1000", "exposure", Measure(large_wb, 5));
	large_wb.ApplyDisplayGamma(2.2, true);
	PrintStats(BuildLabel(), "2000x1000", "2000x1000", "gamma", Measure(large_wb, 5));

	ImagingCanvas canvas;
	ImageBuffer image(Size(1000, 700));
	canvas.SetRect(0, 0, 1000, 700);
	canvas.SetDisplayImage(image, Size(1000, 700));
	canvas.SetFitMode(true);
	canvas.Layout();
	Point source_point;
	bool ok = ViewPointToSourcePoint(Point(0, 0), RectC(0, 0, 1000, 700), Size(1000, 700), source_point);
	std::printf("ROW top-left source=1000x700 proxy=1000x700 display=0,0,1000,700 zoom=100%% mouse=0,0 result=%d,%d %s\n", source_point.x, source_point.y, ok && source_point == Point(0, 0) ? "PASS" : "FAIL");
	ok = ViewPointToSourcePoint(Point(999, 699), RectC(0, 0, 1000, 700), Size(1000, 700), source_point);
	std::printf("ROW bottom-right source=1000x700 proxy=1000x700 display=0,0,1000,700 zoom=100%% mouse=999,699 result=%d,%d %s\n", source_point.x, source_point.y, ok && source_point == Point(999, 699) ? "PASS" : "FAIL");
	ok = ViewPointToSourcePoint(Point(500, 350), RectC(0, 0, 1000, 700), Size(1000, 700), source_point);
	std::printf("ROW centre source=1000x700 proxy=1000x700 display=0,0,1000,700 zoom=100%% mouse=500,350 result=%d,%d %s\n", source_point.x, source_point.y, ok && source_point == Point(500, 350) ? "PASS" : "FAIL");
	Rect letterbox = SourceViewFitRect(Size(1000, 700), Size(800, 700), true);
	ok = ViewPointToSourcePoint(Point(10, 10), letterbox, Size(1000, 700), source_point);
	std::printf("ROW letterbox margin source=1000x700 proxy=1000x700 display=%d,%d,%d,%d zoom=80%% mouse=10,10 result=%s\n",
		letterbox.left, letterbox.top, letterbox.right, letterbox.bottom, ok ? "unexpected" : "cleared");
	Rect reduced = SourceViewFitRect(Size(1000, 700), Size(500, 350), true);
	ok = ViewPointToSourcePoint(Point(reduced.left, reduced.top), reduced, Size(1000, 700), source_point);
	std::printf("ROW reduced proxy source=1000x700 proxy=500x350 display=%d,%d,%d,%d zoom=50%% mouse=%d,%d result=%d,%d %s\n",
		reduced.left, reduced.top, reduced.right, reduced.bottom, reduced.left, reduced.top, source_point.x, source_point.y,
		ok && source_point == Point(0, 0) ? "PASS" : "FAIL");

	return 0;
}
