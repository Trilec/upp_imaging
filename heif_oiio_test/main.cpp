#include <Core/Core.h>
#include <OpenImageIO/OIIO.h>
#include <OpenImageIO/imageio.h>

#include <filesystem>
#include <fstream>
#include <string>

using namespace OIIO;
using namespace UppImaging;
using namespace Upp;

struct State { int passed = 0; int failed = 0; };

static void Check(State& state, bool condition, const char* label)
{
    Cout() << (condition ? "PASS " : "FAIL ") << label << '\n';
    (condition ? state.passed : state.failed)++;
}

CONSOLE_APP_MAIN
{
    State state;
    InitializeOpenImageIO();

    const std::string inputs = get_string_attribute("input_format_list");
    const std::string outputs = get_string_attribute("output_format_list");
    const std::string extensions = get_string_attribute("extension_list");
    Check(state, inputs.find("heif") != std::string::npos,
          "HEIF-family input format registered");
    Check(state, outputs.find("heif") == std::string::npos,
          "HEIF-family output deliberately absent");
    Check(state, extensions.find("avif") != std::string::npos,
          "AVIF extension registered");
    Check(state, extensions.find("heic") != std::string::npos &&
                 extensions.find("heif") != std::string::npos,
          "HEIC and HEIF extensions registered");

    ImageInput::unique_ptr avif = ImageInput::create("probe.avif");
    Check(state, avif && std::string(avif->format_name()) == "heif",
          "AVIF resolves to HEIF input plugin");
    avif.reset();
    ImageInput::unique_ptr heic = ImageInput::create("probe.heic");
    Check(state, heic && std::string(heic->format_name()) == "heif",
          "HEIC resolves to HEIF input plugin");
    heic.reset();

    Check(state, !ImageOutput::create("probe.avif"),
          "AVIF has no output plugin");
    OIIO::geterror();
    Check(state, !ImageOutput::create("probe.heic"),
          "HEIC has no output plugin");
    OIIO::geterror();

    const std::filesystem::path root = std::filesystem::temp_directory_path()
                                     / "opencode" / "heif_oiio_test";
    std::error_code error;
    std::filesystem::remove_all(root, error);
    std::filesystem::create_directories(root, error);
    const std::filesystem::path invalid = root / "invalid.avif";
    {
        std::ofstream stream(invalid, std::ios::binary);
        for(int i = 0; i < 128; ++i) {
            unsigned char value = (unsigned char)((i * 59 + 31) & 0xff);
            stream.write((const char*)&value, 1);
        }
    }
    ImageInput::unique_ptr rejected = ImageInput::open(invalid.string());
    Check(state, !rejected, "malformed AVIF is rejected");
    Check(state, !OIIO::geterror().empty(), "malformed AVIF reports an error");

    std::filesystem::remove_all(root, error);
    Check(state, !std::filesystem::exists(root), "fixture cleanup");

    Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
