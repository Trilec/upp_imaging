#include <ImagingIO/ImagingIO.h>

#include <filesystem>
#include <cstring>

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

static ImageData MakeImage(SampleType type, ChannelLayout layout,
                           int width, int height, int left = 0, int top = 0)
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
	for(int i = 0; i < image.buffer.GetByteCount(); ++i)
		image.buffer.Begin()[i] = (byte)((i * 29 + 7) & 0xff);
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
	return true;
}

static void CheckRoundTrip(State& state, const String& root,
                           const char* label, const char* filename,
                           const ImageData& source)
{
	String path = AppendFileName(root, filename);
	Diagnostics diagnostics;
	uint64 before = BufferHash(source.buffer);
	Result saved = SaveImageFile(path, source, &diagnostics);
	Check(state, saved.IsOk(), Format("%s save", label));
	Check(state, BufferHash(source.buffer) == before && source.IsValid(),
	      Format("%s source unchanged", label));
	ImageData loaded;
	Result read = LoadImageFile(path, loaded, &diagnostics);
	Check(state, read.IsOk(), Format("%s load", label));
	Check(state, read.IsOk() && loaded.spec.data_window == source.spec.data_window &&
	      loaded.spec.sample_type == source.spec.sample_type &&
	      loaded.spec.channel_layout == source.spec.channel_layout &&
	      loaded.spec.alpha_channel == source.spec.alpha_channel,
	      Format("%s structure", label));
	Check(state, read.IsOk() && SameBytes(loaded.buffer, source.buffer),
	      Format("%s pixels", label));
}

CONSOLE_APP_MAIN
{
	ImageIOApplicationScope io_scope;
	State state;
	String root = AppendFileName(GetTempPath(), "imaging_io_contract");
	std::filesystem::path root_path(root.Begin());
	std::error_code error;
	std::filesystem::remove_all(root_path, error);
	std::filesystem::create_directories(root_path, error);
	Check(state, !error, "fixture directory creation");

	ImageData exr = MakeImage(SampleType::Float32, ChannelLayout::RGBA,
	                          4, 3, -2, 5);
	exr.metadata.Set("test:string", Value("fixture"));
	exr.metadata.Set("test:int", Value(7));
	exr.metadata.Set("test:ratio", Value(0.5));
	ValueArray ints; ints.Add(1); ints.Add(2); ints.Add(3);
	ValueArray doubles; doubles.Add(0.25); doubles.Add(0.5);
	exr.metadata.Set("test:ints", Value(ints));
	exr.metadata.Set("test:doubles", Value(doubles));
	exr.metadata.Set("test:large", Value((int64)9000000000LL));
	exr.metadata.Set("compression", Value("contradictory"));

	String exr_path = AppendFileName(root, "rgba.exr");
	Diagnostics diagnostics;
	uint64 exr_hash = BufferHash(exr.buffer);
	Result exr_save = SaveImageFile(exr_path, exr, &diagnostics);
	Check(state, exr_save.IsOk(), "EXR Float32 RGBA save");
	Check(state, HasCode(diagnostics, "IMGIO_METADATA"),
	      "EXR metadata omissions are diagnosed");
	Check(state, BufferHash(exr.buffer) == exr_hash,
	      "EXR save leaves source pixels unchanged");

	ImageData exr_loaded;
	Result exr_read = LoadImageFile(exr_path, exr_loaded, &diagnostics);
	Check(state, exr_read.IsOk(), "EXR Float32 RGBA load");
	Check(state, exr_read.IsOk() &&
	      exr_loaded.spec.data_window == DataWindow{-2, 5, 1, 7} &&
	      exr_loaded.spec.channel_layout == ChannelLayout::RGBA &&
	      exr_loaded.spec.sample_type == SampleType::Float32,
	      "EXR Float32 RGBA structure");
	Check(state, exr_read.IsOk() && SameBytes(exr.buffer, exr_loaded.buffer),
	      "EXR Float32 RGBA exact pixels");
	Check(state, exr_loaded.metadata.Get("test:string") == Value("fixture") &&
	      exr_loaded.metadata.Get("test:int") == Value(7),
	      "EXR scalar metadata round trip");
	Check(state, exr_loaded.metadata.Get("test:ints").Is<ValueArray>() &&
	      exr_loaded.metadata.Get("test:doubles").Is<ValueArray>(),
	      "EXR array metadata remains typed arrays");

	ImageData half;
	half.spec.data_window = {-1, 2, 0, 3};
	half.spec.depth = 1;
	half.spec.channels = 3;
	half.spec.channel_layout = ChannelLayout::MultiChannel;
	half.spec.sample_type = SampleType::Float16;
	half.spec.alpha_channel = -1;
	half.spec.channel_names.Add("confidence");
	half.spec.channel_names.Add("mask");
	half.spec.channel_names.Add("temperature");
	Check(state, half.buffer.Allocate(half.spec),
	      "EXR Float16 MultiChannel allocation");
	const uint16 patterns[] = {0x0000, 0x8000, 0x3c00, 0xc000, 0x3555, 0x7bff};
	Check(state, half.buffer.GetSampleCount() * 2 == half.buffer.GetByteCount(),
	      "EXR Float16 fixture bounds");
	for(int64 i = 0; i < half.buffer.GetSampleCount(); ++i) {
		uint16 value = patterns[i % 6];
		memcpy(half.buffer.Begin() + i * 2, &value, sizeof(value));
	}
	String half_path = AppendFileName(root, "multi.exr");
	Check(state, SaveImageFile(half_path, half, &diagnostics).IsOk(),
	      "EXR Float16 MultiChannel save");
	ImageData half_loaded;
	Check(state, LoadImageFile(half_path, half_loaded, &diagnostics).IsOk(),
	      "EXR Float16 MultiChannel load");
	Check(state, half_loaded.spec.channel_layout == ChannelLayout::MultiChannel &&
	      half_loaded.spec.channel_names == half.spec.channel_names,
	      "EXR Float16 MultiChannel names");
	Check(state, SameBytes(half.buffer, half_loaded.buffer),
	      "EXR Float16 native bit patterns");

	CheckRoundTrip(state, root, "EXR Gray", "gray.exr",
	               MakeImage(SampleType::Float32, ChannelLayout::Gray, 3, 2, -1, -2));
	CheckRoundTrip(state, root, "EXR GrayAlpha", "grayalpha.exr",
	               MakeImage(SampleType::Float16, ChannelLayout::GrayAlpha, 3, 2, 2, -1));

	CheckRoundTrip(state, root, "PNG UInt8 Gray", "u8_gray.png",
	               MakeImage(SampleType::UInt8, ChannelLayout::Gray, 3, 2));
	CheckRoundTrip(state, root, "PNG UInt8 GrayAlpha", "u8_ga.png",
	               MakeImage(SampleType::UInt8, ChannelLayout::GrayAlpha, 3, 2));
	CheckRoundTrip(state, root, "PNG UInt8 RGB", "u8_rgb.png",
	               MakeImage(SampleType::UInt8, ChannelLayout::RGB, 3, 2));
	CheckRoundTrip(state, root, "PNG UInt8 RGBA", "u8_rgba.png",
	               MakeImage(SampleType::UInt8, ChannelLayout::RGBA, 3, 2));
	CheckRoundTrip(state, root, "PNG UInt16 Gray", "u16_gray.png",
	               MakeImage(SampleType::UInt16, ChannelLayout::Gray, 3, 2));
	CheckRoundTrip(state, root, "PNG UInt16 GrayAlpha", "u16_ga.png",
	               MakeImage(SampleType::UInt16, ChannelLayout::GrayAlpha, 3, 2));
	CheckRoundTrip(state, root, "PNG UInt16 RGB", "u16_rgb.png",
	               MakeImage(SampleType::UInt16, ChannelLayout::RGB, 3, 2));
	CheckRoundTrip(state, root, "PNG UInt16 RGBA", "u16_rgba.png",
	               MakeImage(SampleType::UInt16, ChannelLayout::RGBA, 3, 2));

	ImageData preserved = exr_loaded;
	Result empty_load = LoadImageFile(String(), preserved, &diagnostics);
	Check(state, empty_load.code == ResultCode::InvalidArgument &&
	      preserved.spec.data_window == exr_loaded.spec.data_window &&
	      SameBytes(preserved.buffer, exr_loaded.buffer),
	      "failed load preserves prior output");
	Check(state, HasCode(diagnostics, "IMGIO_PATH"),
	      "failed load reports stable diagnostic code");

	ImageData png = MakeImage(SampleType::UInt8, ChannelLayout::RGBA, 4, 3);
	String replacement = AppendFileName(root, "replacement.png");
	ImageData old = MakeImage(SampleType::UInt8, ChannelLayout::Gray, 2, 2);
	Check(state, SaveImageFile(replacement, old, &diagnostics).IsOk(),
	      "replacement seed save");
	Check(state, SaveImageFile(replacement, png, &diagnostics).IsOk(),
	      "existing regular target replacement");
	ImageData replaced;
	Check(state, LoadImageFile(replacement, replaced, &diagnostics).IsOk() &&
	      replaced.spec.channel_layout == ChannelLayout::RGBA &&
	      SameBytes(replaced.buffer, png.buffer),
	      "replacement publishes new contents");

	String directory_target = AppendFileName(root, "directory.png");
	std::filesystem::create_directories(directory_target.Begin(), error);
	Result directory_result = SaveImageFile(directory_target, png, &diagnostics);
	Check(state, directory_result.code == ResultCode::Unsupported &&
	      std::filesystem::is_directory(directory_target.Begin()),
	      "directory destination survives replacement failure");
	Check(state, HasCode(diagnostics, "IMGIO_REPLACE"),
	      "replacement failure reports stable code");
	Check(state, NoTransactionResidue(root_path),
	      "transaction failure leaves no temporary or backup files");

	ImageData nonzero_png = png;
	nonzero_png.spec.data_window = {1, 0, 4, 2};
	Check(state, SaveImageFile(AppendFileName(root, "bad-origin.png"),
	                          nonzero_png, &diagnostics).code == ResultCode::Unsupported,
	      "PNG non-zero origin rejected");
	ImageData bad_exr = png;
	Check(state, SaveImageFile(AppendFileName(root, "bad-sample.exr"),
	                          bad_exr, &diagnostics).code == ResultCode::Unsupported,
	      "EXR integer output rejected");
	Check(state, SaveImageFile(AppendFileName(root, "bad.jpg"),
	                          exr, &diagnostics).code == ResultCode::Unsupported,
	      "unsupported extension rejected");

	Diagnostics cleared;
	LoadImageFile(String(), preserved, &cleared);
	Check(state, cleared.HasErrors(), "diagnostics capture failure");
	ImageData clean_loaded;
	Check(state, LoadImageFile(replacement, clean_loaded, &cleared).IsOk() &&
	      !cleared.HasErrors(), "diagnostics clear before successful operation");

	std::filesystem::remove_all(root_path, error);
	Check(state, !std::filesystem::exists(root_path), "fixture cleanup");
	Cout() << "SUMMARY passed=" << state.passed
	       << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
