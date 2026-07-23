#include "OcioPreview.h"

#include <algorithm>
#include <exception>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <vector>

namespace Upp {
namespace OcioPreview {

static int ClampByte(float value)
{
	if(!std::isfinite(value))
		value = 0.0f;
	value = std::clamp(value, 0.0f, 1.0f);
	return (int)(value * 255.0f + 0.5f);
}

static void BuildGammaLut(Vector<float>& lut, double display_gamma)
{
	static constexpr int LUT_SIZE = 65536;
	lut.SetCount(LUT_SIZE);
	float inverse_gamma = 1.0f / (float)((!std::isfinite(display_gamma) || display_gamma <= 0.0) ? 1.0 : display_gamma);
	for(int i = 0; i < LUT_SIZE; ++i) {
		float normalized = (float)i / (float)(LUT_SIZE - 1);
		lut[i] = std::pow(normalized, inverse_gamma);
	}
}

static String CanonicalPath(const String& path)
{
	if(path.IsEmpty())
		return String();
	std::error_code ec;
	std::filesystem::path fs = std::filesystem::weakly_canonical(std::filesystem::path(~path), ec);
	if(ec)
		fs = std::filesystem::absolute(std::filesystem::path(~path), ec);
	return String(fs.string().c_str());
}

void OcioPreviewProcessor::Clear()
{
	config.reset();
	cpu.reset();
	config_name.Clear();
	config_cache_id.Clear();
	source.Clear();
	display.Clear();
	view.Clear();
	look.Clear();
	lut.Clear();
	lut_direction = OCIO::TRANSFORM_DIR_FORWARD;
}

bool OcioPreviewProcessor::Update(const OCIO::ConstConfigRcPtr& new_config, const String& new_config_name,
	const String& new_source, const String& new_display, const String& new_view,
	const String& new_look, const String& new_lut, OCIO::TransformDirection new_lut_direction,
	String& error)
{
	error.Clear();
	String new_cache_id = new_config ? String(new_config->getCacheID()) : String();
	if(config && config.get() == new_config.get() && config_name == new_config_name && config_cache_id == new_cache_id &&
		source == new_source && display == new_display && view == new_view && look == new_look &&
		lut == new_lut && lut_direction == new_lut_direction)
		return true;

	if(!new_config) {
		error = "OCIO config is not loaded";
		return false;
	}
	if(new_source.IsEmpty() || new_display.IsEmpty() || new_view.IsEmpty()) {
		error = "OCIO preview selection is incomplete";
		return false;
	}

	try {
		OCIO::GroupTransformRcPtr group = OCIO::GroupTransform::Create();
		if(!new_look.IsEmpty() && new_look != "None") {
			OCIO::LookTransformRcPtr look_transform = OCIO::LookTransform::Create();
			look_transform->setSrc(~new_source);
			look_transform->setDst(~new_source);
			look_transform->setLooks(~new_look);
			group->appendTransform(look_transform);
		}
		OCIO::DisplayViewTransformRcPtr display_transform = OCIO::DisplayViewTransform::Create();
		display_transform->setSrc(~new_source);
		display_transform->setDisplay(~new_display);
		display_transform->setView(~new_view);
		display_transform->setLooksBypass(true);
		group->appendTransform(display_transform);
		if(!new_lut.IsEmpty()) {
			OCIO::FileTransformRcPtr lut_transform = OCIO::FileTransform::Create();
			lut_transform->setSrc(~new_lut);
			lut_transform->setDirection(new_lut_direction);
			group->appendTransform(lut_transform);
		}
		OCIO::ConstProcessorRcPtr processor = new_config->getProcessor(group);
		if(!processor) {
			error = "OCIO processor creation failed";
			return false;
		}
		OCIO::ConstCPUProcessorRcPtr new_cpu = processor->getDefaultCPUProcessor();
		if(!new_cpu) {
			error = "OCIO CPU processor creation failed";
			return false;
		}
		config = new_config;
		cpu = new_cpu;
		config_name = new_config_name;
		config_cache_id = new_cache_id;
		source = new_source;
		display = new_display;
		view = new_view;
		look = new_look;
		lut = new_lut;
		lut_direction = new_lut_direction;
		++build_count;
		return true;
	}
	catch(const OCIO::Exception& e) {
		error = e.what();
	}
	catch(const std::exception& e) {
		error = e.what();
	}
	catch(...) {
		error = "OCIO processor creation failed";
	}
	return false;
}

bool OcioPreviewProcessor::Update(const OCIO::ConstConfigRcPtr& new_config, const String& new_config_name,
	const String& new_source, const String& new_display, const String& new_view,
	const String& new_look, const String& new_lut, String& error)
{
	return Update(new_config, new_config_name, new_source, new_display, new_view, new_look, new_lut,
		OCIO::TRANSFORM_DIR_FORWARD, error);
}

float ApplySceneExposure(float value, float exposure_scale)
{
	if(std::isnan(value) || value == 0.0f)
		return 0.0f;
	if(!std::isfinite(exposure_scale))
		exposure_scale = 1.0f;
	if(!std::isfinite(value))
		return value;
	return value * exposure_scale;
}

void UpdateDisplayGamma(DisplayGammaState& gamma, double display_gamma)
{
	if(!std::isfinite(display_gamma) || display_gamma <= 0.0)
		display_gamma = 1.0;
	if(!gamma.lut.IsEmpty() && gamma.gamma == display_gamma)
		return;
	gamma.gamma = display_gamma;
	gamma.inverse_gamma = 1.0f / (float)display_gamma;
	BuildGammaLut(gamma.lut, display_gamma);
}

byte ApplyDisplayGammaToByte(float value, const DisplayGammaState& gamma)
{
	if(gamma.lut.IsEmpty())
		return 0;
	if(std::isnan(value) || value <= 0.0f)
		return 0;
	if(value > 0.0f && !std::isfinite(value))
		return 255;
	double scaled = (double)value;
	if(scaled <= 0.0)
		return 0;
	if(scaled >= 1.0)
		return 255;
	double pos = scaled * (double)(gamma.lut.GetCount() - 1);
	int idx0 = std::clamp((int)std::floor(pos), 0, gamma.lut.GetCount() - 1);
	int idx1 = std::min(idx0 + 1, gamma.lut.GetCount() - 1);
	double frac = pos - idx0;
	double blended = (1.0 - frac) * (double)gamma.lut[idx0] + frac * (double)gamma.lut[idx1];
	return (byte)ClampByte((float)blended);
}

bool ApplyOcioProcessor(const OCIO::ConstCPUProcessorRcPtr& cpu, float* pixels, int width, int height, int channels, String& error)
{
	error.Clear();
	if(!cpu) {
		error = "OCIO CPU processor is not available";
		return false;
	}
	if(!pixels || width <= 0 || height <= 0 || (channels != 3 && channels != 4)) {
		error = "OCIO packed buffer is invalid";
		return false;
	}
	try {
		Vector<float> alpha;
		if(channels == 4) {
			alpha.SetCount((int64)width * height);
			for(int i = 0; i < alpha.GetCount(); ++i)
				alpha[i] = pixels[i * 4 + 3];
		}
		OCIO::PackedImageDesc desc(pixels, width, height, channels);
		cpu->apply(desc);
		if(channels == 4) {
			for(int i = 0; i < alpha.GetCount(); ++i)
				pixels[i * 4 + 3] = alpha[i];
		}
		return true;
	}
	catch(const OCIO::Exception& e) {
		error = e.what();
	}
	catch(const std::exception& e) {
		error = e.what();
	}
	catch(...) {
		error = "OCIO packed buffer processing failed";
	}
	return false;
}

Vector<String> GetBuiltinConfigNames()
{
	Vector<String> names;
	names.Add("cg-config-v4.0.0_aces-v2.0_ocio-v2.5");
	names.Add("studio-config-v4.0.0_aces-v2.0_ocio-v2.5");
	return names;
}

bool LoadBuiltinConfig(const String& name, OCIO::ConstConfigRcPtr& config, String& error)
{
	config.reset();
	error.Clear();
	if(name.IsEmpty()) {
		error = "OCIO builtin config name is empty";
		return false;
	}
	try {
		config = OCIO::Config::CreateFromBuiltinConfig(~name);
		if(!config) {
			error = "OCIO builtin config load returned null";
			return false;
		}
		config->validate();
		return true;
	}
	catch(const OCIO::Exception& e) {
		error = e.what();
		return false;
	}
	catch(const std::exception& e) {
		error = e.what();
		return false;
	}
	catch(...) {
		error = "OCIO builtin config load failed";
		return false;
	}
}

bool LoadEnvironmentConfig(OCIO::ConstConfigRcPtr& config, String& error, String& identity)
{
	config.reset();
	error.Clear();
	identity.Clear();
	const char* env = std::getenv("OCIO");
	if(!env || !*env) {
		error = "OCIO environment variable is absent";
		return false;
	}
	identity = env;
	try {
		config = OCIO::Config::CreateFromEnv();
		if(!config) {
			error = "OCIO environment config could not be loaded";
			return false;
		}
		config->validate();
		return true;
	}
	catch(const OCIO::Exception& e) {
		error = e.what();
		return false;
	}
	catch(const std::exception& e) {
		error = e.what();
		return false;
	}
	catch(...) {
		error = "OCIO environment config could not be loaded";
		return false;
	}
}

bool LoadConfigFile(const String& path, OCIO::ConstConfigRcPtr& config, String& error, String& identity)
{
	config.reset();
	error.Clear();
	identity = CanonicalPath(path);
	if(path.IsEmpty()) {
		error = "OCIO config file path is empty";
		return false;
	}
	try {
		config = OCIO::Config::CreateFromFile(~path);
		if(!config) {
			error = "OCIO config file could not be loaded";
			return false;
		}
		config->validate();
		return true;
	}
	catch(const OCIO::Exception& e) {
		error = e.what();
		return false;
	}
	catch(const std::exception& e) {
		error = e.what();
		return false;
	}
	catch(...) {
		error = "OCIO config file could not be loaded";
		return false;
	}
}

static bool ContainsName(const Vector<String>& values, const String& value)
{
	for(const String& item : values) {
		if(item == value)
			return true;
	}
	return false;
}

Vector<String> GetColorSpaceNames(const OCIO::ConstConfigRcPtr& config)
{
	Vector<String> names;
	if(!config)
		return names;
	for(int i = 0; i < config->getNumColorSpaces(); ++i) {
		const char* name = config->getColorSpaceNameByIndex(i);
		if(name && *name)
			names.Add(name);
	}
	return names;
}

Vector<String> GetLookNames(const OCIO::ConstConfigRcPtr& config)
{
	Vector<String> names;
	if(!config)
		return names;
	for(int i = 0; i < config->getNumLooks(); ++i) {
		const char* name = config->getLookNameByIndex(i);
		if(name && *name)
			names.Add(name);
	}
	return names;
}

String GetDefaultSourceColorSpace(const OCIO::ConstConfigRcPtr& config)
{
	if(!config)
		return String();
	const char* scene_linear = config->getRoleColorSpace("scene_linear");
	if(scene_linear && *scene_linear)
		return scene_linear;
	const char* default_role = config->getRoleColorSpace("default");
	if(default_role && *default_role)
		return default_role;
	Vector<String> names = GetColorSpaceNames(config);
	return names.IsEmpty() ? String() : names[0];
}

Vector<String> GetDisplayNames(const OCIO::ConstConfigRcPtr& config)
{
	Vector<String> names;
	if(!config)
		return names;
	for(int i = 0; i < config->getNumDisplays(); ++i) {
		const char* name = config->getDisplay(i);
		if(name && *name)
			names.Add(name);
	}
	return names;
}

String GetDefaultDisplay(const OCIO::ConstConfigRcPtr& config)
{
	if(!config)
		return String();
	const char* display = config->getDefaultDisplay();
	if(display && *display)
		return display;
	Vector<String> names = GetDisplayNames(config);
	return names.IsEmpty() ? String() : names[0];
}

Vector<String> GetViewNames(const OCIO::ConstConfigRcPtr& config, const String& display)
{
	Vector<String> names;
	if(!config || display.IsEmpty())
		return names;
	for(int i = 0; i < config->getNumViews(~display); ++i) {
		const char* name = config->getView(~display, i);
		if(name && *name)
			names.Add(name);
	}
	return names;
}

String GetDefaultView(const OCIO::ConstConfigRcPtr& config, const String& display)
{
	if(!config || display.IsEmpty())
		return String();
	const char* view = config->getDefaultView(~display);
	if(view && *view)
		return view;
	Vector<String> names = GetViewNames(config, display);
	return names.IsEmpty() ? String() : names[0];
}

bool ApplyPreview(const OCIO::ConstConfigRcPtr& config, const String& source_color_space,
	const String& display, const String& view, const TestImageF& src, TestImageF& dst, String& error)
{
	if(!src.pixels.GetCount() || src.width <= 0 || src.height <= 0 || src.pixels.GetCount() != (int64)src.width * src.height) {
		error = "OCIO preview source image is invalid";
		return false;
	}
	dst.width = src.width;
	dst.height = src.height;
	dst.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		dst.pixels[i] = src.pixels[i];
	OcioPreviewProcessor processor;
	if(!processor.Update(config, String(), source_color_space, display, view, String(), String(), error))
		return false;
	std::vector<float> pixels((size_t)src.width * src.height * 4);
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		pixels[(size_t)i * 4 + 0] = src.pixels[i].r;
		pixels[(size_t)i * 4 + 1] = src.pixels[i].g;
		pixels[(size_t)i * 4 + 2] = src.pixels[i].b;
		pixels[(size_t)i * 4 + 3] = src.pixels[i].a;
	}
	if(!ApplyOcioProcessor(processor.cpu, pixels.data(), src.width, src.height, 4, error))
		return false;
	dst.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		dst.pixels[i].r = pixels[(size_t)i * 4 + 0];
		dst.pixels[i].g = pixels[(size_t)i * 4 + 1];
		dst.pixels[i].b = pixels[(size_t)i * 4 + 2];
		dst.pixels[i].a = pixels[(size_t)i * 4 + 3];
	}
	return true;
}

String DescribeSelection(const String& config_name, const String& source_color_space,
	const String& display, const String& view)
{
	if(config_name.IsEmpty())
		return "OCIO: off";
	return Format("OCIO: %s / %s / %s / %s", config_name, source_color_space, display, view);
}

} // namespace OcioPreview
} // namespace Upp
