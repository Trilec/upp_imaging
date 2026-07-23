#ifndef UPP_IMAGING_ROUNDTRIP_VIEWER_OCIO_H
#define UPP_IMAGING_ROUNDTRIP_VIEWER_OCIO_H

#include <Core/Core.h>
#include <imaging_roundtrip_test_support/ImagingRoundtripTest.h>
#include <opencolorio/OpenColorIO.h>

#ifndef UPP_IMAGING_LOCAL_OPENCOLORIO_INCLUDE
#error local OpenColorIO package not selected
#endif

namespace Upp {
namespace OcioPreview {

namespace OCIO = OCIO_NAMESPACE;

struct DisplayGammaState {
	double gamma = -1.0;
	float inverse_gamma = 1.0f;
	Vector<float> lut;
};

struct OcioPreviewProcessor {
	OCIO::ConstConfigRcPtr config;
	OCIO::ConstCPUProcessorRcPtr cpu;
	String config_name;
	String source;
	String display;
	String view;
	String look;
	String lut;
	int build_count = 0;

	bool IsValid() const { return (bool)cpu; }
	void Clear();
	bool Update(const OCIO::ConstConfigRcPtr& new_config, const String& new_config_name,
		const String& new_source, const String& new_display, const String& new_view,
		const String& new_look, const String& new_lut, String& error);
};

float ApplySceneExposure(float value, float exposure_scale);
void UpdateDisplayGamma(DisplayGammaState& gamma, double display_gamma);
byte ApplyDisplayGammaToByte(float value, const DisplayGammaState& gamma);
bool ApplyOcioProcessor(const OCIO::ConstCPUProcessorRcPtr& cpu, float* pixels, int width, int height, int channels, String& error);

Vector<String> GetBuiltinConfigNames();
bool LoadBuiltinConfig(const String& name, OCIO::ConstConfigRcPtr& config, String& error);

Vector<String> GetColorSpaceNames(const OCIO::ConstConfigRcPtr& config);
String GetDefaultSourceColorSpace(const OCIO::ConstConfigRcPtr& config);

Vector<String> GetDisplayNames(const OCIO::ConstConfigRcPtr& config);
String GetDefaultDisplay(const OCIO::ConstConfigRcPtr& config);

Vector<String> GetViewNames(const OCIO::ConstConfigRcPtr& config, const String& display);
String GetDefaultView(const OCIO::ConstConfigRcPtr& config, const String& display);

bool ApplyPreview(const OCIO::ConstConfigRcPtr& config, const String& source_color_space,
	const String& display, const String& view, const TestImageF& src, TestImageF& dst, String& error);

String DescribeSelection(const String& config_name, const String& source_color_space,
	const String& display, const String& view);

} // namespace OcioPreview
} // namespace Upp

#endif
