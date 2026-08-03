#include <ImagingIO/ImagingIO.h>
#include <OpenImageIO/OIIO.h>

using namespace Upp;
using namespace Upp::Imaging;
using namespace OIIO;

struct State { int passed = 0; int failed = 0; };
static void Check(State& s, bool ok, const char* name) { Cout() << (ok ? "PASS " : "FAIL ") << name << '\n'; (ok ? s.passed : s.failed)++; }

CONSOLE_APP_MAIN
{
	State state; String root = AppendFileName(GetTempPath(), "imaging_io_oiio_contract"); RealizeDirectory(root); String exr = AppendFileName(root, "independent.exr");
	UppImaging::InitializeOpenImageIO();
	float pixels[12] = {0.1f, 0.2f, 0.3f, 1.0f, 0.4f, 0.5f, 0.6f, 1.0f, 0.7f, 0.8f, 0.9f, 1.0f};
	OIIO::ImageSpec spec(2, 2, 4, TypeDesc::FLOAT); spec.x = -3; spec.y = 4; spec.alpha_channel = 3; spec.channelnames = {"R", "G", "B", "A"}; spec.attribute("test:string", "independent"); spec.attribute("test:int", 17);
	ImageBuf image(spec, pixels); Check(state, image.write(exr.Begin()), "independent EXR creation");
	ImageData loaded; Diagnostics diagnostics; Result result = LoadImageFile(exr, loaded, &diagnostics);
	Check(state, result.IsOk(), "adapter loads independent EXR"); Check(state, loaded.spec.data_window == DataWindow{-3, 4, -2, 5} && loaded.spec.sample_type == SampleType::Float32 && loaded.spec.channel_layout == ChannelLayout::RGBA, "independent EXR structure"); Check(state, loaded.metadata.Get("test:string") == Value("independent") && loaded.metadata.Get("test:int") == Value(17), "independent EXR metadata");
	String adapter_exr = AppendFileName(root, "adapter.exr"); Check(state, SaveImageFile(adapter_exr, loaded, &diagnostics).IsOk(), "adapter EXR save"); ImageBuf reopened(adapter_exr.Begin()); Check(state, reopened.read() && reopened.spec().x == -3 && reopened.spec().y == 4 && reopened.spec().format == TypeDesc::FLOAT, "backend reads adapter-compatible EXR");
	FileDelete(exr); FileDelete(adapter_exr); Cout() << "SUMMARY passed=" << state.passed << " failed=" << state.failed << '\n'; SetExitCode(state.failed ? 1 : 0);
}
