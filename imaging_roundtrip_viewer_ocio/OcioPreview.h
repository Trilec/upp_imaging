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
