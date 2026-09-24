#include <Imaging/Imaging.h>

using namespace Upp;
using namespace Upp::Imaging;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const char* name)
{
	Cout() << (condition ? "PASS " : "FAIL ") << name << '\n';
	(condition ? state.passed : state.failed)++;
}

CONSOLE_APP_MAIN
{
	State state;

	ImageData image;
	image.spec.data_window = {0, 0, 0, 0};
	image.spec.depth = 1;
	image.spec.channels = 1;
	image.spec.channel_layout = ChannelLayout::Gray;
	image.spec.sample_type = SampleType::UInt8;
	image.spec.alpha_channel = -1;
	image.spec.channel_names.Add("Y");
	Check(state, image.buffer.Allocate(image.spec) && image.IsValid(),
	      "ImagingCore contracts exposed through umbrella");
	image.buffer.Begin()[0] = 128;

	auto load = &LoadImageFile;
	auto save = &SaveImageFile;
	Check(state, load != nullptr && save != nullptr,
	      "ImagingIO contracts exposed through umbrella");

	auto inspect = &InspectColorConfig;
	auto transform = &ApplyColorSpaceTransform;
	Check(state, inspect != nullptr && transform != nullptr,
	      "ImagingColor contracts exposed through umbrella");

	Vector<ChannelStatistics> statistics;
	Diagnostics diagnostics;
	Check(state, ComputeChannelStatistics(image, statistics, &diagnostics).IsOk() &&
	      statistics.GetCount() == 1 && statistics[0].name == "Y",
	      "ImagingAnalysis executes through umbrella");

	NumericComparison comparison;
	Check(state, CompareNumeric(1.0, 1.0, comparison, NumericTolerance(), &diagnostics).IsOk() &&
	      comparison.match,
	      "ImagingDiagnostics executes through umbrella");

	Check(state, SampleTypeName(SampleType::UInt8) == "UInt8" &&
	      ChannelLayoutName(ChannelLayout::Gray) == "Gray",
	      "framework helper names exposed through umbrella");

	Cout() << "SUMMARY passed=" << state.passed
	       << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
