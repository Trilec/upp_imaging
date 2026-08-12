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

struct State { int passed = 0; int failed = 0; };

static void Check(State& state, bool condition, const char* label)
{
    Cout() << (condition ? "PASS " : "FAIL ") << label << '\n';
    (condition ? state.passed : state.failed)++;
}

template <class T>
static bool WriteReadExact(const std::string& path, int channels, TypeDesc type,
                           const T* pixels, size_t count, bool alpha = false)
{
    ImageSpec spec(2, 2, channels, type);
    if(channels == 1)
        spec.channelnames = { "Y" };
    else if(channels == 3)
        spec.channelnames = { "R", "G", "B" };
    else
        spec.channelnames = { "R", "G", "B", "A" };
    if(alpha) {
        spec.alpha_channel = channels - 1;
        spec.attribute("oiio:UnassociatedAlpha", 1);
    }
    spec.attribute("compression", "zip");

    ImageOutput::unique_ptr output = ImageOutput::create(path);
    if(!output || !output->open(path, spec) ||
       !output->write_image(type, pixels) || !output->close())
        return false;

    ImageSpec config;
    if(alpha)
        config.attribute("oiio:UnassociatedAlpha", 1);
    ImageInput::unique_ptr input = ImageInput::open(path, &config);
    if(!input)
        return false;
    const ImageSpec got = input->spec();
    if(got.width != 2 || got.height != 2 || got.nchannels != channels ||
       got.format != type || (alpha && got.alpha_channel != channels - 1))
        return false;

    std::vector<T> decoded(count);
    bool ok = input->read_image(0, 0, 0, -1, type, decoded.data()) &&
              input->close() &&
              memcmp(decoded.data(), pixels, count * sizeof(T)) == 0;
    return ok;
}

CONSOLE_APP_MAIN
{
    State state;
    InitializeOpenImageIO();

    const std::filesystem::path root = std::filesystem::temp_directory_path()
                                     / "opencode" / "tiff_oiio_test";
    std::error_code error;
    std::filesystem::remove_all(root, error);
    std::filesystem::create_directories(root, error);
    Check(state, !error, "fixture directory creation");

    const std::string inputs = get_string_attribute("input_format_list");
    const std::string outputs = get_string_attribute("output_format_list");
    const std::string extensions = get_string_attribute("extension_list");
    Check(state, inputs.find("tiff") != std::string::npos,
          "TIFF input registered");
    Check(state, outputs.find("tiff") != std::string::npos,
          "TIFF output registered");
    Check(state, extensions.find("tif") != std::string::npos &&
                 extensions.find("tiff") != std::string::npos,
          "TIFF extensions registered");

    ImageInput::unique_ptr probe_in = ImageInput::create("probe.tif");
    Check(state, probe_in && std::string(probe_in->format_name()) == "tiff",
          ".tif resolves to TIFF input");
    ImageOutput::unique_ptr probe_out = ImageOutput::create("probe.tiff");
    Check(state, probe_out && std::string(probe_out->format_name()) == "tiff",
          ".tiff resolves to TIFF output");

    const uint8_t rgb8[] = { 0,17,255, 63,127,191, 255,1,2, 11,22,33 };
    Check(state, WriteReadExact((root / "rgb8.tif").string(), 3, TypeUInt8,
                                rgb8, sizeof(rgb8)),
          "UInt8 RGB ZIP roundtrip is exact");

    const uint8_t rgba8[] = { 91,37,211,0, 255,12,3,64,
                              17,99,201,128, 4,5,6,255 };
    Check(state, WriteReadExact((root / "rgba8.tif").string(), 4, TypeUInt8,
                                rgba8, sizeof(rgba8), true),
          "UInt8 RGBA preserves straight alpha and hidden RGB");

    const uint16_t rgb16[] = { 0,1,65535, 32768,4096,60000,
                               123,456,789, 65534,2222,3333 };
    Check(state, WriteReadExact((root / "rgb16.tiff").string(), 3, TypeUInt16,
                                rgb16, sizeof(rgb16) / sizeof(rgb16[0])),
          "UInt16 RGB ZIP roundtrip is exact");

    const float gray32[] = { 0.0f, 0.25f, 1.0f, 4.5f };
    Check(state, WriteReadExact((root / "gray32.tif").string(), 1, TypeFloat,
                                gray32, sizeof(gray32) / sizeof(gray32[0])),
          "Float32 Gray ZIP roundtrip is exact");

    const std::string invalid = (root / "invalid.tif").string();
    {
        std::ofstream stream(invalid, std::ios::binary);
        for(int i = 0; i < 128; ++i) {
            uint8_t value = (uint8_t)((i * 67 + 19) & 0xff);
            stream.write((const char*)&value, 1);
        }
    }
    ImageInput::unique_ptr rejected = ImageInput::open(invalid);
    Check(state, !rejected, "malformed TIFF is rejected");
    Check(state, !OIIO::geterror().empty(), "malformed TIFF reports an error");

    std::filesystem::remove_all(root, error);
    Check(state, !std::filesystem::exists(root), "fixture cleanup");

    Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
