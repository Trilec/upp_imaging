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

struct State {
    int passed = 0;
    int failed = 0;
};

static void Check(State& state, bool condition, const char* label)
{
    std::printf("%s %s\n", condition ? "PASS" : "FAIL", label);
    condition ? ++state.passed : ++state.failed;
}

static std::vector<unsigned char> MakePixels(int width, int height, int channels)
{
    std::vector<unsigned char> pixels(size_t(width) * height * channels);
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            unsigned char* p = pixels.data() + (size_t(y) * width + x) * channels;
            p[0] = static_cast<unsigned char>((x * 31 + y * 7) & 255);
            p[1] = static_cast<unsigned char>((x * 11 + y * 29 + 17) & 255);
            p[2] = static_cast<unsigned char>((x * 3 + y * 47 + 91) & 255);
            if(channels == 4)
                p[3] = static_cast<unsigned char>((x * 19 + y * 13 + 37) & 255);
        }
    }
    return pixels;
}

static bool Roundtrip(const std::filesystem::path& path, int channels,
                      std::string& error)
{
    constexpr int width = 16;
    constexpr int height = 12;
    std::vector<unsigned char> source_pixels = MakePixels(width, height, channels);
    ImageSpec spec(width, height, channels, TypeDesc::UINT8);
    if(channels == 4)
        spec.alpha_channel = 3;
    spec.attribute("compression", "jpegxl:100");

    ImageBuf source(spec, source_pixels.data());
    if(!SaveImage(path.string().c_str(), source, &error))
        return false;

    ImageBuf reopened;
    if(!LoadImage(path.string().c_str(), reopened, &error))
        return false;
    if(reopened.spec().width != width || reopened.spec().height != height ||
       reopened.spec().nchannels != channels || reopened.spec().format != TypeDesc::UINT8)
        return false;

    std::vector<unsigned char> decoded(source_pixels.size());
    if(!reopened.get_pixels(reopened.roi(), TypeDesc::UINT8, decoded.data(),
                            channels * sizeof(unsigned char),
                            width * channels * sizeof(unsigned char), AutoStride)) {
        error = reopened.geterror();
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
    Check(state, inputs.find("jpegxl") != std::string::npos,
          "JPEG XL input format registered");
    Check(state, outputs.find("jpegxl") != std::string::npos,
          "JPEG XL output format registered");
    Check(state, extensions.find("jxl") != std::string::npos,
          "JXL extension registered");

    const std::filesystem::path root = std::filesystem::temp_directory_path()
                                     / "opencode" / "jpegxl_oiio_test";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    std::string error;
    const std::filesystem::path rgb = root / "rgb8.jxl";
    bool rgb_ok = Roundtrip(rgb, 3, error);
    if(!rgb_ok)
        std::printf("RGB error: %s\n", error.c_str());
    Check(state, rgb_ok, "lossless RGB8 roundtrip");
    Check(state, std::filesystem::exists(rgb) && std::filesystem::file_size(rgb) > 0,
          "RGB8 output is a non-empty file");

    error.clear();
    const std::filesystem::path rgba = root / "rgba8.jxl";
    bool rgba_ok = Roundtrip(rgba, 4, error);
    if(!rgba_ok)
        std::printf("RGBA error: %s\n", error.c_str());
    Check(state, rgba_ok, "lossless RGBA8 roundtrip preserves alpha");
    Check(state, std::filesystem::exists(rgba) && std::filesystem::file_size(rgba) > 0,
          "RGBA8 output is a non-empty file");

    const std::filesystem::path invalid = root / "invalid.jxl";
    {
        std::ofstream stream(invalid, std::ios::binary);
        std::vector<unsigned char> garbage(256, 0);
        stream.write(reinterpret_cast<const char*>(garbage.data()),
                     static_cast<std::streamsize>(garbage.size()));
    }
    ImageBuf rejected;
    error.clear();
    Check(state, !LoadImage(invalid.string().c_str(), rejected, &error),
          "malformed JPEG XL is rejected");
    Check(state, !error.empty(), "malformed JPEG XL reports an error");

    std::filesystem::remove_all(root);
    Check(state, !std::filesystem::exists(root), "fixture cleanup");

    std::printf("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
