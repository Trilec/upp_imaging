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

static void Check(State& state, bool condition, const char* label)
{
	std::printf("%s %s\n", condition ? "PASS" : "FAIL", label);
	(condition ? state.passed : state.failed)++;
}

static std::vector<float> MakePixels(int width, int height)
{
	static const float triples[][3] = {
		{0.25f, 0.5f, 1.0f},
		{1.0f, 0.5f, 0.25f},
		{2.0f, 4.0f, 8.0f},
		{8.0f, 4.0f, 2.0f},
		{0.0f, 0.0f, 0.0f},
		{0.125f, 0.25f, 0.5f},
		{16.0f, 8.0f, 4.0f},
		{0.5f, 0.25f, 0.125f}
	};
	std::vector<float> pixels(size_t(width) * height * 3);
	for(int i = 0; i < width * height; ++i)
		for(int c = 0; c < 3; ++c)
			pixels[size_t(i) * 3 + c] = triples[i % 8][c];
	return pixels;
}

static bool Roundtrip(const std::filesystem::path& path, std::string& error)
{
	constexpr int width = 16;
	constexpr int height = 3;
	std::vector<float> source_pixels = MakePixels(width, height);
	ImageSpec spec(width, height, 3, TypeDesc::FLOAT);
	ImageBuf source(spec, source_pixels.data());
	if(!SaveImage(path.string().c_str(), source, &error))
		return false;

	ImageBuf loaded;
	if(!LoadImage(path.string().c_str(), loaded, &error))
		return false;
	if(loaded.spec().width != width || loaded.spec().height != height ||
	   loaded.spec().nchannels != 3 || loaded.spec().format != TypeDesc::FLOAT)
		return false;

	std::vector<float> decoded(source_pixels.size());
	if(!loaded.get_pixels(loaded.roi(), TypeDesc::FLOAT, decoded.data(),
	                      3 * sizeof(float), width * 3 * sizeof(float), AutoStride)) {
		error = loaded.geterror();
		return false;
	}
	return decoded == source_pixels;
}

CONSOLE_APP_MAIN
{
	State state;
	InitializeOpenImageIO();

	const std::string inputs = get_string_attribute("input_format_list");
	const std::string outputs = get_string_attribute("output_format_list");
	const std::string extensions = get_string_attribute("extension_list");
	Check(state, inputs.find("hdr") != std::string::npos,
	      "HDR input format registered");
	Check(state, outputs.find("hdr") != std::string::npos,
	      "HDR output format registered");
	Check(state, extensions.find("hdr") != std::string::npos,
	      "HDR extension registered");
	Check(state, extensions.find("rgbe") != std::string::npos,
	      "RGBE extension registered");

	const std::filesystem::path root = std::filesystem::temp_directory_path()
	                                 / "opencode" / "hdr_oiio_test";
	std::filesystem::remove_all(root);
	std::filesystem::create_directories(root);

	std::string error;
	const std::filesystem::path hdr = root / "roundtrip.hdr";
	bool hdr_ok = Roundtrip(hdr, error);
	if(!hdr_ok)
		std::printf("HDR error: %s\n", error.c_str());
	Check(state, hdr_ok, "HDR representable Float32 RGB roundtrip");
	Check(state, std::filesystem::exists(hdr) && std::filesystem::file_size(hdr) > 0,
	      "HDR output is non-empty");

	error.clear();
	const std::filesystem::path rgbe = root / "roundtrip.rgbe";
	bool rgbe_ok = Roundtrip(rgbe, error);
	if(!rgbe_ok)
		std::printf("RGBE error: %s\n", error.c_str());
	Check(state, rgbe_ok, "RGBE representable Float32 RGB roundtrip");
	Check(state, std::filesystem::exists(rgbe) && std::filesystem::file_size(rgbe) > 0,
	      "RGBE output is non-empty");

	if(hdr_ok) {
		ImageBuf loaded;
		error.clear();
		bool loaded_ok = LoadImage(hdr.string().c_str(), loaded, &error);
		Check(state, loaded_ok && loaded.spec().nchannels == 3 &&
		             loaded.spec().format == TypeDesc::FLOAT,
		      "HDR native structure is Float32 RGB");
	}
	else {
		Check(state, false, "HDR native structure is Float32 RGB");
	}

	const std::filesystem::path invalid = root / "invalid.hdr";
	{
		std::ofstream stream(invalid, std::ios::binary);
		stream << "not radiance hdr\n";
	}
	ImageBuf rejected;
	error.clear();
	Check(state, !LoadImage(invalid.string().c_str(), rejected, &error),
	      "malformed HDR is rejected");
	Check(state, !error.empty(), "malformed HDR reports an error");

	std::filesystem::remove_all(root);
	Check(state, !std::filesystem::exists(root), "fixture cleanup");

	std::printf("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
