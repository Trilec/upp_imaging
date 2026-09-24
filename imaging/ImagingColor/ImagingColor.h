#ifndef _ImagingColor_ImagingColor_h_
#define _ImagingColor_ImagingColor_h_

#include <ImagingCore/ImagingCore.h>

namespace Upp {
namespace Imaging {

enum class ColorConfigSource {
	Builtin,
	File,
};

struct ColorConfigRef {
	ColorConfigSource source = ColorConfigSource::Builtin;
	String value;
};

struct ColorConfigInfo {
	String identity;
	String cache_id;
	Vector<String> color_spaces;
	Vector<String> displays;
	Vector<String> looks;
	String default_source;
	String default_display;
	String default_view;

	ColorConfigInfo() = default;
	ColorConfigInfo(const ColorConfigInfo& other);
	ColorConfigInfo(ColorConfigInfo&& other);
	ColorConfigInfo& operator=(const ColorConfigInfo& other);
	ColorConfigInfo& operator=(ColorConfigInfo&& other);

	void Clear();
};

struct ColorSpaceTransform {
	ColorConfigRef config;
	String source;
	String destination;
};

struct DisplayTransform {
	ColorConfigRef config;
	String source;
	String display;
	String view;
};

Vector<String> GetBuiltinColorConfigNames();

Result InspectColorConfig(const ColorConfigRef& config,
                          ColorConfigInfo& info,
                          Diagnostics* diagnostics = nullptr);

Result GetDisplayViews(const ColorConfigRef& config,
                       const String& display,
                       Vector<String>& views,
                       String* default_view = nullptr,
                       Diagnostics* diagnostics = nullptr);

Result ApplyColorSpaceTransform(const ImageData& input,
                                ImageData& output,
                                const ColorSpaceTransform& transform,
                                Diagnostics* diagnostics = nullptr);

Result ApplyDisplayTransform(const ImageData& input,
                             ImageData& output,
                             const DisplayTransform& transform,
                             Diagnostics* diagnostics = nullptr);

} // namespace Imaging
} // namespace Upp

#endif
