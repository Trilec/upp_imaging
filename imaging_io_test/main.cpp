#include <ImagingIO/ImagingIO.h>

using namespace Upp;
using namespace Upp::Imaging;

struct State { int passed = 0; int failed = 0; };
static void Check(State& s, bool ok, const char* name) { Cout() << (ok ? "PASS " : "FAIL ") << name << '\n'; (ok ? s.passed : s.failed)++; }

static ImageData MakeImage(SampleType type, ChannelLayout layout, int width, int height, int left = 0, int top = 0)
{
	ImageData image;
	image.spec.data_window = {left, top, left + width - 1, top + height - 1};
	image.spec.channels = CanonicalChannels(layout);
	image.spec.channel_layout = layout;
	image.spec.sample_type = type;
	image.spec.alpha_channel = layout == ChannelLayout::RGBA ? 3 : layout == ChannelLayout::GrayAlpha ? 1 : -1;
	const char* names[] = {"R", "G", "B", "A"};
	for(int i = 0; i < image.spec.channels; i++) image.spec.channel_names.Add(names[i]);
	image.buffer.Allocate(image.spec);
	for(int i = 0; i < image.buffer.GetByteCount(); i++) image.buffer.Begin()[i] = (byte)(i * 17 + 3);
	if(layout == ChannelLayout::RGBA && type == SampleType::UInt8)
		for(int pixel = 0; pixel < width * height; pixel++) image.buffer.Begin()[pixel * 4 + 3] = 255;
	return image;
}

static bool SameBytes(const ImageBuffer& a, const ImageBuffer& b) { return a.GetByteCount() == b.GetByteCount() && memcmp(a.Begin(), b.Begin(), a.GetByteCount()) == 0; }

CONSOLE_APP_MAIN
{
	State state;
	String root = AppendFileName(GetTempPath(), "imaging_io_contract"); RealizeDirectory(root); String exr = AppendFileName(root, "rgba.exr"), png = AppendFileName(root, "rgba.png");
	ImageData source = MakeImage(SampleType::Float32, ChannelLayout::RGBA, 4, 3, -2, 5); source.metadata.Set("name", Value("fixture")); source.metadata.Set("count", Value(7)); source.metadata.Set("large", Value((int64)9000000000LL)); source.metadata.Set("ratio", Value(0.5));
	Diagnostics diagnostics; Result saved = SaveImageFile(exr, source, &diagnostics); Check(state, saved.IsOk(), "EXR save");
	ImageData loaded; Result read = LoadImageFile(exr, loaded, &diagnostics); if(!read.IsOk()) Cout() << "EXR_ERROR " << read.message << " " << read.context << '\n'; Check(state, read.IsOk(), "EXR load"); Check(state, loaded.spec.data_window == source.spec.data_window && loaded.spec.sample_type == SampleType::Float32 && loaded.spec.channel_layout == ChannelLayout::RGBA && loaded.spec.alpha_channel == 3, "EXR structure"); Check(state, SameBytes(loaded.buffer, source.buffer), "EXR pixels");
	ImageData png_source = MakeImage(SampleType::UInt8, ChannelLayout::RGBA, 4, 3); Check(state, SaveImageFile(png, png_source, &diagnostics).IsOk(), "PNG save"); ImageData png_loaded; Result png_read = LoadImageFile(png, png_loaded, &diagnostics); if(!png_read.IsOk()) Cout() << "PNG_ERROR " << png_read.message << " " << png_read.context << '\n'; Check(state, png_read.IsOk(), "PNG load"); if(png_read.IsOk() && !SameBytes(png_loaded.buffer, png_source.buffer)) for(int i = 0; i < png_source.buffer.GetByteCount(); i++) if(png_source.buffer.Begin()[i] != png_loaded.buffer.Begin()[i]) { Cout() << "PNG_BYTES " << i << " " << (int)png_source.buffer.Begin()[i] << " " << (int)png_loaded.buffer.Begin()[i] << '\n'; break; } Check(state, SameBytes(png_loaded.buffer, png_source.buffer), "PNG pixels");
	ImageData preserved = source; Check(state, !LoadImageFile(String(), preserved, &diagnostics).IsOk() && preserved.spec.data_window == source.spec.data_window, "failed load preserves output");
	Check(state, SaveImageFile(AppendFileName(root, "bad.jpg"), source, &diagnostics).code == ResultCode::Unsupported, "unsupported extension");
	ImageData bad = source; bad.spec.sample_type = SampleType::UInt8; Check(state, SaveImageFile(AppendFileName(root, "bad.exr"), bad, &diagnostics).code == ResultCode::Unsupported, "unsupported EXR sample");
	ImageData nonzero_png = png_source; nonzero_png.spec.data_window.left = 1; nonzero_png.spec.data_window.right = 4; Check(state, SaveImageFile(AppendFileName(root, "bad.png"), nonzero_png, &diagnostics).code == ResultCode::Unsupported, "PNG origin policy");
	FileDelete(exr); FileDelete(png); FileDelete(AppendFileName(root, "bad.jpg")); FileDelete(AppendFileName(root, "bad.exr")); FileDelete(AppendFileName(root, "bad.png"));
	Cout() << "SUMMARY passed=" << state.passed << " failed=" << state.failed << '\n'; SetExitCode(state.failed ? 1 : 0);
}
