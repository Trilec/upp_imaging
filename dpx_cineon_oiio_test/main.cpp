#include <Core/Core.h>
#include <OpenImageIO/OIIO.h>
#include <OpenImageIO/imageio.h>

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace OIIO;
using namespace UppImaging;
using namespace Upp;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const String& label)
{
	std::printf("%s %s\n", condition ? "PASS" : "FAIL", label.Begin());
	(condition ? state.passed : state.failed)++;
}

static std::vector<unsigned char> MakePixels(TypeDesc type, int width, int height)
{
	const int channels = 3;
	const size_t sample_bytes = type == TypeDesc::UINT16 ? 2 : 1;
	std::vector<unsigned char> pixels(size_t(width) * height * channels * sample_bytes);
	const int count = width * height * channels;
	if(type == TypeDesc::UINT16) {
		for(int i = 0; i < count; ++i) {
			uint16 value = (uint16)((i * 1009 + 97) & 0xffff);
			memcpy(pixels.data() + size_t(i) * 2, &value, sizeof(value));
		}
	}
	else {
		for(int i = 0; i < count; ++i)
			pixels[i] = (unsigned char)((i * 43 + 17) & 0xff);
	}
	return pixels;
}

static void CheckRoundtrip(State& state, const std::filesystem::path& path,
                           const char* label, TypeDesc type, int x, int y)
{
	constexpr int width = 11;
	constexpr int height = 7;
	std::vector<unsigned char> source_pixels = MakePixels(type, width, height);
	ImageSpec spec(width, height, 3, type);
	spec.x = x;
	spec.y = y;
	ImageBuf source(spec, source_pixels.data());
	std::string error;

	bool saved = SaveImage(path.string().c_str(), source, &error);
	if(!saved)
		std::printf("%s save error: %s\n", label, error.c_str());
	Check(state, saved, String(label) + " save");

	ImageBuf loaded;
	error.clear();
	bool read = saved && LoadImage(path.string().c_str(), loaded, &error);
	if(!read)
		std::printf("%s load error: %s\n", label, error.c_str());
	Check(state, read, String(label) + " load");

	bool structure = read && loaded.spec().width == width &&
	                 loaded.spec().height == height &&
	                 loaded.spec().nchannels == 3 &&
	                 loaded.spec().format == type &&
	                 loaded.spec().x == x && loaded.spec().y == y;
	Check(state, structure, String(label) + " exact structure/origin");

	std::vector<unsigned char> decoded(source_pixels.size());
	bool pixels = false;
	if(read) {
		pixels = loaded.get_pixels(loaded.roi(), type, decoded.data(), AutoStride,
		                          AutoStride, AutoStride) && decoded == source_pixels;
	}
	Check(state, pixels, String(label) + " exact pixels");
}

CONSOLE_APP_MAIN
{
	State state;
	InitializeOpenImageIO();

	const std::string inputs = get_string_attribute("input_format_list");
	const std::string outputs = get_string_attribute("output_format_list");
	const std::string extensions = get_string_attribute("extension_list");
	Check(state, inputs.find("dpx") != std::string::npos,
	      "DPX input format registered");
	Check(state, outputs.find("dpx") != std::string::npos,
	      "DPX output format registered");
	Check(state, inputs.find("cineon") != std::string::npos,
	      "Cineon input format registered");
	Check(state, extensions.find("dpx") != std::string::npos,
	      "DPX extension registered");
	Check(state, extensions.find("cin") != std::string::npos,
	      "Cineon extension registered");
	Check(state, outputs.find("cineon") == std::string::npos,
	      "Cineon remains input-only");

	const std::filesystem::path root = std::filesystem::temp_directory_path()
	                                 / "opencode" / "dpx_cineon_oiio_test";
	std::filesystem::remove_all(root);
	std::filesystem::create_directories(root);

	CheckRoundtrip(state, root / "rgb8.dpx", "DPX UInt8 RGB",
	               TypeDesc::UINT8, 0, 0);
	CheckRoundtrip(state, root / "rgb16_origin.dpx", "DPX UInt16 RGB",
	               TypeDesc::UINT16, 7, 11);

	const std::filesystem::path invalid_dpx = root / "invalid.dpx";
	{
		std::ofstream stream(invalid_dpx, std::ios::binary);
		stream << "not dpx";
	}
	ImageBuf rejected;
	std::string error;
	Check(state, !LoadImage(invalid_dpx.string().c_str(), rejected, &error),
	      "malformed DPX is rejected");
	Check(state, !error.empty(), "malformed DPX reports an error");

	const std::filesystem::path invalid_cin = root / "invalid.cin";
	{
		std::ofstream stream(invalid_cin, std::ios::binary);
		stream << "not cineon";
	}
	rejected.reset();
	error.clear();
	Check(state, !LoadImage(invalid_cin.string().c_str(), rejected, &error),
	      "malformed Cineon is rejected");
	Check(state, !error.empty(), "malformed Cineon reports an error");

	std::filesystem::remove_all(root);
	Check(state, !std::filesystem::exists(root), "fixture cleanup");

	std::printf("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
