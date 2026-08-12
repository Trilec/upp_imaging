#include <ImagingIO/ImagingIO.h>

#include <cstring>
#include <filesystem>
#include <fstream>

using namespace Upp;
using namespace Upp::Imaging;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const char* label)
{
	Cout() << (condition ? "PASS " : "FAIL ") << label << '\n';
	(condition ? state.passed : state.failed)++;
}

static bool HasCode(const Diagnostics& diagnostics, const char* code)
{
	for(const DiagnosticEntry& entry : diagnostics.Entries())
		if(entry.code == code)
			return true;
	return false;
}

static uint64 Hash(const ImageBuffer& buffer)
{
	uint64 hash = 1469598103934665603ULL;
	for(int i = 0; i < buffer.GetByteCount(); ++i) {
		hash ^= buffer.Begin()[i];
		hash *= 1099511628211ULL;
	}
	return hash;
}

static ImageData MakeImage(ChannelLayout layout, SampleType type = SampleType::UInt8,
                           int left = 0, int top = 0)
{
	ImageData image;
	int channels = layout == ChannelLayout::RGBA ? 4
	             : layout == ChannelLayout::RGB ? 3 : 1;
	image.spec.data_window = {left, top, left + 1, top + 1};
	image.spec.depth = 1;
	image.spec.channels = channels;
	image.spec.channel_layout = layout;
	image.spec.sample_type = type;
	image.spec.alpha_channel = layout == ChannelLayout::RGBA ? 3 : -1;
	if(layout == ChannelLayout::RGBA)
		image.spec.channel_names << "R" << "G" << "B" << "A";
	else if(layout == ChannelLayout::RGB)
		image.spec.channel_names << "R" << "G" << "B";
	else
		image.spec.channel_names << "Y";
	image.buffer.Allocate(image.spec);
	if(type == SampleType::UInt8 && layout == ChannelLayout::RGBA) {
		const byte pixels[] = {
			91, 37, 211, 0,    255, 12, 3, 64,
			17, 99, 201, 128,  4, 5, 6, 255
		};
		memcpy(image.buffer.Begin(), pixels, sizeof(pixels));
	}
	else {
		for(int i = 0; i < image.buffer.GetByteCount(); ++i)
			image.buffer.Begin()[i] = (byte)((i * 31 + 17) & 0xff);
	}
	return image;
}

static bool SamePixels(const ImageData& a, const ImageData& b)
{
	return a.buffer.GetByteCount() == b.buffer.GetByteCount() &&
	       (!a.buffer.GetByteCount() ||
	        memcmp(a.buffer.Begin(), b.buffer.Begin(), a.buffer.GetByteCount()) == 0);
}

static bool NoResidue(const std::filesystem::path& root)
{
	std::error_code error;
	for(const auto& entry : std::filesystem::directory_iterator(root, error)) {
		std::string name = entry.path().filename().string();
		if(name.find(".imagingio-") != std::string::npos ||
		   name.find(".imagingio-backup-") != std::string::npos)
			return false;
	}
	return !error;
}

CONSOLE_APP_MAIN
{
	State state;
	String root = AppendFileName(GetTempPath(), "webp_imagingio_contract");
	std::filesystem::path root_path(root.Begin());
	std::error_code error;
	std::filesystem::remove_all(root_path, error);
	std::filesystem::create_directories(root_path, error);
	Check(state, !error, "fixture directory creation");

	Diagnostics diagnostics;
	ImageData rgb = MakeImage(ChannelLayout::RGB);
	uint64 rgb_hash = Hash(rgb.buffer);
	String rgb_path = AppendFileName(root, "rgb.webp");
	Result rgb_save = SaveImageFile(rgb_path, rgb, &diagnostics);
	Check(state, rgb_save.IsOk(), "WebP RGB save");
	Check(state, Hash(rgb.buffer) == rgb_hash, "WebP RGB source unchanged");
	ImageData rgb_loaded;
	Result rgb_load = LoadImageFile(rgb_path, rgb_loaded, &diagnostics);
	Check(state, rgb_load.IsOk(), "WebP RGB load");
	Check(state, rgb_load.IsOk() && rgb_loaded.spec.sample_type == SampleType::UInt8 &&
	             rgb_loaded.spec.channel_layout == ChannelLayout::RGB &&
	             rgb_loaded.spec.data_window == DataWindow{0, 0, 1, 1},
	      "WebP RGB structure");
	Check(state, rgb_load.IsOk() && SamePixels(rgb, rgb_loaded),
	      "WebP RGB exact pixels");

	ImageData rgba = MakeImage(ChannelLayout::RGBA);
	uint64 rgba_hash = Hash(rgba.buffer);
	String rgba_path = AppendFileName(root, "rgba.webp");
	Result rgba_save = SaveImageFile(rgba_path, rgba, &diagnostics);
	Check(state, rgba_save.IsOk(), "WebP RGBA save");
	Check(state, Hash(rgba.buffer) == rgba_hash, "WebP RGBA source unchanged");
	ImageData rgba_loaded;
	Result rgba_load = LoadImageFile(rgba_path, rgba_loaded, &diagnostics);
	Check(state, rgba_load.IsOk(), "WebP RGBA load");
	Check(state, rgba_load.IsOk() && rgba_loaded.spec.sample_type == SampleType::UInt8 &&
	             rgba_loaded.spec.channel_layout == ChannelLayout::RGBA &&
	             rgba_loaded.spec.alpha_channel == 3,
	      "WebP RGBA structure");
	Check(state, rgba_load.IsOk() && SamePixels(rgba, rgba_loaded),
	      "WebP RGBA exact pixels including transparent RGB");

	ImageData u16 = MakeImage(ChannelLayout::RGB, SampleType::UInt16);
	String u16_path = AppendFileName(root, "u16.webp");
	Result u16_save = SaveImageFile(u16_path, u16, &diagnostics);
	Check(state, u16_save.code == ResultCode::Unsupported && HasCode(diagnostics, "IMGIO_SAMPLE"),
	      "WebP UInt16 save fails closed");
	Check(state, !std::filesystem::exists(u16_path.Begin()),
	      "WebP UInt16 refusal creates no file");

	ImageData gray = MakeImage(ChannelLayout::Gray);
	String gray_path = AppendFileName(root, "gray.webp");
	Result gray_save = SaveImageFile(gray_path, gray, &diagnostics);
	Check(state, gray_save.code == ResultCode::Unsupported && HasCode(diagnostics, "IMGIO_CHANNELS"),
	      "WebP Gray save fails closed");
	Check(state, !std::filesystem::exists(gray_path.Begin()),
	      "WebP Gray refusal creates no file");

	ImageData nonzero = MakeImage(ChannelLayout::RGB, SampleType::UInt8, 2, 3);
	String nonzero_path = AppendFileName(root, "nonzero.webp");
	Result nonzero_save = SaveImageFile(nonzero_path, nonzero, &diagnostics);
	Check(state, nonzero_save.code == ResultCode::Unsupported && HasCode(diagnostics, "IMGIO_SPEC"),
	      "WebP non-zero origin fails closed");
	Check(state, !std::filesystem::exists(nonzero_path.Begin()),
	      "WebP origin refusal creates no file");

	String invalid = AppendFileName(root, "invalid.webp");
	{
		std::ofstream stream(invalid.Begin(), std::ios::binary);
		for(int i = 0; i < 96; ++i) {
			byte value = (byte)((i * 53 + 7) & 0xff);
			stream.write((const char*)&value, 1);
		}
	}
	ImageData preserved = rgba_loaded;
	uint64 preserved_hash = Hash(preserved.buffer);
	Result invalid_load = LoadImageFile(invalid, preserved, &diagnostics);
	Check(state, invalid_load.code == ResultCode::IOError && HasCode(diagnostics, "IMGIO_OPEN"),
	      "malformed WebP fails with stable open diagnostic");
	Check(state, Hash(preserved.buffer) == preserved_hash &&
	             preserved.spec.channel_layout == ChannelLayout::RGBA,
	      "failed WebP load preserves prior output");

	Check(state, NoResidue(root_path), "WebP operations leave no transaction residue");
	std::filesystem::remove_all(root_path, error);
	Check(state, !std::filesystem::exists(root_path), "fixture cleanup");

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
