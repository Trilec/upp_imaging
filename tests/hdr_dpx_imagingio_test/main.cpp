#include <ImagingIO/ImagingIO.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>

using namespace Upp;
using namespace Upp::Imaging;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const String& name)
{
	Cout() << (condition ? "PASS " : "FAIL ") << name << '\n';
	(condition ? state.passed : state.failed)++;
}

static Vector<String> Names(ChannelLayout layout)
{
	Vector<String> names;
	if(layout == ChannelLayout::RGB) {
		names.Add("R"); names.Add("G"); names.Add("B");
	}
	else if(layout == ChannelLayout::RGBA) {
		names.Add("R"); names.Add("G"); names.Add("B"); names.Add("A");
	}
	else if(layout == ChannelLayout::Gray) {
		names.Add("Y");
	}
	return names;
}

static ImageData MakeImage(SampleType type, ChannelLayout layout,
                           int width = 8, int height = 5,
                           int left = 0, int top = 0)
{
	ImageData image;
	image.spec.data_window = {left, top, left + width - 1, top + height - 1};
	image.spec.depth = 1;
	image.spec.channels = CanonicalChannels(layout);
	image.spec.channel_layout = layout;
	image.spec.sample_type = type;
	image.spec.alpha_channel = layout == ChannelLayout::RGBA ? 3 : -1;
	image.spec.channel_names = Names(layout);
	image.buffer.Allocate(image.spec);

	int64 samples = image.buffer.GetSampleCount();
	if(type == SampleType::UInt8) {
		for(int64 i = 0; i < samples; ++i)
			image.buffer.Begin()[i] = (byte)((i * 43 + 17) & 0xff);
	}
	else if(type == SampleType::UInt16) {
		for(int64 i = 0; i < samples; ++i) {
			uint16 value = (uint16)((i * 1009 + 97) & 0xffff);
			memcpy(image.buffer.Begin() + i * 2, &value, sizeof(value));
		}
	}
	else if(type == SampleType::Float32) {
		static const float rgb[][3] = {
			{0.25f, 0.5f, 1.0f}, {1.0f, 0.5f, 0.25f},
			{2.0f, 4.0f, 8.0f}, {8.0f, 4.0f, 2.0f},
			{0.0f, 0.0f, 0.0f}, {0.125f, 0.25f, 0.5f},
			{16.0f, 8.0f, 4.0f}, {0.5f, 0.25f, 0.125f}
		};
		for(int64 pixel = 0; pixel < samples / image.spec.channels; ++pixel) {
			for(int c = 0; c < image.spec.channels; ++c) {
				float value = c < 3 ? rgb[pixel % 8][c] : 1.0f;
				memcpy(image.buffer.Begin() + (pixel * image.spec.channels + c) * 4,
				       &value, sizeof(value));
			}
		}
	}
	return image;
}

static bool SameBytes(const ImageBuffer& a, const ImageBuffer& b)
{
	return a.GetByteCount() == b.GetByteCount() &&
	       (!a.GetByteCount() ||
	        memcmp(a.Begin(), b.Begin(), a.GetByteCount()) == 0);
}

static uint64 BufferHash(const ImageBuffer& buffer)
{
	uint64 hash = 1469598103934665603ULL;
	for(int i = 0; i < buffer.GetByteCount(); ++i) {
		hash ^= buffer.Begin()[i];
		hash *= 1099511628211ULL;
	}
	return hash;
}

static bool HasCode(const Diagnostics& diagnostics, const char* code)
{
	for(const DiagnosticEntry& entry : diagnostics.Entries())
		if(entry.code == code)
			return true;
	return false;
}

static std::vector<byte> ReadFileBytes(const String& path)
{
	std::ifstream input(path.Begin(), std::ios::binary);
	return std::vector<byte>((std::istreambuf_iterator<char>(input)),
	                         std::istreambuf_iterator<char>());
}

static bool NoTransactionResidue(const std::filesystem::path& root)
{
	std::error_code error;
	if(!std::filesystem::exists(root, error))
		return true;
	for(const auto& entry : std::filesystem::directory_iterator(root, error)) {
		String name = entry.path().filename().string().c_str();
		if(name.Find(".imagingio-") >= 0 ||
		   name.Find(".imagingio-backup-") >= 0)
			return false;
	}
	return !error;
}

static void CheckRoundtrip(State& state, const String& path,
                           const String& label, ImageData source)
{
	Diagnostics diagnostics;
	uint64 hash = BufferHash(source.buffer);
	Result saved = SaveImageFile(path, source, &diagnostics);
	Check(state, saved.IsOk(), label + " save");
	Check(state, source.IsValid() && BufferHash(source.buffer) == hash,
	      label + " source unchanged");

	ImageData loaded;
	Result read = LoadImageFile(path, loaded, &diagnostics);
	bool exact = read.IsOk() &&
	             loaded.spec.data_window == source.spec.data_window &&
	             loaded.spec.sample_type == source.spec.sample_type &&
	             loaded.spec.channel_layout == source.spec.channel_layout &&
	             SameBytes(loaded.buffer, source.buffer);
	Check(state, read.IsOk(), label + " reload");
	Check(state, exact, label + " exact represented pixels/spec");
}

CONSOLE_APP_MAIN
{
	State state;
	String root = AppendFileName(GetTempPath(), "hdr_dpx_imagingio_contract");
	std::filesystem::path root_path(root.Begin());
	std::error_code error;
	std::filesystem::remove_all(root_path, error);
	error.clear();
	std::filesystem::create_directories(root_path, error);
	Check(state, !error, "fixture directory creation");

	CheckRoundtrip(state, AppendFileName(root, "scene.hdr"), "HDR",
	               MakeImage(SampleType::Float32, ChannelLayout::RGB));
	CheckRoundtrip(state, AppendFileName(root, "scene.rgbe"), "RGBE",
	               MakeImage(SampleType::Float32, ChannelLayout::RGB));

	Diagnostics diagnostics;
	ImageData hdr_integer = MakeImage(SampleType::UInt16, ChannelLayout::RGB);
	Result hdr_integer_result = SaveImageFile(AppendFileName(root, "integer.hdr"),
	                                          hdr_integer, &diagnostics);
	Check(state, hdr_integer_result.code == ResultCode::Unsupported,
	      "HDR integer output is refused");
	Check(state, HasCode(diagnostics, "IMGIO_SAMPLE"),
	      "HDR integer refusal has sample diagnostic");

	ImageData hdr_rgba = MakeImage(SampleType::Float32, ChannelLayout::RGBA);
	Result hdr_rgba_result = SaveImageFile(AppendFileName(root, "rgba.hdr"),
	                                       hdr_rgba, &diagnostics);
	Check(state, hdr_rgba_result.code == ResultCode::Unsupported,
	      "HDR RGBA output is refused");
	Check(state, HasCode(diagnostics, "IMGIO_CHANNELS"),
	      "HDR RGBA refusal has channel diagnostic");

	ImageData hdr_origin = MakeImage(SampleType::Float32, ChannelLayout::RGB,
	                                 5, 4, 3, 2);
	Result hdr_origin_result = SaveImageFile(AppendFileName(root, "origin.hdr"),
	                                         hdr_origin, &diagnostics);
	Check(state, hdr_origin_result.code == ResultCode::Unsupported,
	      "HDR non-zero origin is refused");
	Check(state, HasCode(diagnostics, "IMGIO_SPEC"),
	      "HDR origin refusal has spec diagnostic");

	CheckRoundtrip(state, AppendFileName(root, "rgb8.dpx"), "DPX UInt8",
	               MakeImage(SampleType::UInt8, ChannelLayout::RGB));
	CheckRoundtrip(state, AppendFileName(root, "rgb16_origin.dpx"), "DPX UInt16 origin",
	               MakeImage(SampleType::UInt16, ChannelLayout::RGB, 7, 4, 9, 6));

	ImageData dpx_float = MakeImage(SampleType::Float32, ChannelLayout::RGB);
	Result dpx_float_result = SaveImageFile(AppendFileName(root, "float.dpx"),
	                                        dpx_float, &diagnostics);
	Check(state, dpx_float_result.code == ResultCode::Unsupported,
	      "DPX Float32 output is refused");
	Check(state, HasCode(diagnostics, "IMGIO_SAMPLE"),
	      "DPX Float32 refusal has sample diagnostic");

	ImageData dpx_rgba = MakeImage(SampleType::UInt8, ChannelLayout::RGBA);
	Result dpx_rgba_result = SaveImageFile(AppendFileName(root, "rgba.dpx"),
	                                       dpx_rgba, &diagnostics);
	Check(state, dpx_rgba_result.code == ResultCode::Unsupported,
	      "DPX RGBA output is deferred");
	Check(state, HasCode(diagnostics, "IMGIO_CHANNELS"),
	      "DPX RGBA refusal has channel diagnostic");

	ImageData dpx_negative = MakeImage(SampleType::UInt16, ChannelLayout::RGB,
	                                   5, 3, -1, 2);
	Result dpx_negative_result = SaveImageFile(AppendFileName(root, "negative.dpx"),
	                                           dpx_negative, &diagnostics);
	Check(state, dpx_negative_result.code == ResultCode::Unsupported,
	      "DPX negative origin is refused");
	Check(state, HasCode(diagnostics, "IMGIO_SPEC"),
	      "DPX negative origin refusal has spec diagnostic");

	ImageData cineon = MakeImage(SampleType::UInt16, ChannelLayout::RGB);
	Result cineon_save = SaveImageFile(AppendFileName(root, "output.cin"),
	                                   cineon, &diagnostics);
	Check(state, cineon_save.code == ResultCode::Unsupported,
	      "Cineon output is explicitly unsupported");
	Check(state, HasCode(diagnostics, "IMGIO_FORMAT"),
	      "Cineon output refusal has format diagnostic");

	String replacement = AppendFileName(root, "replacement.dpx");
	ImageData seed = MakeImage(SampleType::UInt8, ChannelLayout::RGB, 4, 3);
	Check(state, SaveImageFile(replacement, seed, &diagnostics).IsOk(),
	      "DPX replacement seed save");
	std::vector<byte> before_failure = ReadFileBytes(replacement);
	Result refused = SaveImageFile(replacement, dpx_rgba, &diagnostics);
	Check(state, refused.code == ResultCode::Unsupported,
	      "invalid DPX replacement is refused");
	Check(state, !before_failure.empty() && ReadFileBytes(replacement) == before_failure,
	      "failed DPX replacement preserves destination bytes");

	ImageData published = MakeImage(SampleType::UInt16, ChannelLayout::RGB,
	                                6, 3, 4, 5);
	Check(state, SaveImageFile(replacement, published, &diagnostics).IsOk(),
	      "verified DPX replacement save");
	ImageData reopened;
	Result reopened_result = LoadImageFile(replacement, reopened, &diagnostics);
	Check(state, reopened_result.IsOk() &&
	      reopened.spec.data_window == published.spec.data_window &&
	      reopened.spec.sample_type == published.spec.sample_type &&
	      SameBytes(reopened.buffer, published.buffer),
	      "verified DPX replacement publishes exact new contents");

	Check(state, NoTransactionResidue(root_path),
	      "no temporary or backup residue");
	std::filesystem::remove_all(root_path, error);
	Check(state, !std::filesystem::exists(root_path), "fixture cleanup");

	Cout() << "SUMMARY passed=" << state.passed
	       << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
