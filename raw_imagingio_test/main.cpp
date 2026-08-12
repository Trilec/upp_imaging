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

static ImageData MakeUInt16RGB()
{
	ImageData image;
	image.spec.data_window = {0, 0, 2, 1};
	image.spec.depth = 1;
	image.spec.channels = 3;
	image.spec.channel_layout = ChannelLayout::RGB;
	image.spec.sample_type = SampleType::UInt16;
	image.spec.alpha_channel = -1;
	image.spec.channel_names << "R" << "G" << "B";
	image.buffer.Allocate(image.spec);
	for(int64 i = 0; i < image.buffer.GetSampleCount(); ++i) {
		uint16 value = (uint16)((i * 4093 + 257) & 0xffff);
		memcpy(image.buffer.Begin() + i * 2, &value, sizeof(value));
	}
	return image;
}

static ImageData MakeHDRRGB()
{
	ImageData image;
	image.spec.data_window = {0, 0, 1, 1};
	image.spec.depth = 1;
	image.spec.channels = 3;
	image.spec.channel_layout = ChannelLayout::RGB;
	image.spec.sample_type = SampleType::Float32;
	image.spec.alpha_channel = -1;
	image.spec.channel_names << "R" << "G" << "B";
	image.buffer.Allocate(image.spec);
	const float values[] = {
		0.0f, 0.5f, 1.0f,
		2.0f, 4.0f, 8.0f,
		0.25f, 0.125f, 0.0625f,
		1.5f, 3.0f, 6.0f
	};
	memcpy(image.buffer.Begin(), values, sizeof(values));
	return image;
}

CONSOLE_APP_MAIN
{
	State state;
	String root = AppendFileName(GetTempPath(), "raw_imagingio_contract");
	std::filesystem::path root_path(root.Begin());
	std::error_code error;
	std::filesystem::remove_all(root_path, error);
	std::filesystem::create_directories(root_path, error);
	Check(state, !error, "fixture directory creation");

	Diagnostics diagnostics;
	ImageData rgb16 = MakeUInt16RGB();
	String dng = AppendFileName(root, "output.dng");
	Result dng_save = SaveImageFile(dng, rgb16, &diagnostics);
	Check(state, dng_save.code == ResultCode::Unsupported,
	      "DNG output is explicitly unsupported");
	Check(state, HasCode(diagnostics, "IMGIO_FORMAT"),
	      "DNG output reports IMGIO_FORMAT");
	Check(state, !std::filesystem::exists(dng.Begin()),
	      "DNG refusal creates no file");

	String nef = AppendFileName(root, "output.nef");
	Result nef_save = SaveImageFile(nef, rgb16, &diagnostics);
	Check(state, nef_save.code == ResultCode::Unsupported &&
	             HasCode(diagnostics, "IMGIO_FORMAT") &&
	             !std::filesystem::exists(nef.Begin()),
	      "NEF output is input-only and side-effect free");

	String invalid = AppendFileName(root, "invalid.dng");
	{
		std::ofstream stream(invalid.Begin(), std::ios::binary);
		for(int i = 0; i < 256; ++i) {
			unsigned char value = (unsigned char)((i * 61 + 23) & 0xff);
			stream.write((const char*)&value, 1);
		}
	}
	ImageData preserved = rgb16;
	uint64 before = Hash(preserved.buffer);
	Result invalid_load = LoadImageFile(invalid, preserved, &diagnostics);
	Check(state, invalid_load.code == ResultCode::IOError,
	      "malformed DNG reaches RAW reader and fails as I/O");
	Check(state, HasCode(diagnostics, "IMGIO_OPEN"),
	      "malformed DNG reports IMGIO_OPEN");
	Check(state, preserved.spec.sample_type == SampleType::UInt16 &&
	             preserved.spec.channel_layout == ChannelLayout::RGB &&
	             Hash(preserved.buffer) == before,
	      "failed RAW load preserves prior output");

	String hdr = AppendFileName(root, "radiance.hdr");
	ImageData hdr_source = MakeHDRRGB();
	Result hdr_save = SaveImageFile(hdr, hdr_source, &diagnostics);
	ImageData hdr_loaded;
	Result hdr_load = LoadImageFile(hdr, hdr_loaded, &diagnostics);
	Check(state, hdr_save.IsOk() && hdr_load.IsOk() &&
	             hdr_loaded.spec.sample_type == SampleType::Float32 &&
	             hdr_loaded.spec.channel_layout == ChannelLayout::RGB,
	      "Radiance HDR routing remains intact beside RAW");

	std::filesystem::remove_all(root_path, error);
	Check(state, !std::filesystem::exists(root_path), "fixture cleanup");

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
