#include <Core/Core.h>

#include <algorithm>
#include <cmath>
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

static bool WriteCurveLut(const std::filesystem::path& path)
{
	std::ofstream out(path, std::ios::binary);
	if(!out)
		return false;
	out << "LUT_1D_SIZE 4\n";
	out << "DOMAIN_MIN 0.0 0.0 0.0\n";
	out << "DOMAIN_MAX 1.0 1.0 1.0\n";
	out << "0.0 0.0 0.0\n";
	out << "0.1 0.1 0.1\n";
	out << "0.5 0.5 0.5\n";
	out << "1.0 1.0 1.0\n";
	return out.good();
}

struct ImageSnapshot {
	int width = 0;
	int height = 0;
	int nchannels = 0;
	int alpha_channel = -1;
	OIIO::TypeDesc format;
	std::vector<std::string> channelnames;
	std::vector<float> pixels;
};

struct WorkbenchSnapshot {
	bool ocio_enabled = false;
	bool ocio_processor_valid = false;
	bool ocio_preview_applied = false;
	int selected_preview_group = -1;
	ChannelView channel_view = ChannelView::RGB;
	double exposure_stops = 0.0;
	double display_gamma = 1.0;
	String ocio_config_name;
	String ocio_source_name;
	String ocio_display_name;
	String ocio_view_name;
	String ocio_look_name;
	String ocio_lut_name;
	String ocio_lut_path;
	String ocio_lut_requested_path;
	String ocio_lut_direction;
	OcioConfigSource ocio_config_source = OcioConfigSource::Builtin;
	ImageSnapshot source_image;
};

static bool CaptureImageSnapshot(const OIIO::ImageBuf& image, ImageSnapshot& snapshot)
{
	const OIIO::ImageSpec& spec = image.spec();
	snapshot.width = spec.width;
	snapshot.height = spec.height;
	snapshot.nchannels = spec.nchannels;
	snapshot.alpha_channel = spec.alpha_channel;
	snapshot.format = spec.format;
	snapshot.channelnames = spec.channelnames;
	snapshot.pixels.resize((size_t)spec.width * spec.height * spec.nchannels);
	if(!snapshot.pixels.empty()) {
		if(!image.get_pixels(image.roi(), OIIO::TypeDesc::FLOAT, snapshot.pixels.data(),
				spec.nchannels * sizeof(float), spec.width * spec.nchannels * sizeof(float), OIIO::AutoStride)) {
			return false;
		}
	}
	return true;
}

static bool SameImageSpec(const ImageSnapshot& a, const ImageSnapshot& b, bool compare_names)
{
	if(a.width != b.width || a.height != b.height || a.nchannels != b.nchannels || a.alpha_channel != b.alpha_channel || a.format != b.format)
		return false;
	if(compare_names && a.channelnames != b.channelnames)
		return false;
	return true;
}

static bool SameImagePixels(const ImageSnapshot& a, const ImageSnapshot& b, float tolerance)
{
	if(a.pixels.size() != b.pixels.size())
		return false;
	for(size_t i = 0; i < a.pixels.size(); ++i) {
		if(std::fabs(a.pixels[i] - b.pixels[i]) > tolerance)
			return false;
	}
	return true;
}

static bool SameImageSnapshot(const ImageSnapshot& a, const ImageSnapshot& b, float tolerance, bool compare_names)
{
	return SameImageSpec(a, b, compare_names) && SameImagePixels(a, b, tolerance);
}

static bool CaptureWorkbenchSnapshot(const ImagingWorkbench& wb, WorkbenchSnapshot& snapshot)
{
	snapshot.ocio_enabled = wb.ocio_enabled;
	snapshot.ocio_processor_valid = wb.ocio_processor_valid;
	snapshot.ocio_preview_applied = wb.ocio_preview_applied;
	snapshot.selected_preview_group = wb.selected_preview_group;
	snapshot.channel_view = wb.channel_view;
	snapshot.exposure_stops = wb.exposure_stops;
	snapshot.display_gamma = wb.display_gamma;
	snapshot.ocio_config_name = wb.ocio_config_name;
	snapshot.ocio_source_name = wb.ocio_source_name;
	snapshot.ocio_display_name = wb.ocio_display_name;
	snapshot.ocio_view_name = wb.ocio_view_name;
	snapshot.ocio_look_name = wb.ocio_look_name;
	snapshot.ocio_lut_name = wb.ocio_lut_name;
	snapshot.ocio_lut_path = wb.ocio_lut_path;
	snapshot.ocio_lut_requested_path = wb.ocio_lut_requested_path;
	snapshot.ocio_lut_direction = AsString(wb.ocio_lut_direction_drop.GetSelectedData());
	snapshot.ocio_config_source = wb.ocio_config_source;
	return CaptureImageSnapshot(wb.source_image, snapshot.source_image);
}

static bool SameWorkbenchSnapshot(const WorkbenchSnapshot& a, const WorkbenchSnapshot& b)
{
	return a.ocio_enabled == b.ocio_enabled
		&& a.ocio_processor_valid == b.ocio_processor_valid
		&& a.ocio_preview_applied == b.ocio_preview_applied
		&& a.selected_preview_group == b.selected_preview_group
		&& a.channel_view == b.channel_view
		&& a.exposure_stops == b.exposure_stops
		&& a.display_gamma == b.display_gamma
		&& a.ocio_config_name == b.ocio_config_name
		&& a.ocio_source_name == b.ocio_source_name
		&& a.ocio_display_name == b.ocio_display_name
		&& a.ocio_view_name == b.ocio_view_name
		&& a.ocio_look_name == b.ocio_look_name
		&& a.ocio_lut_name == b.ocio_lut_name
		&& a.ocio_lut_path == b.ocio_lut_path
		&& a.ocio_lut_requested_path == b.ocio_lut_requested_path
		&& a.ocio_lut_direction == b.ocio_lut_direction
		&& a.ocio_config_source == b.ocio_config_source
		&& SameImageSnapshot(a.source_image, b.source_image, 0.0f, true);
}

static int FirstDifferentIndex(const Vector<String>& values, const String& preferred)
{
	for(int i = 0; i < values.GetCount(); ++i) {
		if(values[i] != preferred)
			return i;
	}
	return values.IsEmpty() ? -1 : 0;
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
	Check(wb.ocio_config_source_drop.GetCount() == 3, "config source options", passed, failed);
	Check(wb.ocio_look_drop.GetCount() >= 1, "look options populated", passed, failed);
	Check(wb.GetOcioSummary().Find("(") >= 0, "metadata/default/user source labels", passed, failed);

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
		Check(wb.GetOcioSummary().Find("(user)") >= 0, "user source label", passed, failed);
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

	if(wb.ocio_look_drop.GetCount() > 1) {
		wb.ocio_look_drop.Select(1);
		wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Look);
		Check(wb.ocio_look_name == SelectedText(wb.ocio_look_drop), "look selection retained", passed, failed);
	}

	std::filesystem::path lut_path = root / "workbench_curve.cube";
	Check(WriteCurveLut(lut_path), "lut fixture written", passed, failed);
	int lut_build = wb.ocio_processor.build_count;
	wb.ocio_lut_path = lut_path.string().c_str();
	wb.ocio_lut_requested_path.Clear();
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Lut);
	Check(wb.ocio_processor.build_count == lut_build + 1, "lut load rebuilds once", passed, failed);
	lut_build = wb.ocio_processor.build_count;
	wb.ocio_lut_direction_drop.Select(1);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::LutDirection);
	Check(wb.ocio_processor.build_count == lut_build + 1, "lut direction rebuilds once", passed, failed);
	lut_build = wb.ocio_processor.build_count;
	wb.ocio_lut_path.Clear();
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::LutClear);
	Check(wb.ocio_lut_path.IsEmpty(), "clear LUT", passed, failed);
	Check(wb.ocio_processor.build_count == lut_build + 1, "clear LUT rebuilds once", passed, failed);

	String save_error;
	String config_error;
	std::string load_error;
	std::filesystem::path file_config_path = std::filesystem::path("opencolorio_src/upstream/src/OpenColorIO/builtinconfigs/configs") / "studio-config-v4.0.0_aces-v2.0_ocio-v2.5.ocio";
	OCIO::ConstConfigRcPtr file_config;
	String file_identity;
	Check(OcioPreview::LoadConfigFile(file_config_path.string().c_str(), file_config, config_error, file_identity), "file config load for A/B save", passed, failed);

	wb.ocio_look_drop.Clear();
	wb.ocio_look_drop.Add("none", String());
	wb.ocio_look_drop.Select(0);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Look);
	wb.ocio_enable_drop.Select(0);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Enable);
	wb.ocio_look_name.Clear();
	wb.ocio_look_requested_name.Clear();
	wb.ocio_look_drop.Clear();
	wb.ocio_look_drop.Add("none", String());
	wb.ocio_look_drop.Select(0);
	wb.ApplyExposureStops(0.0, true);
	wb.ApplyDisplayGamma(1.0, true);
	wb.ApplyChannelView(ChannelView::RGB);
	wb.RenderPreviewFromProxy();
	Check(!wb.ocio_enabled, "baseline OCIO disabled", passed, failed);
	Check(wb.exposure_stops == 0.0, "baseline exposure zero", passed, failed);
	Check(wb.display_gamma == 1.0, "baseline gamma default", passed, failed);
	Check(wb.channel_view == ChannelView::RGB, "baseline channel view RGB", passed, failed);
	Check(wb.ocio_look_name.IsEmpty(), "baseline look cleared", passed, failed);
	Check(wb.ocio_lut_path.IsEmpty(), "baseline LUT cleared", passed, failed);
	Check(!wb.ocio_preview_applied, "baseline preview bypassed", passed, failed);

	WorkbenchSnapshot baseline_state;
	Check(CaptureWorkbenchSnapshot(wb, baseline_state), "capture baseline save state", passed, failed);

	std::filesystem::path baseline_exr = root / "baseline.exr";
	String baseline_exr_path = baseline_exr.string().c_str();
	Check(wb.SaveCurrentImage(baseline_exr_path, "EXR", save_error), "baseline EXR save", passed, failed);
	WorkbenchSnapshot baseline_after_exr;
	Check(CaptureWorkbenchSnapshot(wb, baseline_after_exr), "capture EXR post-save state", passed, failed);
	Check(SameWorkbenchSnapshot(baseline_state, baseline_after_exr), "EXR save leaves source and viewer state unchanged", passed, failed);

	std::filesystem::path baseline_png = root / "baseline.png";
	String baseline_png_path = baseline_png.string().c_str();
	Check(wb.SaveCurrentImage(baseline_png_path, "PNG", save_error), "baseline PNG save", passed, failed);
	WorkbenchSnapshot baseline_after_png;
	Check(CaptureWorkbenchSnapshot(wb, baseline_after_png), "capture PNG post-save state", passed, failed);
	Check(SameWorkbenchSnapshot(baseline_state, baseline_after_png), "PNG save leaves source and viewer state unchanged", passed, failed);

	OIIO::ImageBuf baseline_exr_image;
	Check(UppImaging::LoadImage(baseline_exr_path.Begin(), baseline_exr_image, &load_error), "baseline EXR reopen", passed, failed);
	OIIO::ImageBuf baseline_png_image;
	Check(UppImaging::LoadImage(baseline_png_path.Begin(), baseline_png_image, &load_error), "baseline PNG reopen", passed, failed);
	ImageSnapshot baseline_exr_snapshot;
	ImageSnapshot baseline_png_snapshot;
	Check(CaptureImageSnapshot(baseline_exr_image, baseline_exr_snapshot), "capture baseline EXR snapshot", passed, failed);
	Check(CaptureImageSnapshot(baseline_png_image, baseline_png_snapshot), "capture baseline PNG snapshot", passed, failed);

	if(file_config) {
	Vector<String> source_names = OcioPreview::GetColorSpaceNames(file_config);
	Vector<String> display_names = OcioPreview::GetDisplayNames(file_config);
	String default_source = OcioPreview::GetDefaultSourceColorSpace(file_config);
	String default_display = OcioPreview::GetDefaultDisplay(file_config);
	String chosen_view_name;
	String chosen_look_name;
	int chosen_view_index = -1;
	int chosen_look_index = -1;
	int chosen_source_index = FirstDifferentIndex(source_names, default_source);
	int chosen_display_index = FirstDifferentIndex(display_names, default_display);
	if(chosen_source_index < 0 && !source_names.IsEmpty())
		chosen_source_index = 0;
	if(chosen_display_index < 0 && !display_names.IsEmpty())
		chosen_display_index = 0;
	if(chosen_display_index >= 0 && chosen_display_index < display_names.GetCount()) {
		Vector<String> view_names = OcioPreview::GetViewNames(file_config, display_names[chosen_display_index]);
		String default_view = OcioPreview::GetDefaultView(file_config, display_names[chosen_display_index]);
		chosen_view_index = FirstDifferentIndex(view_names, default_view);
		if(chosen_view_index < 0 && !view_names.IsEmpty())
			chosen_view_index = 0;
		if(chosen_view_index >= 0 && chosen_view_index < view_names.GetCount())
			chosen_view_name = view_names[chosen_view_index];
	}
	Vector<String> look_names = OcioPreview::GetLookNames(file_config);
	if(look_names.GetCount() > 1) {
		chosen_look_index = 1;
		chosen_look_name = look_names[1];
	}
	else if(!look_names.IsEmpty()) {
		chosen_look_index = 0;
		chosen_look_name = look_names[0];
	}

	wb.ocio_enable_drop.Select(1);
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Enable);
	wb.ocio_config_source_drop.Select((int)OcioConfigSource::File);
	wb.ocio_config_file_requested_path = file_config_path.string().c_str();
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::ConfigSource);
	if(chosen_source_index >= 0 && chosen_source_index < wb.ocio_source_drop.GetCount()) {
		wb.ocio_source_drop.Select(chosen_source_index);
		wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Source);
	}
	if(chosen_display_index >= 0 && chosen_display_index < wb.ocio_display_drop.GetCount()) {
		wb.ocio_display_drop.Select(chosen_display_index);
		wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Display);
	}
	if(chosen_view_index >= 0 && chosen_view_index < wb.ocio_view_drop.GetCount()) {
		wb.ocio_view_drop.Select(chosen_view_index);
		wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::View);
	}
	if(chosen_look_index >= 0 && chosen_look_index < wb.ocio_look_drop.GetCount()) {
		wb.ocio_look_drop.Select(chosen_look_index);
		wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Look);
	}
	wb.ocio_lut_path = (root / "workbench_curve.cube").string().c_str();
	wb.ocio_lut_requested_path.Clear();
	wb.UpdateOcioControls(ImagingWorkbench::OcioControlChange::Lut);
	wb.ApplyExposureStops(3.0, true);
	wb.ApplyDisplayGamma(2.2, true);
	wb.ApplyChannelView(ChannelView::Blue);
	wb.RenderPreviewFromProxy();
	Check(wb.ocio_enabled, "viewer-state OCIO enabled", passed, failed);
	Check(wb.ocio_processor_valid, "viewer-state processor valid", passed, failed);
	Check(wb.ocio_preview_applied, "viewer-state preview applied", passed, failed);
	Check(!wb.ocio_look_name.IsEmpty(), "viewer-state look selected", passed, failed);
	Check(!wb.ocio_lut_path.IsEmpty(), "viewer-state LUT active", passed, failed);
	Check(wb.exposure_stops != 0.0, "viewer-state exposure non-zero", passed, failed);
	Check(wb.display_gamma != 1.0, "viewer-state gamma non-default", passed, failed);
	Check(wb.channel_view != ChannelView::RGB, "viewer-state channel view non-RGB", passed, failed);
	if(source_names.GetCount() > 1)
		Check(wb.ocio_source_name != default_source, "viewer-state source non-default", passed, failed);
	if(display_names.GetCount() > 1)
		Check(wb.ocio_display_name != default_display, "viewer-state display non-default", passed, failed);
	if(!chosen_view_name.IsEmpty() && wb.ocio_view_drop.GetCount() > 1)
		Check(wb.ocio_view_name == chosen_view_name, "viewer-state view selected", passed, failed);

	WorkbenchSnapshot viewer_state;
	Check(CaptureWorkbenchSnapshot(wb, viewer_state), "capture viewer-state save state", passed, failed);

	std::filesystem::path viewer_exr = root / "viewer_state.exr";
	String viewer_exr_path = viewer_exr.string().c_str();
	Check(wb.SaveCurrentImage(viewer_exr_path, "EXR", save_error), "viewer-state EXR save", passed, failed);
	WorkbenchSnapshot viewer_after_exr;
	Check(CaptureWorkbenchSnapshot(wb, viewer_after_exr), "capture viewer EXR post-save state", passed, failed);
	Check(SameWorkbenchSnapshot(viewer_state, viewer_after_exr), "viewer EXR save leaves source and viewer state unchanged", passed, failed);

	std::filesystem::path viewer_png = root / "viewer_state.png";
	String viewer_png_path = viewer_png.string().c_str();
	Check(wb.SaveCurrentImage(viewer_png_path, "PNG", save_error), "viewer-state PNG save", passed, failed);
	WorkbenchSnapshot viewer_after_png;
	Check(CaptureWorkbenchSnapshot(wb, viewer_after_png), "capture viewer PNG post-save state", passed, failed);
	Check(SameWorkbenchSnapshot(viewer_state, viewer_after_png), "viewer PNG save leaves source and viewer state unchanged", passed, failed);

	OIIO::ImageBuf viewer_exr_image;
	Check(UppImaging::LoadImage(viewer_exr_path.Begin(), viewer_exr_image, &load_error), "viewer EXR reopen", passed, failed);
	OIIO::ImageBuf viewer_png_image;
	Check(UppImaging::LoadImage(viewer_png_path.Begin(), viewer_png_image, &load_error), "viewer PNG reopen", passed, failed);
	ImageSnapshot viewer_exr_snapshot;
	ImageSnapshot viewer_png_snapshot;
	Check(CaptureImageSnapshot(viewer_exr_image, viewer_exr_snapshot), "capture viewer EXR snapshot", passed, failed);
	Check(CaptureImageSnapshot(viewer_png_image, viewer_png_snapshot), "capture viewer PNG snapshot", passed, failed);

	Check(SameImageSpec(baseline_png_snapshot, viewer_png_snapshot, true), "PNG outputs have matching specs", passed, failed);
	Check(SameImagePixels(baseline_png_snapshot, viewer_png_snapshot, 0.0f), "PNG output pixels ignore viewer controls", passed, failed);
	Check(SameImageSpec(baseline_exr_snapshot, viewer_exr_snapshot, true), "EXR outputs have matching specs", passed, failed);
	Check(SameImagePixels(baseline_exr_snapshot, viewer_exr_snapshot, 1e-6f), "EXR output pixels ignore viewer controls", passed, failed);
	ImageSnapshot source_snapshot;
	Check(CaptureImageSnapshot(wb.source_image, source_snapshot), "capture immutable source snapshot", passed, failed);
	Check(SameImageSpec(source_snapshot, baseline_exr_snapshot, true), "EXR reopened source matches immutable source spec", passed, failed);
	Check(SameImagePixels(source_snapshot, baseline_exr_snapshot, 1e-6f), "EXR reopened source matches immutable source pixels", passed, failed);
	Check(SameImageSpec(source_snapshot, viewer_exr_snapshot, true), "viewer EXR matches immutable source spec", passed, failed);
	Check(SameImagePixels(source_snapshot, viewer_exr_snapshot, 1e-6f), "viewer EXR matches immutable source pixels", passed, failed);

	std::filesystem::path fail_exr_dir = root / "failed_save.exr";
	std::filesystem::create_directory(fail_exr_dir);
	WorkbenchSnapshot before_failed_exr;
	Check(CaptureWorkbenchSnapshot(wb, before_failed_exr), "capture failed EXR save state", passed, failed);
	String failed_exr_path = fail_exr_dir.string().c_str();
	Check(!wb.SaveCurrentImage(failed_exr_path, "EXR", save_error), "failed EXR save", passed, failed);
	WorkbenchSnapshot after_failed_exr;
	Check(CaptureWorkbenchSnapshot(wb, after_failed_exr), "capture failed EXR post-save state", passed, failed);
	Check(SameWorkbenchSnapshot(before_failed_exr, after_failed_exr), "failed EXR save leaves source and viewer state unchanged", passed, failed);

	std::filesystem::path fail_png_dir = root / "failed_save.png";
	std::filesystem::create_directory(fail_png_dir);
	WorkbenchSnapshot before_failed_png;
	Check(CaptureWorkbenchSnapshot(wb, before_failed_png), "capture failed PNG save state", passed, failed);
	String failed_png_path = fail_png_dir.string().c_str();
	Check(!wb.SaveCurrentImage(failed_png_path, "PNG", save_error), "failed PNG save", passed, failed);
	WorkbenchSnapshot after_failed_png;
	Check(CaptureWorkbenchSnapshot(wb, after_failed_png), "capture failed PNG post-save state", passed, failed);
	Check(SameWorkbenchSnapshot(before_failed_png, after_failed_png), "failed PNG save leaves source and viewer state unchanged", passed, failed);
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
