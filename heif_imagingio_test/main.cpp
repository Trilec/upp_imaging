#include <ImagingIO/ImagingIO.h>

#include <cstring>
#include <filesystem>
#include <fstream>

using namespace Upp;
using namespace Upp::Imaging;

struct State { int passed = 0; int failed = 0; };

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

static ImageData MakeImage()
{
    ImageData image;
    image.spec.data_window = {0, 0, 1, 1};
    image.spec.depth = 1;
    image.spec.channels = 4;
    image.spec.channel_layout = ChannelLayout::RGBA;
    image.spec.sample_type = SampleType::UInt8;
    image.spec.alpha_channel = 3;
    image.spec.channel_names << "R" << "G" << "B" << "A";
    image.buffer.Allocate(image.spec);
    for(int i = 0; i < image.buffer.GetByteCount(); ++i)
        image.buffer.Begin()[i] = (byte)((i * 37 + 9) & 0xff);
    return image;
}

CONSOLE_APP_MAIN
{
    State state;
    String root = AppendFileName(GetTempPath(), "heif_imagingio_contract");
    std::filesystem::path root_path(root.Begin());
    std::error_code error;
    std::filesystem::remove_all(root_path, error);
    std::filesystem::create_directories(root_path, error);
    Check(state, !error, "fixture directory creation");

    Diagnostics diagnostics;
    ImageData source = MakeImage();

    String avif = AppendFileName(root, "output.avif");
    Result avif_save = SaveImageFile(avif, source, &diagnostics);
    Check(state, avif_save.code == ResultCode::Unsupported,
          "AVIF output is explicitly unsupported");
    Check(state, HasCode(diagnostics, "IMGIO_FORMAT"),
          "AVIF output reports IMGIO_FORMAT");
    Check(state, !std::filesystem::exists(avif.Begin()),
          "AVIF refusal creates no file");

    String heic = AppendFileName(root, "output.heic");
    Result heic_save = SaveImageFile(heic, source, &diagnostics);
    Check(state, heic_save.code == ResultCode::Unsupported &&
                 HasCode(diagnostics, "IMGIO_FORMAT") &&
                 !std::filesystem::exists(heic.Begin()),
          "HEIC output is input-only and side-effect free");

    String heif = AppendFileName(root, "output.heif");
    Result heif_save = SaveImageFile(heif, source, &diagnostics);
    Check(state, heif_save.code == ResultCode::Unsupported &&
                 HasCode(diagnostics, "IMGIO_FORMAT") &&
                 !std::filesystem::exists(heif.Begin()),
          "HEIF output is input-only and side-effect free");

    String invalid = AppendFileName(root, "invalid.avif");
    {
        std::ofstream stream(invalid.Begin(), std::ios::binary);
        for(int i = 0; i < 128; ++i) {
            unsigned char value = (unsigned char)((i * 61 + 29) & 0xff);
            stream.write((const char*)&value, 1);
        }
    }
    ImageData preserved = source;
    uint64 before = Hash(preserved.buffer);
    Result invalid_load = LoadImageFile(invalid, preserved, &diagnostics);
    Check(state, invalid_load.code == ResultCode::IOError,
          "malformed AVIF reaches HEIF reader and fails as I/O");
    Check(state, HasCode(diagnostics, "IMGIO_OPEN"),
          "malformed AVIF reports IMGIO_OPEN");
    Check(state, Hash(preserved.buffer) == before &&
                 preserved.spec.channel_layout == ChannelLayout::RGBA,
          "failed HEIF-family load preserves prior output");

    std::filesystem::remove_all(root_path, error);
    Check(state, !std::filesystem::exists(root_path), "fixture cleanup");

    Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
