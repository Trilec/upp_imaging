#include <Core/Core.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <vector>

#define private public
#define protected public
#include "../examples/ImagingWorkbench/ImagingWorkbench.h"
#undef protected
#undef private

#include <oiio/OIIO.h>

using namespace Upp;
using namespace UppImaging;

static std::filesystem::path TempRoot()
{
	return std::filesystem::temp_directory_path() / "upp_imaging_workbench_ocio_test";
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
	const std::vector<std::string>& names, int alpha_channel, const std::vector<float>& pixels,
	const char* color_space = nullptr)
{
	OIIO::ImageSpec spec(width, height, channels, OIIO::TypeDesc::FLOAT);
	spec.channelnames = names;
	spec.alpha_channel = alpha_channel;
	if(color_space)
		spec.attribute("oiio:ColorSpace", color_space);
	OIIO::ImageBuf image(spec, (void*)pixels.data());
	std::string error;
	return UppImaging::SaveImage(path.string().c_str(), image, &error);
}

static bool Check(bool condition, const char* label, int& passed, int& failed)
{
	std::printf("%s %s\n", condition ? "PASS" : "FAIL", label);
	condition ? ++passed : ++failed;
	return condition;
}

static String SelectedText(const UiDropdown& drop)
{
	return AsString(drop.GetSelectedData());
}

static void CompareSelection(const char* label, const String& actual, const String& expected, int& passed, int& failed)
{
	Check(actual == expected, label, passed, failed);
}

int main()
{
	int passed = 0;
	int failed = 0;
	UppImaging::InitializeOpenImageIO();

	std::filesystem::path root = TempRoot();
	std::filesystem::remove_all(root);
	std::filesystem::create_directories(root);

	const std::filesystem::path rgba = root / "rgba.exr";
	const std::filesystem::path data = root / "data.exr";
	WriteFixture(rgba, 4, 2, 4, { "R", "G", "B", "A" }, 3, MakePixels(4, 2, 4, 0.2f), "ACEScg");
	WriteFixture(data, 4, 2, 1, { "Y" }, -1, MakePixels(4, 2, 1, 0.4f), nullptr);

	ImagingWorkbench wb;
	String error;
	Check(wb.LoadImageFile(rgba.string().c_str(), error, true), "rgba load", passed, failed);
	wb.ocio_enable_drop.Select(1);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Enable);
	Check(wb.ocio_enabled, "enable OCIO", passed, failed);
	Check(wb.ocio_processor_valid, "processor valid", passed, failed);
	Check(!wb.ocio_source_name.IsEmpty(), "source selected", passed, failed);

	int build0 = wb.ocio_processor.build_count;
	wb.ApplyExposureStops(1.0, true);
	Check(wb.ocio_processor.build_count == build0, "exposure does not rebuild", passed, failed);
	wb.ApplyDisplayGamma(2.2, true);
	Check(wb.ocio_processor.build_count == build0, "gamma does not rebuild", passed, failed);

	String display_before = wb.ocio_display_name;
	String view_before = wb.ocio_view_name;
	if(wb.ocio_source_drop.GetCount() > 2) {
		wb.ocio_source_drop.Select(2);
		wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Source);
		CompareSelection("selected source survives", wb.ocio_source_name, SelectedText(wb.ocio_source_drop), passed, failed);
		Check(wb.ocio_display_name == display_before, "source keeps display", passed, failed);
		Check(wb.ocio_view_name == view_before, "source keeps view", passed, failed);
	}

	int source_build = wb.ocio_processor.build_count;
	if(wb.ocio_display_drop.GetCount() > 1) {
		wb.ocio_display_drop.Select(1);
		wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Display);
		CompareSelection("selected display survives", wb.ocio_display_name, SelectedText(wb.ocio_display_drop), passed, failed);
		Check(wb.ocio_processor.build_count == source_build + 1, "display rebuilds processor", passed, failed);
	}

	if(wb.ocio_view_drop.GetCount() > 1) {
		wb.ocio_view_drop.Select(1);
		wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::View);
		CompareSelection("selected view survives", wb.ocio_view_name, SelectedText(wb.ocio_view_drop), passed, failed);
		Check(wb.ocio_processor.build_count == source_build + 2, "view rebuilds processor", passed, failed);
	}

	if(wb.ocio_config_drop.GetCount() > 2) {
		wb.ocio_config_drop.Select(2);
		wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Config);
		Check(wb.ocio_config_name == SelectedText(wb.ocio_config_drop), "config selection retained", passed, failed);
		Check(wb.ocio_processor.build_count >= source_build + 3, "config rebuilds processor", passed, failed);
	}

	wb.ApplyChannelView(ChannelView::RGB);
	wb.RenderPreviewFromProxy();
	Check(wb.ocio_preview_applied, "OCIO success uses transformed output", passed, failed);

	Image baseline;
	wb.ocio_enable_drop.Select(0);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Enable);
	wb.RenderPreviewFromProxy();
	baseline = wb.preview_image;
	Check(!wb.ocio_preview_applied, "OCIO disabled bypasses processing", passed, failed);

	wb.ocio_enable_drop.Select(1);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Enable);
	wb.ocio_processor.cpu.reset();
	wb.ocio_processor_valid = true;
	wb.RenderPreviewFromProxy();
	Check(!wb.ocio_preview_applied, "OCIO failure falls back", passed, failed);
	Check(!wb.ocio_error_text.IsEmpty(), "OCIO failure shows error", passed, failed);
	Check(wb.preview_image == baseline, "OCIO failure preserves fallback image", passed, failed);

	wb.ocio_error_text.Clear();
	wb.ocio_processor.cpu = OCIO::ConstCPUProcessorRcPtr();

	Check(wb.LoadImageFile(data.string().c_str(), error, true), "data load", passed, failed);
	wb.ocio_enable_drop.Select(1);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Enable);
	wb.selected_preview_group = 0;
	wb.channel_view = ChannelView::RGB;
	wb.RenderPreviewFromProxy();
	Check(!wb.ocio_preview_applied, "data group bypasses OCIO", passed, failed);

	wb.channel_view = ChannelView::Alpha;
	wb.RenderPreviewFromProxy();
	Check(!wb.ocio_preview_applied, "Alpha mode bypasses OCIO", passed, failed);

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
