#include <plugin/exr/exr.h>
#include <ImagingIO/ImagingIO.h>

#include <filesystem>
#include <cstring>
#include <initializer_list>
#include <limits>

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

static ImageData MakeFloatImage(ChannelLayout layout, int channels,
                                const Vector<String>& names, int alpha,
                                int width, int height)
{
	ImageData image;
	image.spec.data_window = {0, 0, width - 1, height - 1};
	image.spec.depth = 1;
	image.spec.channels = channels;
	image.spec.channel_layout = layout;
	image.spec.sample_type = SampleType::Float32;
	image.spec.alpha_channel = alpha;
	for(const String& name : names)
		image.spec.channel_names.Add(name);
	image.buffer.Allocate(image.spec);
	return image;
}

static void SetFloat(ImageData& image, int64 sample, float value)
{
	memcpy(image.buffer.Begin() + sample * sizeof(float), &value, sizeof(value));
}

static Vector<String> Names(std::initializer_list<const char*> list)
{
	Vector<String> names;
	for(const char* name : list)
		names.Add(name);
	return names;
}

static bool RawPixel(EXRRaster& raster, int x, int y, RGBA& pixel)
{
	Raster::Line line = raster.GetLine(y);
	const byte* raw = line.GetRawData();
	if(!raw)
		return false;
	memcpy(&pixel, raw + x * sizeof(RGBA), sizeof(RGBA));
	return true;
}

CONSOLE_APP_MAIN
{
	State state;
	String root = AppendFileName(GetTempPath(), "plugin_exr_contract");
	std::filesystem::path root_path(root.Begin());
	std::error_code error;
	std::filesystem::remove_all(root_path, error);
	error.clear();
	std::filesystem::create_directories(root_path, error);
	Check(state, !error, "fixture directory creation");

	Diagnostics diagnostics;
	ImageData rgba = MakeFloatImage(ChannelLayout::RGBA, 4,
	                                Names({"R", "G", "B", "A"}), 3, 2, 1);
	const float rgba_values[] = {
		0.0f, 0.5f, 1.0f, 1.0f,
		2.0f, -1.0f, 0.25f, 0.5f,
	};
	for(int i = 0; i < 8; ++i)
		SetFloat(rgba, i, rgba_values[i]);
	String rgba_path = AppendFileName(root, "preview-rgba.exr");
	Check(state, SaveImageFile(rgba_path, rgba, &diagnostics).IsOk(),
	      "RGBA EXR fixture save");
	String rgba_encoded = LoadFile(rgba_path);
	Check(state, !rgba_encoded.IsEmpty(), "RGBA EXR fixture encoded");

	StringStream rgba_stream(rgba_encoded);
	EXRRaster rgba_raster;
	Check(state, rgba_raster.Open(rgba_stream) && rgba_raster.GetSize() == Size(2, 1),
	      "EXRRaster opens encoded RGBA stream");
	RGBA first = {}, second = {};
	Check(state, RawPixel(rgba_raster, 0, 0, first) &&
	      first.r == 0 && first.g == 128 && first.b == 255 && first.a == 255,
	      "RGBA unit-range preview bytes");
	Check(state, RawPixel(rgba_raster, 1, 0, second) &&
	      second.r == 255 && second.g == 0 && second.b == 64 && second.a == 128,
	      "RGBA HDR clamp and straight alpha");
	Check(state, rgba_raster.GetInfo().kind == IMAGE_ALPHA &&
	      rgba_raster.GetFormat() && rgba_raster.GetFormat()->HasAlpha(),
	      "RGBA raster reports alpha format");

	Image registered = StreamRaster::LoadStringAny(rgba_encoded);
	Check(state, !registered.IsEmpty() && registered.GetSize() == Size(2, 1),
	      "StreamRaster registration loads EXR from memory");

	ImageData gray = MakeFloatImage(ChannelLayout::Gray, 1, Names({"Y"}), -1, 1, 1);
	SetFloat(gray, 0, 0.25f);
	String gray_path = AppendFileName(root, "preview-gray.exr");
	Check(state, SaveImageFile(gray_path, gray, &diagnostics).IsOk(),
	      "Gray EXR fixture save");
	StringStream gray_stream(LoadFile(gray_path));
	EXRRaster gray_raster;
	RGBA gray_pixel = {};
	Check(state, gray_raster.Open(gray_stream) && RawPixel(gray_raster, 0, 0, gray_pixel) &&
	      gray_pixel.r == 64 && gray_pixel.g == 64 && gray_pixel.b == 64 && gray_pixel.a == 255,
	      "Gray preview replicates to opaque RGB");
	Check(state, gray_raster.GetInfo().kind == IMAGE_OPAQUE,
	      "Gray raster reports opaque preview");

	ImageData gray_alpha = MakeFloatImage(ChannelLayout::GrayAlpha, 2,
	                                      Names({"Y", "A"}), 1, 2, 1);
	const float gray_alpha_values[] = {
		0.75f, 0.25f,
		std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::infinity(),
	};
	for(int i = 0; i < 4; ++i)
		SetFloat(gray_alpha, i, gray_alpha_values[i]);
	String gray_alpha_path = AppendFileName(root, "preview-gray-alpha.exr");
	Check(state, SaveImageFile(gray_alpha_path, gray_alpha, &diagnostics).IsOk(),
	      "GrayAlpha EXR fixture save");
	StringStream gray_alpha_stream(LoadFile(gray_alpha_path));
	EXRRaster gray_alpha_raster;
	RGBA gray_alpha_first = {}, gray_alpha_second = {};
	Check(state, gray_alpha_raster.Open(gray_alpha_stream) &&
	      RawPixel(gray_alpha_raster, 0, 0, gray_alpha_first) &&
	      gray_alpha_first.r == 191 && gray_alpha_first.g == 191 &&
	      gray_alpha_first.b == 191 && gray_alpha_first.a == 64,
	      "GrayAlpha preview preserves straight alpha");
	Check(state, RawPixel(gray_alpha_raster, 1, 0, gray_alpha_second) &&
	      gray_alpha_second.r == 0 && gray_alpha_second.g == 0 &&
	      gray_alpha_second.b == 0 && gray_alpha_second.a == 0,
	      "non-finite preview samples map to zero");
	Check(state, gray_alpha_raster.GetInfo().kind == IMAGE_ALPHA,
	      "GrayAlpha raster reports alpha preview");

	ImageData mask = MakeFloatImage(ChannelLayout::MultiChannel, 1,
	                                Names({"mask"}), -1, 1, 1);
	SetFloat(mask, 0, 0.6f);
	String mask_path = AppendFileName(root, "preview-mask.exr");
	Check(state, SaveImageFile(mask_path, mask, &diagnostics).IsOk(),
	      "single-channel mask EXR fixture save");
	StringStream mask_stream(LoadFile(mask_path));
	EXRRaster mask_raster;
	RGBA mask_pixel = {};
	Check(state, mask_raster.Open(mask_stream) && RawPixel(mask_raster, 0, 0, mask_pixel) &&
	      mask_pixel.r == 153 && mask_pixel.g == 153 &&
	      mask_pixel.b == 153 && mask_pixel.a == 255 &&
	      mask_raster.GetInfo().kind == IMAGE_OPAQUE,
	      "single-channel mask previews as opaque gray");

	ImageData multi = MakeFloatImage(ChannelLayout::MultiChannel, 5,
	                                 Names({"Z", "R", "G", "B", "A"}), 4, 1, 1);
	const float multi_values[] = {5.0f, 0.2f, 0.4f, 0.6f, 0.8f};
	for(int i = 0; i < 5; ++i)
		SetFloat(multi, i, multi_values[i]);
	String multi_path = AppendFileName(root, "preview-multi.exr");
	Check(state, SaveImageFile(multi_path, multi, &diagnostics).IsOk(),
	      "named MultiChannel EXR fixture save");
	StringStream multi_stream(LoadFile(multi_path));
	EXRRaster multi_raster;
	RGBA multi_pixel = {};
	Check(state, multi_raster.Open(multi_stream) && RawPixel(multi_raster, 0, 0, multi_pixel) &&
	      multi_pixel.r == 51 && multi_pixel.g == 102 && multi_pixel.b == 153 && multi_pixel.a == 204,
	      "named MultiChannel selects RGB and ignores extra channel");

	StringStream invalid_stream("not an exr file");
	EXRRaster invalid_raster;
	Check(state, !invalid_raster.Open(invalid_stream), "non-EXR stream rejected");

	String truncated = rgba_encoded.Left(min(12, rgba_encoded.GetLength()));
	StringStream truncated_stream(truncated);
	EXRRaster truncated_raster;
	Check(state, !truncated_raster.Open(truncated_stream), "truncated EXR stream rejected");

	std::filesystem::remove_all(root_path, error);
	Check(state, !std::filesystem::exists(root_path), "fixture cleanup");

	Cout() << "SUMMARY passed=" << state.passed
	       << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
