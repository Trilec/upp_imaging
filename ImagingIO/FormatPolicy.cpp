#include "FormatPolicy.h"

namespace Upp {
namespace Imaging {
namespace IOFormatPolicy {

namespace {

static Result FailPolicy(ResultCode code, Diagnostics* diagnostics,
                         String message, const String& path,
                         const char* diagnostic)
{
    if(diagnostics)
        diagnostics->Error(message, diagnostic, path);
    return Result::Failure(code, message, path);
}

static bool IsHDR(const String& extension)
{
    return extension == ".hdr" || extension == ".rgbe";
}

static bool IsHEIF(const String& extension)
{
    return extension == ".avif" || extension == ".heic" ||
           extension == ".heif" || extension == ".heics" ||
           extension == ".hif";
}

static bool IsTIFF(const String& extension)
{
    return extension == ".tif" || extension == ".tiff";
}

static bool IsRaw(const String& extension)
{
    static const char* extensions[] = {
        ".bay", ".bmq", ".cr2", ".cr3", ".crw", ".cs1", ".dc2", ".dcr",
        ".dng", ".erf", ".fff", ".k25", ".kdc", ".mdc", ".mos", ".mrw",
        ".nef", ".orf", ".pef", ".pxn", ".raf", ".raw", ".rdc", ".sr2",
        ".srf", ".x3f", ".arw", ".3fr", ".cine", ".ia", ".kc2", ".mef",
        ".nrw", ".qtk", ".rw2", ".sti", ".rwl", ".srw", ".drf", ".dsc",
        ".ptx", ".cap", ".iiq", ".rwz"
    };
    for(const char* item : extensions)
        if(extension == item)
            return true;
    return false;
}

static bool IsCanonicalTIFFLayout(ChannelLayout layout)
{
    return layout == ChannelLayout::Gray || layout == ChannelLayout::GrayAlpha ||
           layout == ChannelLayout::RGB || layout == ChannelLayout::RGBA;
}

} // namespace

String Extension(const String& path)
{
    int slash = max(path.ReverseFind('/'), path.ReverseFind('\\'));
    int dot = path.ReverseFind('.');
    return dot > slash ? ToLower(path.Mid(dot)) : String();
}

bool IsSupportedExtension(const String& extension)
{
    return extension == ".exr" || extension == ".png" ||
           extension == ".jxl" || IsHDR(extension) ||
           extension == ".dpx" || extension == ".cin" ||
           IsRaw(extension) || extension == ".webp" ||
           IsHEIF(extension) || IsTIFF(extension);
}

bool RequiresZeroOrigin(const String& extension)
{
    return extension == ".png" || extension == ".jxl" ||
           IsHDR(extension) || IsRaw(extension) ||
           extension == ".webp" || IsHEIF(extension) || IsTIFF(extension);
}

const char* FormatName(const String& extension)
{
    if(extension == ".exr") return "EXR";
    if(extension == ".png") return "PNG";
    if(extension == ".jxl") return "JPEG XL";
    if(IsHDR(extension)) return "Radiance HDR/RGBE";
    if(extension == ".dpx") return "DPX";
    if(extension == ".cin") return "Cineon";
    if(IsRaw(extension)) return "camera RAW";
    if(extension == ".webp") return "WebP";
    if(IsHEIF(extension)) return "HEIF/AVIF";
    if(IsTIFF(extension)) return "TIFF";
    return "image";
}

void ConfigureRead(const String& extension, OIIO::ImageSpec& config)
{
    if(extension == ".png" || extension == ".webp" ||
       IsHEIF(extension) || IsTIFF(extension))
        config.attribute("oiio:UnassociatedAlpha", 1);
    else if(IsRaw(extension)) {
        config.attribute("raw:use_camera_wb", 1);
        config.attribute("raw:auto_bright", 0);
        config.attribute("raw:use_camera_matrix", 1);
        config.attribute("raw:ColorSpace", "srgb_rec709_scene");
    }
}

void ConfigureWriteTarget(const String& extension, const ImageData& image,
                          OIIO::ImageSpec& target)
{
    if((extension == ".png" || extension == ".webp" || IsTIFF(extension)) &&
       image.spec.alpha_channel != -1)
        target.attribute("oiio:UnassociatedAlpha", 1);
    if(extension == ".jxl")
        target.attribute("compression", "jpegxl:100");
    if(extension == ".webp") {
        target.attribute("compression", "lossless:70");
        target.attribute("webp:method", 6);
    }
    if(IsTIFF(extension))
        target.attribute("compression", "zip");
}

Result ValidateLoaded(const String& extension, const ImageSpec& spec,
                      Diagnostics* diagnostics, const String& path)
{
    if((extension == ".png" || extension == ".jxl") &&
       spec.channel_layout == ChannelLayout::MultiChannel)
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          String(FormatName(extension)) +
                          " arbitrary multichannel images are unsupported",
                          path, "IMGIO_CHANNELS");

    if(extension == ".jxl" && spec.channel_layout == ChannelLayout::GrayAlpha)
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          "JPEG XL GrayAlpha is deferred until the backend exposes its alpha semantics reliably",
                          path, "IMGIO_CHANNELS");

    if(IsHDR(extension)) {
        if(spec.sample_type != SampleType::Float32)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "Radiance HDR/RGBE input must decode as Float32",
                              path, "IMGIO_SAMPLE");
        if(spec.channel_layout != ChannelLayout::RGB)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "Radiance HDR/RGBE supports RGB images only",
                              path, "IMGIO_CHANNELS");
    }

    if(extension == ".dpx") {
        if(spec.sample_type != SampleType::UInt8 &&
           spec.sample_type != SampleType::UInt16)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial DPX slice supports UInt8 and UInt16 only",
                              path, "IMGIO_SAMPLE");
        if(spec.channel_layout != ChannelLayout::RGB)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial DPX slice supports RGB images only",
                              path, "IMGIO_CHANNELS");
    }

    if(extension == ".cin") {
        if(spec.sample_type != SampleType::UInt8 &&
           spec.sample_type != SampleType::UInt16)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial Cineon slice supports UInt8 and UInt16 input only",
                              path, "IMGIO_SAMPLE");
        if(spec.channel_layout != ChannelLayout::Gray &&
           spec.channel_layout != ChannelLayout::RGB)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial Cineon slice supports Gray and RGB input only",
                              path, "IMGIO_CHANNELS");
    }

    if(IsRaw(extension)) {
        if(spec.sample_type != SampleType::UInt16)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "camera RAW input must decode as UInt16",
                              path, "IMGIO_SAMPLE");
        if(spec.channel_layout != ChannelLayout::RGB)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial camera RAW slice supports processed RGB input only",
                              path, "IMGIO_CHANNELS");
    }

    if(extension == ".webp") {
        if(spec.sample_type != SampleType::UInt8)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "WebP input must decode as UInt8",
                              path, "IMGIO_SAMPLE");
        if(spec.channel_layout != ChannelLayout::RGB &&
           spec.channel_layout != ChannelLayout::RGBA)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial WebP slice supports RGB and RGBA only",
                              path, "IMGIO_CHANNELS");
    }

    if(IsHEIF(extension)) {
        if(spec.sample_type != SampleType::UInt8 &&
           spec.sample_type != SampleType::UInt16)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "HEIF/AVIF input supports UInt8 and UInt16 only",
                              path, "IMGIO_SAMPLE");
        if(spec.channel_layout != ChannelLayout::Gray &&
           spec.channel_layout != ChannelLayout::GrayAlpha &&
           spec.channel_layout != ChannelLayout::RGB &&
           spec.channel_layout != ChannelLayout::RGBA)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "HEIF/AVIF input supports Gray, GrayAlpha, RGB and RGBA only",
                              path, "IMGIO_CHANNELS");
    }

    if(IsTIFF(extension)) {
        if(spec.sample_type != SampleType::UInt8 &&
           spec.sample_type != SampleType::UInt16 &&
           spec.sample_type != SampleType::Float32)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial TIFF slice supports UInt8, UInt16 and Float32 only",
                              path, "IMGIO_SAMPLE");
        if(!IsCanonicalTIFFLayout(spec.channel_layout))
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial TIFF slice supports Gray, GrayAlpha, RGB and RGBA only",
                              path, "IMGIO_CHANNELS");
    }

    return Result::Success();
}

Result ValidateSave(const String& extension, const ImageData& image,
                    Diagnostics* diagnostics, const String& path)
{
    if(extension == ".cin")
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          "Cineon output is not supported by the bundled OpenImageIO backend",
                          path, "IMGIO_FORMAT");

    if(IsRaw(extension))
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          "camera RAW formats are input-only",
                          path, "IMGIO_FORMAT");

    if(IsHEIF(extension))
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          "HEIF/AVIF formats are input-only in the current decode-only backend",
                          path, "IMGIO_FORMAT");

    if(extension == ".exr" &&
       image.spec.sample_type != SampleType::Float16 &&
       image.spec.sample_type != SampleType::Float32)
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          "EXR output supports Float16 and Float32 only",
                          path, "IMGIO_SAMPLE");

    if(extension == ".png" &&
       image.spec.sample_type != SampleType::UInt8 &&
       image.spec.sample_type != SampleType::UInt16)
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          "PNG output supports UInt8 and UInt16 only",
                          path, "IMGIO_SAMPLE");

    if(RequiresZeroOrigin(extension) &&
       (image.spec.data_window.left != 0 ||
        image.spec.data_window.top != 0))
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          String(FormatName(extension)) +
                          " output requires a zero data-window origin",
                          path, "IMGIO_SPEC");

    if((extension == ".png" || extension == ".jxl") &&
       image.spec.channel_layout == ChannelLayout::MultiChannel)
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          String(FormatName(extension)) +
                          " arbitrary multichannel output is unsupported",
                          path, "IMGIO_CHANNELS");

    if(extension == ".jxl" &&
       image.spec.channel_layout == ChannelLayout::GrayAlpha)
        return FailPolicy(ResultCode::Unsupported, diagnostics,
                          "JPEG XL GrayAlpha output is deferred until the backend exposes its alpha semantics reliably",
                          path, "IMGIO_CHANNELS");

    if(IsHDR(extension)) {
        if(image.spec.sample_type != SampleType::Float32)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "Radiance HDR/RGBE output requires Float32 samples",
                              path, "IMGIO_SAMPLE");
        if(image.spec.channel_layout != ChannelLayout::RGB)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "Radiance HDR/RGBE output supports RGB only",
                              path, "IMGIO_CHANNELS");
    }

    if(extension == ".dpx") {
        if(image.spec.sample_type != SampleType::UInt8 &&
           image.spec.sample_type != SampleType::UInt16)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial DPX output slice supports UInt8 and UInt16 only",
                              path, "IMGIO_SAMPLE");
        if(image.spec.channel_layout != ChannelLayout::RGB)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "the initial DPX output slice supports RGB only",
                              path, "IMGIO_CHANNELS");
        if(image.spec.data_window.left < 0 || image.spec.data_window.top < 0)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "DPX output requires a non-negative data-window origin",
                              path, "IMGIO_SPEC");
    }

    if(extension == ".webp") {
        if(image.spec.sample_type != SampleType::UInt8)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "WebP output supports UInt8 only",
                              path, "IMGIO_SAMPLE");
        if(image.spec.channel_layout != ChannelLayout::RGB &&
           image.spec.channel_layout != ChannelLayout::RGBA)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "WebP output supports RGB and RGBA only",
                              path, "IMGIO_CHANNELS");
    }

    if(IsTIFF(extension)) {
        if(image.spec.sample_type != SampleType::UInt8 &&
           image.spec.sample_type != SampleType::UInt16 &&
           image.spec.sample_type != SampleType::Float32)
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "TIFF output supports UInt8, UInt16 and Float32 only in the initial slice",
                              path, "IMGIO_SAMPLE");
        if(!IsCanonicalTIFFLayout(image.spec.channel_layout))
            return FailPolicy(ResultCode::Unsupported, diagnostics,
                              "TIFF output supports Gray, GrayAlpha, RGB and RGBA only in the initial slice",
                              path, "IMGIO_CHANNELS");
    }

    return Result::Success();
}

} // namespace IOFormatPolicy
} // namespace Imaging
} // namespace Upp
