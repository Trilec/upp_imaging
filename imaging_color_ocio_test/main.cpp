#include <ImagingColor/ImagingColor.h>
#include <OpenColorIO/OpenColorIO.h>

#include <cmath>
#include <cstring>
#include <sstream>

namespace OCIO = OCIO_NAMESPACE;

using namespace Upp;
using namespace Upp::Imaging;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool ok, const char* name)
{
	Cout() << (ok ? "PASS " : "FAIL ") << name << '\n';
	(ok ? state.passed : state.failed)++;
}

static bool Near(float a, float b, float epsilon = 2e-5f)
{
	return std::fabs(a - b) <= epsilon;
}

static String ConfigText()
{
	return
		"ocio_profile_version: 2\n"
		"environment: {}\n"
		"search_path:\n"
		"  - .\n"
		"roles:\n"
		"  default: linear\n"
		"  scene_linear: linear\n"
		"colorspaces:\n"
		"  - !<ColorSpace>\n"
		"    name: linear\n"
		"    isdata: false\n"
		"  - !<ColorSpace>\n"
		"    name: log\n"
		"    isdata: false\n"
		"    to_scene_reference: !<GroupTransform>\n"
		"      children:\n"
		"        - !<MatrixTransform> {matrix: [2., 0., 0., 0., 0., 0.5, 0., 0., 0., 0., 1.5, 0., 0., 0., 0., 1.], offset: [0.1, 0.2, -0.1, 0.]}\n"
		"        - !<ExponentTransform> {value: [2., 2., 2., 1.]}\n"
		"        - !<RangeTransform> {min_in_value: 0., min_out_value: 0., max_in_value: 1., max_out_value: 1.}\n"
		"displays:\n"
		"  sRGB:\n"
		"    - !<View> {name: Raw, colorspace: linear}\n"
		"active_displays: [sRGB]\n"
		"active_views: [Raw]\n";
}

static ColorConfigRef FileConfig(const String& path)
{
	ColorConfigRef ref;
	ref.source = ColorConfigSource::File;
	ref.value = path;
	return ref;
}

static ImageData MakeRgba(float r, float g, float b, float a)
{
	ImageData image;
	image.spec.data_window = {-4, 8, -4, 8};
	image.spec.depth = 1;
	image.spec.channels = 4;
	image.spec.channel_layout = ChannelLayout::RGBA;
	image.spec.sample_type = SampleType::Float32;
	image.spec.channel_names.Add("R");
	image.spec.channel_names.Add("G");
	image.spec.channel_names.Add("B");
	image.spec.channel_names.Add("A");
	image.spec.alpha_channel = 3;
	image.buffer.Allocate(image.spec);
	float values[4] = {r, g, b, a};
	memcpy(image.buffer.Begin(), values, sizeof(values));
	image.metadata.Set("independent", "preserved");
	return image;
}

static void ReadRgba(const ImageData& image, float values[4])
{
	memcpy(values, image.buffer.Begin(), sizeof(float) * 4);
}

CONSOLE_APP_MAIN
{
	State state;
	String root = AppendFileName(GetTempPath(), "imaging_color_ocio_contract");
	RealizeDirectory(root);
	String config_path = AppendFileName(root, "independent.ocio");
	Check(state, SaveFile(config_path, ConfigText()), "independent config fixture creation");

	OCIO::ConstConfigRcPtr direct;
	try {
		direct = OCIO::Config::CreateFromFile(config_path.Begin());
		if(direct)
			direct->validate();
	}
	catch(...) {
		direct.reset();
	}
	Check(state, (bool)direct, "direct OCIO config validation");

	float expected[4] = {0.2f, 0.4f, 0.6f, 0.37f};
	bool direct_color_ok = false;
	if(direct) {
		try {
			OCIO::ConstProcessorRcPtr processor = direct->getProcessor("log", "linear");
			OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
			cpu->applyRGBA(expected);
			direct_color_ok = true;
		}
		catch(...) {
		}
	}
	Check(state, direct_color_ok, "direct OCIO color-space reference");

	ImageData source = MakeRgba(0.2f, 0.4f, 0.6f, 0.37f);
	ColorSpaceTransform color;
	color.config = FileConfig(config_path);
	color.source = "log";
	color.destination = "linear";
	ImageData actual;
	Diagnostics diagnostics;
	bool color_ok = ApplyColorSpaceTransform(source, actual, color, &diagnostics).IsOk();
	Check(state, color_ok, "ImagingColor color-space transform");
	float got[4] = {};
	if(color_ok)
		ReadRgba(actual, got);
	Check(state, color_ok && Near(got[0], expected[0]) && Near(got[1], expected[1]) &&
	             Near(got[2], expected[2]),
	      "ImagingColor matches direct OCIO RGB");
	Check(state, color_ok && got[3] == 0.37f && actual.metadata == source.metadata,
	      "ImagingColor preserves alpha and metadata");

	float display_expected[4] = {0.11f, 0.22f, 0.33f, 0.44f};
	bool direct_display_ok = false;
	if(direct) {
		try {
			OCIO::DisplayViewTransformRcPtr transform = OCIO::DisplayViewTransform::Create();
			transform->setSrc("linear");
			transform->setDisplay("sRGB");
			transform->setView("Raw");
			OCIO::ConstProcessorRcPtr processor = direct->getProcessor(transform);
			OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
			cpu->applyRGBA(display_expected);
			direct_display_ok = true;
		}
		catch(...) {
		}
	}
	Check(state, direct_display_ok, "direct OCIO display reference");

	ImageData display_source = MakeRgba(0.11f, 0.22f, 0.33f, 0.44f);
	DisplayTransform display;
	display.config = FileConfig(config_path);
	display.source = "linear";
	display.display = "sRGB";
	display.view = "Raw";
	ImageData display_actual;
	bool display_ok = ApplyDisplayTransform(display_source, display_actual, display,
	                                        &diagnostics).IsOk();
	Check(state, display_ok, "ImagingColor display transform");
	if(display_ok)
		ReadRgba(display_actual, got);
	Check(state, display_ok && Near(got[0], display_expected[0]) &&
	             Near(got[1], display_expected[1]) &&
	             Near(got[2], display_expected[2]),
	      "ImagingColor display matches direct OCIO RGB");
	Check(state, display_ok && got[3] == 0.44f, "display alpha preserved");

	ColorConfigInfo info;
	Check(state, InspectColorConfig(FileConfig(config_path), info, &diagnostics).IsOk(),
	      "ImagingColor config inspection");
	Check(state, direct && info.cache_id == direct->getCacheID(),
	      "config cache id matches direct OCIO");
	Check(state, direct &&
	             info.color_spaces.GetCount() == direct->getNumColorSpaces(),
	      "config color-space count matches direct OCIO");
	Check(state, direct &&
	             info.default_display == direct->getDefaultDisplay() &&
	             info.default_view == direct->getDefaultView(info.default_display.Begin()),
	      "config display defaults match direct OCIO");

	FileDelete(config_path);
	Check(state, !FileExists(config_path), "independent fixture cleanup");

	Cout() << "SUMMARY passed=" << state.passed << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
