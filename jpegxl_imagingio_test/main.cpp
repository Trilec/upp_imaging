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

static Vector<String> CanonicalNames(ChannelLayout layout)
{
	Vector<String> names;
	switch(layout) {
	case ChannelLayout::Gray:
		names.Add("Y");
		break;
	case ChannelLayout::GrayAlpha:
		names.Add("Y"); names.Add("A");
		break;
	case ChannelLayout::RGB:
		names.Add("R"); names.Add("G"); names.Add("B");
		break;
	case ChannelLayout::RGBA:
		names.Add("R"); names.Add("G"); names.Add("B"); names.Add("A");
		break;
	default:
		break;
	}
	return names;
}

static void FillSamples(ImageBuffer& buffer, SampleType type)
{
	const int64 samples = buffer.GetSampleCount();
	switch(type) {
	case SampleType::UInt8:
		for(int64 i = 0; i < samples; ++i)
			buffer.Begin()[i] = (byte)((i * 37 + 11) & 0xff);
		break;
	case SampleType::UInt16:
		for(int64 i = 0; i < samples; ++i) {
			uint16 value = (uint16)((i * 4093 + 257) & 0xffff);
			memcpy(buffer.Begin() + i * 2, &value, sizeof(value));
		}
		break;
	case SampleType::Float16: {
		static const uint16 finite[] = {
			0x0000, 0x3400, 0x3800, 0x3a00, 0x3c00, 0x3e00, 0x4000, 0x4200
		};
		for(int64 i = 0; i < samples; ++i) {
			uint16 value = finite[i % (int)(sizeof(finite) / sizeof(finite[0]))];
			memcpy(buffer.Begin() + i * 2, &value, sizeof(value));
		}
		break;
	}
	case SampleType::Float32:
		for(int64 i = 0; i < samples; ++i) {
			float value = float(i % 17) / 16.0f;
			memcpy(buffer.Begin() + i * 4, &value, sizeof(value));
		}
		break;
	default:
		break;
	}
}

static ImageData MakeImage(SampleType type, ChannelLayout layout,
                           int width = 5, int height = 4,
                           int left = 0, int top = 0)
{
	ImageData image;
	image.spec.data_window = {left, top, left + width - 1, top + height - 1};
	image.spec.depth = 1;
	image.spec.channels = CanonicalChannels(layout);
	image.spec.channel_layout = layout;
	image.spec.sample_type = type;
	image.spec.alpha_channel = layout == ChannelLayout::RGBA ? 3
	                         : layout == ChannelLayout::GrayAlpha ? 1 : -1;
	image.spec.channel_names = CanonicalNames(layout);
	image.buffer.Allocate(image.spec);
	FillSamples(image.buffer, type);
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

static bool NoTransactionResidue(const std::filesystem::path& root)
{
	std::error_code error;
	if(!std::filesystem::exists(root, error))
		return true;
	for(const auto& entry : std::filesystem::directory_iterator(root, error)) {
		std::string name = entry.path().filename().string();
		if(name.find(".imagingio-") != std::string::npos ||
		   name.find(".imagingio-backup-") != std::string::npos)
			return false;
	}
	return !error;
}

static std::vector<byte> ReadFileBytes(const String& path)
{
	std::ifstream input(path.Begin(), std::ios::binary);
	return std::vector<byte>((std::istreambuf_iterator<char>(input)),
	                         std::istreambuf_iterator<char>());
}

static void CheckRoundTrip(State& state, const String& root,
                           const char* label, SampleType type,
                           ChannelLayout layout)
{
	ImageData source = MakeImage(type, layout);
	String path = AppendFileName(root, String(label) + ".jxl");
	Diagnostics diagnostics;
	uint64 hash = BufferHash(source.buffer);

	Result saved = SaveImageFile(path, source, &diagnostics);
	Check(state, saved.IsOk(), String(label) + " save");
	Check(state, BufferHash(source.buffer) == hash && source.IsValid(),
	      String(label) + " source unchanged");

	ImageData loaded;
	Result read = LoadImageFile(path, loaded, &diagnostics);
	bool exact = read.IsOk() &&
	             loaded.spec.data_window == source.spec.data_window &&
	             loaded.spec.sample_type == source.spec.sample_type &&
	             loaded.spec.channel_layout == source.spec.channel_layout &&
	             loaded.spec.alpha_channel == source.spec.alpha_channel &&
	             SameBytes(loaded.buffer, source.buffer);
	Check(state, exact, String(label) + " lossless exact roundtrip");
}

CONSOLE_APP_MAIN
{
	State state;
	String root = AppendFileName(GetTempPath(), "jpegxl_imagingio_contract");
	std::filesystem::path root_path(root.Begin());
	std::error_code error;
	std::filesystem::remove_all(root_path, error);
	error.clear();
	std::filesystem::create_directories(root_path, error);
	Check(state, !error, "fixture directory creation");

	CheckRoundTrip(state, root, "u8_gray", SampleType::UInt8, ChannelLayout::Gray);
	CheckRoundTrip(state, root, "u8_rgb", SampleType::UInt8, ChannelLayout::RGB);
	CheckRoundTrip(state, root, "u8_rgba", SampleType::UInt8, ChannelLayout::RGBA);
	CheckRoundTrip(state, root, "u16_gray", SampleType::UInt16, ChannelLayout::Gray);
	CheckRoundTrip(state, root, "u16_rgb", SampleType::UInt16, ChannelLayout::RGB);
	CheckRoundTrip(state, root, "u16_rgba", SampleType::UInt16, ChannelLayout::RGBA);
	CheckRoundTrip(state, root, "f16_gray", SampleType::Float16, ChannelLayout::Gray);
	CheckRoundTrip(state, root, "f16_rgb", SampleType::Float16, ChannelLayout::RGB);
	CheckRoundTrip(state, root, "f16_rgba", SampleType::Float16, ChannelLayout::RGBA);
	CheckRoundTrip(state, root, "f32_gray", SampleType::Float32, ChannelLayout::Gray);
	CheckRoundTrip(state, root, "f32_rgb", SampleType::Float32, ChannelLayout::RGB);
	CheckRoundTrip(state, root, "f32_rgba", SampleType::Float32, ChannelLayout::RGBA);

	Diagnostics diagnostics;
	ImageData gray_alpha = MakeImage(SampleType::UInt8, ChannelLayout::GrayAlpha);
	Result gray_alpha_result = SaveImageFile(AppendFileName(root, "grayalpha.jxl"),
	                                         gray_alpha, &diagnostics);
	Check(state, gray_alpha_result.code == ResultCode::Unsupported,
	      "GrayAlpha output is fail-closed");
	Check(state, HasCode(diagnostics, "IMGIO_CHANNELS"),
	      "GrayAlpha refusal has stable channel diagnostic");

	ImageData multi;
	multi.spec.data_window = {0, 0, 3, 2};
	multi.spec.depth = 1;
	multi.spec.channels = 3;
	multi.spec.channel_layout = ChannelLayout::MultiChannel;
	multi.spec.sample_type = SampleType::UInt8;
	multi.spec.alpha_channel = -1;
	multi.spec.channel_names.Add("depth");
	multi.spec.channel_names.Add("confidence");
	multi.spec.channel_names.Add("mask");
	multi.buffer.Allocate(multi.spec);
	FillSamples(multi.buffer, multi.spec.sample_type);
	Result multi_result = SaveImageFile(AppendFileName(root, "multi.jxl"),
	                                    multi, &diagnostics);
	Check(state, multi_result.code == ResultCode::Unsupported,
	      "MultiChannel output is fail-closed");
	Check(state, HasCode(diagnostics, "IMGIO_CHANNELS"),
	      "MultiChannel refusal has stable channel diagnostic");

	ImageData nonzero = MakeImage(SampleType::UInt16, ChannelLayout::RGB,
	                              4, 3, -2, 5);
	Result origin_result = SaveImageFile(AppendFileName(root, "nonzero.jxl"),
	                                     nonzero, &diagnostics);
	Check(state, origin_result.code == ResultCode::Unsupported,
	      "non-zero-origin output is fail-closed");
	Check(state, HasCode(diagnostics, "IMGIO_SPEC"),
	      "non-zero-origin refusal has stable spec diagnostic");

	String replacement = AppendFileName(root, "replacement.jxl");
	ImageData seed = MakeImage(SampleType::UInt8, ChannelLayout::RGB, 3, 2);
	Check(state, SaveImageFile(replacement, seed, &diagnostics).IsOk(),
	      "replacement seed save");
	std::vector<byte> before_failure = ReadFileBytes(replacement);
	Result refused_replacement = SaveImageFile(replacement, gray_alpha, &diagnostics);
	Check(state, refused_replacement.code == ResultCode::Unsupported,
	      "invalid replacement is refused");
	Check(state, !before_failure.empty() && ReadFileBytes(replacement) == before_failure,
	      "failed replacement preserves destination bytes");

	ImageData published = MakeImage(SampleType::UInt8, ChannelLayout::RGBA, 6, 3);
	Check(state, SaveImageFile(replacement, published, &diagnostics).IsOk(),
	      "verified replacement save");
	ImageData reopened;
	Result reopened_result = LoadImageFile(replacement, reopened, &diagnostics);
	Check(state, reopened_result.IsOk() &&
	      reopened.spec.channel_layout == ChannelLayout::RGBA &&
	      SameBytes(reopened.buffer, published.buffer),
	      "verified replacement publishes exact new contents");

	Check(state, NoTransactionResidue(root_path),
	      "no temporary or backup residue");

	std::filesystem::remove_all(root_path, error);
	Check(state, !std::filesystem::exists(root_path), "fixture cleanup");

	Cout() << "SUMMARY passed=" << state.passed
	       << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
