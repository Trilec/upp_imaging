#include <Core/Core.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <utility>
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

struct RenderSample {
	double processor = 0.0;
	double buffer = 0.0;
	double ocio = 0.0;
	double tone = 0.0;
	double convert = 0.0;
	double publish = 0.0;
	double total = 0.0;
};

static std::vector<RenderSample> Measure(ImagingWorkbench& wb, int runs)
{
	using Clock = std::chrono::steady_clock;
	std::vector<RenderSample> samples;
	samples.reserve(runs);
	for(int i = 0; i < runs; ++i) {
		auto start = Clock::now();
		wb.RenderPreviewFromProxy();
		RenderSample sample;
		sample.total = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
		sample.processor = wb.preview_timing.processor_ms;
		sample.buffer = wb.preview_timing.buffer_ms;
		sample.ocio = wb.preview_timing.ocio_ms;
		sample.tone = wb.preview_timing.buffer_ms;
		sample.convert = wb.preview_timing.convert_ms;
		sample.publish = wb.preview_timing.publish_ms;
		samples.push_back(sample);
	}
	return samples;
}

static void PrintStats(const char* build, const char* source, const char* proxy, const char* mode, const std::vector<RenderSample>& samples)
{
	std::vector<double> processor;
	std::vector<double> buffer;
	std::vector<double> ocio;
	std::vector<double> totals;
	std::vector<double> convert;
	std::vector<double> publish;
	processor.reserve(samples.size());
	buffer.reserve(samples.size());
	ocio.reserve(samples.size());
	totals.reserve(samples.size());
	convert.reserve(samples.size());
	publish.reserve(samples.size());
	for(const RenderSample& sample : samples) {
		processor.push_back(sample.processor);
		buffer.push_back(sample.buffer);
		ocio.push_back(sample.ocio);
		totals.push_back(sample.total);
		convert.push_back(sample.convert);
		publish.push_back(sample.publish);
	}
	std::vector<double> sorted = totals;
	std::sort(sorted.begin(), sorted.end());
	std::printf("TIMING build=%s source=%s proxy=%s mode=%s runs=%d min=%.2f median=%.2f max=%.2f\n",
		build, source, proxy, mode, (int)sorted.size(), sorted.front(), Median(sorted), sorted.back());
	std::printf("PHASE build=%s source=%s proxy=%s mode=%s processor=%.2f buffer=%.2f ocio=%.2f convert=%.2f publish=%.2f total=%.2f\n",
		build, source, proxy, mode, Median(processor), Median(buffer), Median(ocio), Median(convert), Median(publish), Median(totals));
}

static const char* BuildLabel()
{
#ifdef _DEBUG
	return "Debug";
#else
	return "Release";
#endif
}

static void SetOcioEnabled(ImagingWorkbench& wb, bool enabled)
{
	wb.ocio_enable_drop.Select(enabled ? 1 : 0);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Enable);
}

static void SelectOcioBuiltin(ImagingWorkbench& wb, int index)
{
	wb.ocio_config_drop.Select(index);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Config);
}

static void PrintSingleStats(const char* build, const char* source, const char* proxy, const char* mode, const ImagingWorkbench& wb)
{
	std::printf("TIMING build=%s source=%s proxy=%s mode=%s runs=1 min=%.2f median=%.2f max=%.2f\n",
		build, source, proxy, mode, wb.preview_timing.total_ms, wb.preview_timing.total_ms, wb.preview_timing.total_ms);
	std::printf("PHASE build=%s source=%s proxy=%s mode=%s processor=%.2f buffer=%.2f ocio=%.2f convert=%.2f publish=%.2f total=%.2f\n",
		build, source, proxy, mode, wb.preview_timing.processor_ms, wb.preview_timing.buffer_ms,
		wb.preview_timing.ocio_ms, wb.preview_timing.convert_ms, wb.preview_timing.publish_ms, wb.preview_timing.total_ms);
}

int main(int argc, char** argv)
{
	bool quick = argc > 1 && String(argv[1]) == "--quick";
	InitializeOpenImageIO();
	std::filesystem::path root = TempRoot();
	std::filesystem::remove_all(root);
	std::filesystem::create_directories(root);

	const std::filesystem::path mid = root / "mid.exr";
	const std::filesystem::path small = root / "small.exr";
	const std::filesystem::path large = root / "large.exr";
	WriteFixture(mid, 800, 600, 4, { "R", "G", "B", "A" }, 3, MakePixels(800, 600, 4, 0.15f));
	WriteFixture(small, 1000, 700, 4, { "R", "G", "B", "A" }, 3, MakePixels(1000, 700, 4, 0.1f));
	WriteFixture(large, 2000, 1000, 4, { "R", "G", "B", "A" }, 3, MakePixels(2000, 1000, 4, 0.2f));

	String error;
	int runs = quick ? 1 : 3;
	const std::pair<const char*, const std::filesystem::path> fixtures[] = {
		{ "800x600", mid },
		{ "1000x700", small },
		{ "2000x1000", large },
	};
	for(const auto& fixture : fixtures) {
		ImagingWorkbench wb;
		String path = fixture.second.string().c_str();
		wb.LoadImageFile(path, error, true);
		wb.ApplyChannelView(ChannelView::RGB);

		SetOcioEnabled(wb, false);
		wb.preview_timing.processor_ms = 0.0;
		PrintStats(BuildLabel(), fixture.first, fixture.first, "OCIO off RGB", Measure(wb, runs));

		SetOcioEnabled(wb, true);
		SelectOcioBuiltin(wb, 1);
		wb.ApplyChannelView(ChannelView::RGB);
		wb.preview_timing.processor_ms = 0.0;
		PrintStats(BuildLabel(), fixture.first, fixture.first, "OCIO on RGB", Measure(wb, runs));

		wb.ApplyExposureStops(2.0, true);
		wb.preview_timing.processor_ms = 0.0;
		PrintStats(BuildLabel(), fixture.first, fixture.first, "OCIO on exposure", Measure(wb, runs));

		wb.ApplyDisplayGamma(2.2, true);
		wb.preview_timing.processor_ms = 0.0;
		PrintStats(BuildLabel(), fixture.first, fixture.first, "OCIO on gamma", Measure(wb, runs));

		if(wb.ocio_source_drop.GetCount() > 1) {
			wb.ocio_source_drop.Select(1);
			wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Source);
			PrintSingleStats(BuildLabel(), fixture.first, fixture.first, "OCIO selection change", wb);
		}
	}

	ImagingCanvas canvas;
	ImageBuffer image(Size(1000, 700));
	canvas.SetRect(0, 0, 1000, 700);
	canvas.SetDisplayImage(image, Size(1000, 700));
	canvas.SetFitMode(true);
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
