#include "OcioPreview.h"

#include <exception>

namespace Upp {
namespace OcioPreview {

static void CopyImage(const TestImageF& src, TestImageF& dst)
{
	dst.width = src.width;
	dst.height = src.height;
	dst.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		dst.pixels[i] = src.pixels[i];
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
	error.Clear();
	CopyImage(src, dst);
	if(!config) {
		error = "OCIO config is not loaded";
		return false;
	}
	if(!src.pixels.GetCount() || src.width <= 0 || src.height <= 0 || src.pixels.GetCount() != (int64)src.width * src.height) {
		error = "OCIO preview source image is invalid";
		return false;
	}
	if(source_color_space.IsEmpty() || display.IsEmpty() || view.IsEmpty()) {
		error = "OCIO preview selection is incomplete";
		return false;
	}
	try {
		OCIO::ConstProcessorRcPtr processor = config->getProcessor(~source_color_space, ~display, ~view, OCIO::TRANSFORM_DIR_FORWARD);
		if(!processor) {
			error = "OCIO processor creation failed";
			return false;
		}
		OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
		if(!cpu) {
			error = "OCIO CPU processor creation failed";
			return false;
		}
		OCIO::PackedImageDesc desc(&dst.pixels[0].r, dst.width, dst.height, 4);
		cpu->apply(desc);
		for(int i = 0; i < dst.pixels.GetCount(); ++i)
			dst.pixels[i].a = src.pixels[i].a;
		return true;
	}
	catch(const OCIO::Exception& e) {
		error = e.what();
	}
	catch(const std::exception& e) {
		error = e.what();
	}
	catch(...) {
		error = "OCIO preview transform failed";
	}
	CopyImage(src, dst);
	return false;
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
