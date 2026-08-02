#include <Core/Core.h>
#include <ImagingCore/ImagingCore.h>

#include <cstdio>

using namespace Upp;
using namespace Upp::Imaging;

struct TestResult {
	int passed = 0;
	int failed = 0;
};

static void Check(TestResult& result, bool condition, const char* label)
{
	std::printf("%s: %s\n", label, condition ? "PASS" : "FAIL");
	(condition ? result.passed : result.failed)++;
}

int main()
{
	TestResult result;

	DataWindow window;
	window.left = -10;
	window.top = 20;
	window.right = 13;
	window.bottom = 35;
	Check(result, window.IsValid() && window.Width() == 24 && window.Height() == 16, "data window preserves origin");

	ImageSpec spec;
	spec.width = window.Width();
	spec.height = window.Height();
	spec.channels = 4;
	spec.channel_layout = ChannelLayout::RGBA;
	spec.sample_type = SampleType::Float32;
	spec.data_window = window;
	Check(result, spec.IsValid(), "image specification validates");

	ImageData image;
	image.spec = spec;
	image.buffer.SetSampleCount(spec.width * spec.height * spec.channels, SampleType::Float32);
	image.metadata.Set("source", "synthetic");
	Check(result, image.IsValid() && image.buffer.GetByteCount() == 6144, "typed image data validates");
	Check(result, image.metadata.Has("source") && image.metadata.Get("source") == "synthetic", "metadata round trip");

	Result failure = Result::Failure(ResultCode::InvalidArgument, "bad image");
	Check(result, !failure.IsOk() && failure.code == ResultCode::InvalidArgument, "result reports failure");

	Diagnostics diagnostics;
	diagnostics.Add("sample warning");
	Check(result, !diagnostics.IsEmpty() && diagnostics.messages.GetCount() == 1, "diagnostics collect messages");

	std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
	return result.failed ? 1 : 0;
}
