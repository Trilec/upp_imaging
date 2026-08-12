#include <Core/Core.h>
#include <OpenImageIO/OIIO.h>
#include <OpenImageIO/imageio.h>

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
    Cout() << (condition ? "PASS " : "FAIL ") << label << '\n';
    condition ? ++state.passed : ++state.failed;
}

static bool WriteLossless(const std::string& path, const ImageSpec& source_spec,
                          const uint8_t* pixels)
{
    ImageSpec spec = source_spec;
    spec.attribute("compression", "lossless:70");
    spec.attribute("webp:method", 6);
    if(spec.alpha_channel != -1)
        spec.attribute("oiio:UnassociatedAlpha", 1);
    ImageOutput::unique_ptr output = ImageOutput::create(path);
    return output && output->open(path, spec) &&
           output->write_image(TypeUInt8, pixels) && output->close();
}

static bool ReadExact(const std::string& path, const uint8_t* expected,
                      size_t bytes, int channels, bool alpha)
{
    ImageSpec config;
    if(alpha)
        config.attribute("oiio:UnassociatedAlpha", 1);
    ImageInput::unique_ptr input = ImageInput::open(path, &config);
    if(!input)
        return false;
    ImageSpec spec = input->spec();
    if(spec.width != 2 || spec.height != 2 || spec.nchannels != channels ||
       spec.format != TypeUInt8 || (alpha && spec.alpha_channel != 3))
        return false;
    std::vector<uint8_t> decoded(bytes);
    bool ok = input->read_image(TypeUInt8, decoded.data()) && input->close() &&
              memcmp(decoded.data(), expected, bytes) == 0;
    return ok;
}

CONSOLE_APP_MAIN
{
    State state;
    InitializeOpenImageIO();

    const std::filesystem::path root = std::filesystem::temp_directory_path()
                                     / "opencode" / "webp_oiio_test";
    std::error_code error;
    std::filesystem::remove_all(root, error);
    std::filesystem::create_directories(root, error);
    Check(state, !error, "fixture directory creation");

    const std::string inputs = get_string_attribute("input_format_list");
    const std::string outputs = get_string_attribute("output_format_list");
    const std::string extensions = get_string_attribute("extension_list");
    Check(state, inputs.find("webp") != std::string::npos,
          "WebP input registered");
    Check(state, outputs.find("webp") != std::string::npos,
          "WebP output registered");
    Check(state, extensions.find("webp") != std::string::npos,
          "WebP extension registered");

    const uint8_t rgb[] = {
        0, 17, 255, 63, 127, 191,
        255, 1, 2, 11, 22, 33
    };
    ImageSpec rgb_spec(2, 2, 3, TypeUInt8);
    rgb_spec.channelnames = { "R", "G", "B" };
    const std::string rgb_path = (root / "rgb.webp").string();
    Check(state, WriteLossless(rgb_path, rgb_spec, rgb),
          "lossless RGB write");
    Check(state, std::filesystem::exists(rgb_path) &&
                 std::filesystem::file_size(rgb_path) > 0,
          "lossless RGB file is non-empty");
    Check(state, ReadExact(rgb_path, rgb, sizeof(rgb), 3, false),
          "lossless RGB exact readback");

    const uint8_t rgba[] = {
        91, 37, 211, 0,    255, 12, 3, 64,
        17, 99, 201, 128,  4, 5, 6, 255
    };
    ImageSpec rgba_spec(2, 2, 4, TypeUInt8);
    rgba_spec.channelnames = { "R", "G", "B", "A" };
    rgba_spec.alpha_channel = 3;
    rgba_spec.attribute("oiio:UnassociatedAlpha", 1);
    const std::string rgba_path = (root / "rgba.webp").string();
    Check(state, WriteLossless(rgba_path, rgba_spec, rgba),
          "exact lossless RGBA write");
    Check(state, std::filesystem::exists(rgba_path) &&
                 std::filesystem::file_size(rgba_path) > 0,
          "lossless RGBA file is non-empty");
    Check(state, ReadExact(rgba_path, rgba, sizeof(rgba), 4, true),
          "lossless RGBA preserves hidden RGB and alpha");

    const std::string invalid = (root / "invalid.webp").string();
    {
        std::ofstream stream(invalid, std::ios::binary);
        for(int i = 0; i < 96; ++i) {
            uint8_t value = (uint8_t)((i * 47 + 13) & 0xff);
            stream.write((const char*)&value, 1);
        }
    }
    ImageInput::unique_ptr rejected = ImageInput::open(invalid);
    Check(state, !rejected, "malformed WebP is rejected");
    std::string backend_error = OIIO::geterror();
    Check(state, !backend_error.empty(), "malformed WebP reports an error");

    std::filesystem::remove_all(root, error);
    Check(state, !std::filesystem::exists(root), "fixture cleanup");

    Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
