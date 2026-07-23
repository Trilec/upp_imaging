#include <stdio.h>

#include <imaging_roundtrip_viewer_ocio/OcioPreview.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

using namespace Upp;

namespace OCIO = OCIO_NAMESPACE;

static bool SameImage(const TestImageF& a, const TestImageF& b)
{
	if(a.width != b.width || a.height != b.height || a.pixels.GetCount() != b.pixels.GetCount())
		return false;
	for(int i = 0; i < a.pixels.GetCount(); ++i) {
		if(a.pixels[i].r != b.pixels[i].r || a.pixels[i].g != b.pixels[i].g || a.pixels[i].b != b.pixels[i].b || a.pixels[i].a != b.pixels[i].a)
			return false;
	}
	return true;
}

static void CopyTestImage(const TestImageF& src, TestImageF& dst)
{
	dst.width = src.width;
	dst.height = src.height;
	dst.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		dst.pixels[i] = src.pixels[i];
}

static bool HasRgbDifference(const TestImageF& a, const TestImageF& b)
{
	for(int i = 0; i < a.pixels.GetCount(); ++i) {
		if(a.pixels[i].r != b.pixels[i].r || a.pixels[i].g != b.pixels[i].g || a.pixels[i].b != b.pixels[i].b)
			return true;
	}
	return false;
}

static bool TestProcessorCache(const OCIO::ConstConfigRcPtr& config, const String& builtin)
{
	Vector<String> sources = OcioPreview::GetColorSpaceNames(config);
	Vector<String> displays = OcioPreview::GetDisplayNames(config);
	if(sources.GetCount() < 2 || displays.IsEmpty())
		return false;
	String source0 = OcioPreview::GetDefaultSourceColorSpace(config);
	if(source0.IsEmpty())
		source0 = sources[0];
	String source1 = sources[1];
	String display0 = displays[0];
	Vector<String> views0 = OcioPreview::GetViewNames(config, display0);
	if(views0.GetCount() < 2)
		return false;
	String view0 = OcioPreview::GetDefaultView(config, display0);
	if(view0.IsEmpty())
		view0 = views0[0];
	String view1 = views0[1];

	OcioPreview::OcioPreviewProcessor processor;
	String error;
	if(!processor.Update(config, builtin, source0, display0, view0, String(), String(), error) || !processor.IsValid())
		return false;
	int build_count = processor.build_count;
	if(!processor.Update(config, builtin, source0, display0, view0, String(), String(), error))
		return false;
	if(processor.build_count != build_count)
		return false;
	if(!processor.Update(config, builtin, source1, display0, view0, String(), String(), error))
		return false;
	if(processor.build_count != build_count + 1)
		return false;
	build_count = processor.build_count;
	if(!processor.Update(config, builtin, source1, displays[0], view0, String(), String(), error))
		return false;
	if(processor.build_count != build_count)
		return false;
	if(displays.GetCount() > 1) {
		if(!processor.Update(config, builtin, source1, displays[1], OcioPreview::GetDefaultView(config, displays[1]), String(), String(), error))
			return false;
		if(processor.build_count != build_count + 1)
			return false;
		build_count = processor.build_count;
	}
	if(!processor.Update(config, builtin, source1, display0, view1, String(), String(), error))
		return false;
	if(processor.build_count != build_count + 1)
		return false;

	std::vector<float> pixels = { 0.15f, 0.35f, 0.55f, 0.25f };
	if(!OcioPreview::ApplyOcioProcessor(processor.cpu, pixels.data(), 1, 1, 4, error))
		return false;
	if(pixels[3] != 0.25f)
		return false;
	if(OcioPreview::ApplyOcioProcessor(OCIO::ConstCPUProcessorRcPtr(), pixels.data(), 1, 1, 4, error))
		return false;
	if(error.IsEmpty())
		return false;
	if(OcioPreview::ApplyOcioProcessor(processor.cpu, pixels.data(), 1, 1, 2, error))
		return false;
	if(error.IsEmpty())
		return false;
	printf("OCIO processor cache %s: OK\n", ~builtin);
	return true;
}

static bool TestPreviewRecovery(const OCIO::ConstConfigRcPtr& config, const String& builtin)
{
	String source = OcioPreview::GetDefaultSourceColorSpace(config);
	Vector<String> displays = OcioPreview::GetDisplayNames(config);
	if(source.IsEmpty() || displays.IsEmpty())
		return false;
	String display = displays[0];
	Vector<String> views = OcioPreview::GetViewNames(config, display);
	if(views.IsEmpty())
		return false;
	String view = views[0];

	TestImageF src;
	src.width = 2;
	src.height = 1;
	src.pixels.SetCount(2);
	src.pixels[0] = {0.2f, 0.4f, 0.6f, 0.7f};
	src.pixels[1] = {0.3f, 0.1f, 0.5f, 0.9f};
	TestImageF original;
	CopyTestImage(src, original);

	TestImageF invalid_preview;
	String preview_error;
	if(OcioPreview::ApplyPreview(config, source, "invalid-display", "invalid-view", src, invalid_preview, preview_error))
		return false;
	if(preview_error.IsEmpty())
		return false;
	if(!SameImage(src, original))
		return false;

	TestImageF valid_preview;
	preview_error.Clear();
	if(!OcioPreview::ApplyPreview(config, source, display, view, src, valid_preview, preview_error))
		return false;
	if(valid_preview.pixels.GetCount() == 0 || valid_preview.pixels.GetCount() != src.pixels.GetCount())
		return false;
	if(!HasRgbDifference(src, valid_preview))
		return false;
	if(!SameImage(src, original))
		return false;

	printf("OCIO preview recovery %s: OK (%s)\n", ~builtin, ~OcioPreview::DescribeSelection(builtin, source, display, view));
	return true;
}

static std::filesystem::path TempRoot()
{
	return std::filesystem::temp_directory_path() / "upp_imaging_ocio_smoke_test";
}

static std::filesystem::path RepoConfigPath(const char* file)
{
	return std::filesystem::path("opencolorio_src/upstream/src/OpenColorIO/builtinconfigs/configs") / file;
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

static bool SameRgb(const std::vector<float>& a, const std::vector<float>& b)
{
	return a.size() == b.size() && a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}

static bool Check(bool condition, const char* label, int& passed, int& failed)
{
	std::printf("%s %s\n", condition ? "PASS" : "FAIL", label);
	condition ? ++passed : ++failed;
	return condition;
}

int main()
{
	int passed = 0;
	int failed = 0;
	std::filesystem::path root = TempRoot();
	std::filesystem::remove_all(root);
	std::filesystem::create_directories(root);

	Vector<String> builtins = OcioPreview::GetBuiltinConfigNames();
	if(builtins.GetCount() == 2) {
		printf("OCIO builtin list: OK\n");
		passed++;
	} else {
		printf("OCIO builtin list: FAIL\n");
		failed++;
	}

	TestImageF src;
	src.width = 2;
	src.height = 1;
	src.pixels.SetCount(2);
	src.pixels[0] = {0.2f, 0.4f, 0.6f, 0.7f};
	src.pixels[1] = {0.3f, 0.1f, 0.5f, 0.9f};
	TestImageF original;
	CopyTestImage(src, original);

	bool preview_ok = false;
	String selected_note;
	bool cache_ok = false;
	for(const String& builtin : builtins) {
		OCIO::ConstConfigRcPtr config;
		String error;
		if(!OcioPreview::LoadBuiltinConfig(builtin, config, error)) {
			printf("OCIO builtin config %s: FAIL (%s)\n", ~builtin, ~error);
			failed++;
			continue;
		}
		printf("OCIO builtin config %s: OK\n", ~builtin);
		passed++;
		if(!cache_ok) {
			cache_ok = TestProcessorCache(config, builtin);
			if(cache_ok)
				passed++;
		}

		String source = OcioPreview::GetDefaultSourceColorSpace(config);
		Vector<String> displays = OcioPreview::GetDisplayNames(config);
		for(const String& display : displays) {
			Vector<String> views = OcioPreview::GetViewNames(config, display);
			for(const String& view : views) {
				TestImageF preview;
				String preview_error;
				if(!OcioPreview::ApplyPreview(config, source, display, view, src, preview, preview_error))
					continue;
				if(!SameImage(src, original))
					continue;
				if(!HasRgbDifference(src, preview))
					continue;
				if(preview.pixels[0].a != src.pixels[0].a || preview.pixels[1].a != src.pixels[1].a)
					continue;
				selected_note = OcioPreview::DescribeSelection(builtin, source, display, view);
				printf("OCIO preview transform: OK (%s)\n", ~selected_note);
				printf("OCIO preview alpha preserved: OK\n");
				preview_ok = true;
				break;
			}
			if(preview_ok)
				break;
		}
		if(preview_ok)
			break;
	}

	bool recovery_ok = false;
	for(const String& builtin : builtins) {
		OCIO::ConstConfigRcPtr config;
		String error;
		if(!OcioPreview::LoadBuiltinConfig(builtin, config, error))
			continue;
		recovery_ok = TestPreviewRecovery(config, builtin);
		if(recovery_ok)
			break;
	}
	if(recovery_ok)
		passed++;
	else {
		printf("OCIO preview recovery: FAIL\n");
		failed++;
	}

	if(preview_ok)
		passed += 2;
	else {
		printf("OCIO preview transform: FAIL\n");
		printf("OCIO preview alpha preserved: FAIL\n");
		failed += 2;
	}

	if(!cache_ok) {
		printf("OCIO processor cache: FAIL\n");
		failed++;
	}

	if(SameImage(src, original)) {
		printf("OCIO raw source unchanged: OK\n");
		passed++;
	} else {
		printf("OCIO raw source unchanged: FAIL\n");
		failed++;
	}

	const std::filesystem::path file_config_path = RepoConfigPath("studio-config-v4.0.0_aces-v2.0_ocio-v2.5.ocio");
	OCIO::ConstConfigRcPtr file_config;
	String file_error;
	String file_identity;
	String file_source;
	String file_display;
	String file_look;
	Vector<String> file_views;
	Check(OcioPreview::LoadConfigFile(file_config_path.string().c_str(), file_config, file_error, file_identity), "file config load", passed, failed);
	if(file_config) {
		Vector<String> file_looks = OcioPreview::GetLookNames(file_config);
		Check(!file_looks.IsEmpty(), "file config looks", passed, failed);
		file_source = OcioPreview::GetDefaultSourceColorSpace(file_config);
		Vector<String> file_displays = OcioPreview::GetDisplayNames(file_config);
		if(!file_source.IsEmpty() && !file_displays.IsEmpty()) {
			file_display = file_displays[0];
			file_views = OcioPreview::GetViewNames(file_config, file_display);
			if(!file_views.IsEmpty()) {
				TestImageF file_preview;
				String file_preview_error;
				Check(OcioPreview::ApplyPreview(file_config, file_source, file_display, file_views[0], src, file_preview, file_preview_error), "file config preview", passed, failed);
				if(file_preview.pixels.GetCount() == src.pixels.GetCount()) {
					bool file_diff = HasRgbDifference(src, file_preview);
					Check(file_diff, "file config transform differs", passed, failed);
					Check(file_preview.pixels[0].a == src.pixels[0].a && file_preview.pixels[1].a == src.pixels[1].a, "file config alpha preserved", passed, failed);
				}
			}

			file_look = file_looks.IsEmpty() ? String() : file_looks[0];
			if(!file_look.IsEmpty() && !file_views.IsEmpty()) {
				std::filesystem::path lut_path = root / "curve_test.cube";
				Check(WriteCurveLut(lut_path), "cube lut fixture", passed, failed);
				OcioPreview::OcioPreviewProcessor processor;
				String proc_error;
				Check(processor.Update(file_config, file_identity, file_source, file_display, file_views[0], String(), String(), OCIO::TRANSFORM_DIR_FORWARD, proc_error), "processor baseline build", passed, failed);
				int build_count = processor.build_count;
				Check(processor.Update(file_config, file_identity, file_source, file_display, file_views[0], file_look, String(), OCIO::TRANSFORM_DIR_FORWARD, proc_error), "look build", passed, failed);
				Check(processor.build_count == build_count + 1, "look rebuilds once", passed, failed);
				build_count = processor.build_count;
				std::vector<float> baseline = { 0.35f, 0.25f, 0.15f, 0.6f };
				Check(OcioPreview::ApplyOcioProcessor(processor.cpu, baseline.data(), 1, 1, 4, proc_error), "look apply", passed, failed);
				Check(processor.Update(file_config, file_identity, file_source, file_display, file_views[0], file_look, lut_path.string().c_str(), OCIO::TRANSFORM_DIR_FORWARD, proc_error), "lut build", passed, failed);
				Check(processor.build_count == build_count + 1, "lut rebuilds once", passed, failed);
				build_count = processor.build_count;
				std::vector<float> px = { 0.35f, 0.25f, 0.15f, 0.6f };
				Check(OcioPreview::ApplyOcioProcessor(processor.cpu, px.data(), 1, 1, 4, proc_error), "lut apply", passed, failed);
				Check(px[3] == 0.6f, "lut alpha preserved", passed, failed);
				Check(processor.Update(file_config, file_identity, file_source, file_display, file_views[0], file_look, String(), OCIO::TRANSFORM_DIR_FORWARD, proc_error), "lut clear build", passed, failed);
				Check(processor.build_count == build_count + 1, "lut clear rebuilds once", passed, failed);
				std::vector<float> cleared = { 0.35f, 0.25f, 0.15f, 0.6f };
				Check(OcioPreview::ApplyOcioProcessor(processor.cpu, cleared.data(), 1, 1, 4, proc_error), "lut clear apply", passed, failed);
				Check(cleared[0] == baseline[0] && cleared[1] == baseline[1] && cleared[2] == baseline[2], "lut clear restores non-lut result", passed, failed);
				build_count = processor.build_count;
				Check(processor.Update(file_config, file_identity, file_source, file_display, file_views[0], file_look, lut_path.string().c_str(), OCIO::TRANSFORM_DIR_INVERSE, proc_error), "lut inverse build", passed, failed);
				Check(processor.build_count == build_count + 1, "lut inverse rebuilds once", passed, failed);
				std::vector<float> inverse = { 0.35f, 0.25f, 0.15f, 0.6f };
				Check(OcioPreview::ApplyOcioProcessor(processor.cpu, inverse.data(), 1, 1, 4, proc_error), "lut inverse apply", passed, failed);
				Check(!SameRgb(px, inverse), "lut forward/inverse differ", passed, failed);
			}
		}
	}

	OCIO::ConstConfigRcPtr env_config;
	String env_error;
	String env_identity;
	String previous_ocio = std::getenv("OCIO") ? std::getenv("OCIO") : String();
	std::string env_value = file_config_path.string();
	_putenv_s("OCIO", env_value.c_str());
	Check(OcioPreview::LoadEnvironmentConfig(env_config, env_error, env_identity), "environment config load", passed, failed);
	if(previous_ocio.IsEmpty())
		_putenv_s("OCIO", "");
	else
		_putenv_s("OCIO", ~previous_ocio);
	if(env_config)
		Check(!env_identity.IsEmpty(), "environment identity", passed, failed);

	if(file_config && env_config) {
		String source = OcioPreview::GetDefaultSourceColorSpace(file_config);
		Vector<String> displays = OcioPreview::GetDisplayNames(file_config);
		if(!source.IsEmpty() && !displays.IsEmpty()) {
			Vector<String> views = OcioPreview::GetViewNames(file_config, displays[0]);
			if(!views.IsEmpty()) {
				OcioPreview::OcioPreviewProcessor source_processor;
				String source_error;
				Check(source_processor.Update(file_config, file_identity, source, displays[0], views[0], String(), String(), OCIO::TRANSFORM_DIR_FORWARD, source_error), "file config processor build", passed, failed);
				int source_build = source_processor.build_count;
				Check(source_processor.Update(env_config, env_identity, source, displays[0], views[0], String(), String(), OCIO::TRANSFORM_DIR_FORWARD, source_error), "environment config processor build", passed, failed);
				Check(source_processor.build_count == source_build + 1, "config source change rebuilds once", passed, failed);
			}
		}
	}

	std::filesystem::path bad_config_path = root / "bad.ocio";
	{
		std::ofstream out(bad_config_path, std::ios::binary);
		out << "not a config";
	}
	OCIO::ConstConfigRcPtr bad_config;
	String bad_error;
	String bad_identity;
	Check(!OcioPreview::LoadConfigFile(bad_config_path.string().c_str(), bad_config, bad_error, bad_identity), "invalid config rejection", passed, failed);
	Check(file_config && env_config, "previous valid config survives invalid file", passed, failed);

	if(file_config && !file_look.IsEmpty() && !file_views.IsEmpty()) {
		OcioPreview::OcioPreviewProcessor invalid_lut_processor;
		String lut_error;
		Check(invalid_lut_processor.Update(file_config, file_identity, file_source, file_display, file_views[0], file_look, String(), OCIO::TRANSFORM_DIR_FORWARD, lut_error), "invalid-lut baseline", passed, failed);
		int lut_build = invalid_lut_processor.build_count;
		std::filesystem::path bad_lut_path = root / "bad.cube";
		{
			std::ofstream out(bad_lut_path, std::ios::binary);
			out << "not a lut";
		}
		Check(!invalid_lut_processor.Update(file_config, file_identity, file_source, file_display, file_views[0], file_look, bad_lut_path.string().c_str(), OCIO::TRANSFORM_DIR_FORWARD, lut_error), "invalid lut rejection", passed, failed);
		Check(invalid_lut_processor.build_count == lut_build, "invalid lut preserves prior valid state", passed, failed);
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
