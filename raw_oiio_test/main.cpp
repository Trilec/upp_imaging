#include <Core/Core.h>
#include <OpenImageIO/OIIO.h>
#include <OpenImageIO/imageio.h>

#include <cstdio>
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
    condition ? ++state.passed : ++state.failed;
}

CONSOLE_APP_MAIN
{
    State state;
    InitializeOpenImageIO();

    const std::string inputs = get_string_attribute("input_format_list");
    const std::string outputs = get_string_attribute("output_format_list");
    const std::string extensions = get_string_attribute("extension_list");
    Check(state, inputs.find("raw") != std::string::npos,
          "RAW input format registered");
    Check(state, outputs.find("raw") == std::string::npos,
          "RAW remains input-only");
    Check(state, extensions.find("dng") != std::string::npos,
          "DNG extension registered");

    ImageInput::unique_ptr raw = ImageInput::create("probe.dng");
    Check(state, raw && std::string(raw->format_name()) == "raw",
          "DNG resolves to RAW input plugin");
    raw.reset();

    ImageInput::unique_ptr hdr = ImageInput::create("probe.hdr");
    Check(state, hdr && std::string(hdr->format_name()) == "hdr",
          "HDR remains owned by Radiance plugin");
    hdr.reset();

    ImageOutput::unique_ptr raw_output = ImageOutput::create("probe.dng");
    Check(state, !raw_output,
          "DNG has no RAW output plugin");
    OIIO::geterror();

    const std::filesystem::path root = std::filesystem::temp_directory_path()
                                     / "opencode" / "raw_oiio_test";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);
    const std::filesystem::path invalid = root / "invalid.dng";
    {
        std::ofstream stream(invalid, std::ios::binary);
        std::vector<unsigned char> garbage(256);
        for(size_t i = 0; i < garbage.size(); ++i)
            garbage[i] = static_cast<unsigned char>((i * 53 + 19) & 0xff);
        stream.write(reinterpret_cast<const char*>(garbage.data()),
                     static_cast<std::streamsize>(garbage.size()));
    }

    ImageBuf rejected;
    std::string error;
    Check(state, !LoadImage(invalid.string().c_str(), rejected, &error),
          "malformed DNG is rejected");
    Check(state, !error.empty(),
          "malformed DNG reports an error");

    std::filesystem::remove_all(root);
    Check(state, !std::filesystem::exists(root),
          "fixture cleanup");

    std::printf("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
