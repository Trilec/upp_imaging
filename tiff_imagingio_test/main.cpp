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

static int Channels(ChannelLayout layout)
{
    if(layout == ChannelLayout::Gray) return 1;
    if(layout == ChannelLayout::GrayAlpha) return 2;
    if(layout == ChannelLayout::RGB) return 3;
    return 4;
}

static ImageData MakeImage(ChannelLayout layout, SampleType type,
                           int left = 0, int top = 0)
{
    ImageData image;
    image.spec.data_window = { left, top, left + 1, top + 1 };
    image.spec.depth = 1;
    image.spec.channels = Channels(layout);
    image.spec.channel_layout = layout;
    image.spec.sample_type = type;
    image.spec.alpha_channel = layout == ChannelLayout::GrayAlpha ? 1 :
                               layout == ChannelLayout::RGBA ? 3 : -1;
    if(layout == ChannelLayout::Gray)
        image.spec.channel_names << "Y";
    else if(layout == ChannelLayout::GrayAlpha)
        image.spec.channel_names << "Y" << "A";
    else if(layout == ChannelLayout::RGB)
        image.spec.channel_names << "R" << "G" << "B";
    else
        image.spec.channel_names << "R" << "G" << "B" << "A";
    image.buffer.Allocate(image.spec);

    if(type == SampleType::UInt8) {
        for(int i = 0; i < image.buffer.GetByteCount(); ++i)
            image.buffer.Begin()[i] = (byte)((i * 37 + 11) & 0xff);
        if(layout == ChannelLayout::RGBA) {
            const byte pixels[] = { 91,37,211,0, 255,12,3,64,
                                    17,99,201,128, 4,5,6,255 };
            memcpy(image.buffer.Begin(), pixels, sizeof(pixels));
        }
    }
    else if(type == SampleType::UInt16) {
        uint16* p = (uint16*)image.buffer.Begin();
        int n = image.buffer.GetByteCount() / 2;
        for(int i = 0; i < n; ++i)
            p[i] = (uint16)((i * 4093 + 17) & 0xffff);
    }
    else if(type == SampleType::Float32) {
        float* p = (float*)image.buffer.Begin();
        int n = image.buffer.GetByteCount() / 4;
        for(int i = 0; i < n; ++i)
            p[i] = (float)i * 0.25f - 0.5f;
    }
    return image;
}

static bool SamePixels(const ImageData& a, const ImageData& b)
{
    return a.buffer.GetByteCount() == b.buffer.GetByteCount() &&
           (!a.buffer.GetByteCount() ||
            memcmp(a.buffer.Begin(), b.buffer.Begin(), a.buffer.GetByteCount()) == 0);
}

static bool NoResidue(const std::filesystem::path& root)
{
    std::error_code error;
    for(const auto& entry : std::filesystem::directory_iterator(root, error)) {
        std::string name = entry.path().filename().string();
        if(name.find(".imagingio-") != std::string::npos ||
           name.find(".imagingio-backup-") != std::string::npos)
            return false;
    }
    return !error;
}

static void Roundtrip(const String& root, const char* name, ImageData source,
                      State& state, const char* label)
{
    Diagnostics diagnostics;
    uint64 source_hash = Hash(source.buffer);
    String path = AppendFileName(root, name);
    Result saved = SaveImageFile(path, source, &diagnostics);
    Check(state, saved.IsOk(), (String(label) + " save").Begin());
    Check(state, Hash(source.buffer) == source_hash,
          (String(label) + " source unchanged").Begin());

    ImageData loaded;
    Result read = LoadImageFile(path, loaded, &diagnostics);
    Check(state, read.IsOk(), (String(label) + " load").Begin());
    bool structure = read.IsOk() &&
                     loaded.spec.sample_type == source.spec.sample_type &&
                     loaded.spec.channel_layout == source.spec.channel_layout &&
                     loaded.spec.alpha_channel == source.spec.alpha_channel &&
                     loaded.spec.data_window == source.spec.data_window;
    Check(state, structure, (String(label) + " structure").Begin());
    Check(state, read.IsOk() && SamePixels(source, loaded),
          (String(label) + " exact pixels").Begin());
}

CONSOLE_APP_MAIN
{
    State state;
    String root = AppendFileName(GetTempPath(), "tiff_imagingio_contract");
    std::filesystem::path root_path(root.Begin());
    std::error_code error;
    std::filesystem::remove_all(root_path, error);
    std::filesystem::create_directories(root_path, error);
    Check(state, !error, "fixture directory creation");

    Roundtrip(root, "rgb8.tif", MakeImage(ChannelLayout::RGB, SampleType::UInt8),
              state, "TIFF UInt8 RGB");
    Roundtrip(root, "rgba8.tiff", MakeImage(ChannelLayout::RGBA, SampleType::UInt8),
              state, "TIFF UInt8 RGBA");
    Roundtrip(root, "grayalpha16.tif",
              MakeImage(ChannelLayout::GrayAlpha, SampleType::UInt16),
              state, "TIFF UInt16 GrayAlpha");
    Roundtrip(root, "gray32.tif", MakeImage(ChannelLayout::Gray, SampleType::Float32),
              state, "TIFF Float32 Gray");

    Diagnostics diagnostics;
    ImageData half = MakeImage(ChannelLayout::RGB, SampleType::Float16);
    String half_path = AppendFileName(root, "half.tif");
    Result half_save = SaveImageFile(half_path, half, &diagnostics);
    Check(state, half_save.code == ResultCode::Unsupported &&
                 HasCode(diagnostics, "IMGIO_SAMPLE"),
          "TIFF Float16 save fails closed");
    Check(state, !std::filesystem::exists(half_path.Begin()),
          "TIFF Float16 refusal creates no file");

    ImageData nonzero = MakeImage(ChannelLayout::RGB, SampleType::UInt8, 3, 4);
    String nonzero_path = AppendFileName(root, "origin.tif");
    Result origin_save = SaveImageFile(nonzero_path, nonzero, &diagnostics);
    Check(state, origin_save.code == ResultCode::Unsupported &&
                 HasCode(diagnostics, "IMGIO_SPEC"),
          "initial TIFF non-zero origin fails closed");
    Check(state, !std::filesystem::exists(nonzero_path.Begin()),
          "TIFF origin refusal creates no file");

    String invalid = AppendFileName(root, "invalid.tif");
    {
        std::ofstream stream(invalid.Begin(), std::ios::binary);
        for(int i = 0; i < 128; ++i) {
            byte value = (byte)((i * 71 + 23) & 0xff);
            stream.write((const char*)&value, 1);
        }
    }
    ImageData preserved = MakeImage(ChannelLayout::RGBA, SampleType::UInt8);
    uint64 before = Hash(preserved.buffer);
    Result invalid_load = LoadImageFile(invalid, preserved, &diagnostics);
    Check(state, invalid_load.code == ResultCode::IOError &&
                 HasCode(diagnostics, "IMGIO_OPEN"),
          "malformed TIFF fails with stable open diagnostic");
    Check(state, Hash(preserved.buffer) == before &&
                 preserved.spec.channel_layout == ChannelLayout::RGBA,
          "failed TIFF load preserves prior output");

    Check(state, NoResidue(root_path), "TIFF operations leave no transaction residue");
    std::filesystem::remove_all(root_path, error);
    Check(state, !std::filesystem::exists(root_path), "fixture cleanup");

    Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
