#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <vector>

#include <openexr/Imf.h>
#include <imath/half.h>
#include <imath/ImathVec.h>
#include <zlib/zlib.h>
#include <libpng/png.h>
#include <libjpeg_turbo/jpeglib.h>
#include <libtiff/tiffio.h>
#include <opencolorio/OpenColorIO.h>
#include <fmt/format.h>
#include <robinmap/robin_map.h>

#ifndef UPP_IMAGING_LOCAL_OPENEXR_INCLUDE
#error stable OpenEXR marker not selected
#endif

#ifdef UPP_IMAGING_LOCAL_OPENEXR_SOURCE_INCLUDE
#error strict OpenEXR marker leaked into stable coexistence test
#endif

#ifndef UPP_IMAGING_LOCAL_OPENCOLORIO_INCLUDE
#error stable OpenColorIO marker not selected
#endif

#ifdef UPP_IMAGING_LOCAL_OPENCOLORIO_SOURCE_INCLUDE
#error strict OpenColorIO marker leaked into stable coexistence test
#endif

namespace OCIO = OCIO_NAMESPACE;

struct Result
{
    int passed = 0;
    int failed = 0;
};

static bool Check(Result& result, bool condition, const char* label)
{
    std::printf("%s: %s\n", label, condition ? "OK" : "FAIL");
    if(condition)
        ++result.passed;
    else
        ++result.failed;
    return condition;
}

static std::string MakeVersionString(int major, int minor, int patch)
{
    return fmt::format("{}.{}.{}", major, minor, patch);
}

static bool TestOpenEXR(Result& result, std::string& version)
{
    version = MakeVersionString(OPENEXR_VERSION_MAJOR, OPENEXR_VERSION_MINOR, OPENEXR_VERSION_PATCH);
    std::printf("OpenEXR=%s\n", version.c_str());

    bool ok = OPENEXR_VERSION_MAJOR == 3 && OPENEXR_VERSION_MINOR == 4 && OPENEXR_VERSION_PATCH == 13;

    Imf::Header header(4, 3);
    ok = ok && header.dataWindow().min.x == 0 && header.dataWindow().min.y == 0
        && header.dataWindow().max.x == 3 && header.dataWindow().max.y == 2;

    header.channels().insert("R", Imf::Channel(Imf::HALF));
    ok = ok && header.channels().findChannel("R") != nullptr;

    Imf::FrameBuffer frameBuffer;
    float sample = 0.25f;
    frameBuffer.insert("R", Imf::Slice(Imf::FLOAT, reinterpret_cast<char*>(&sample), sizeof(float), sizeof(float)));
    ok = ok && frameBuffer.findSlice("R") != nullptr;

    Check(result, ok, "PASS OpenEXR stable API");
    return ok;
}

static bool TestImath(Result& result, std::string& version)
{
    version = IMATH_VERSION_STRING;
    std::printf("Imath=%s\n", version.c_str());

    Imath::V2f a(1.0f, 2.0f);
    Imath::V2f b(3.0f, 4.0f);
    Imath::V2f c = a + b;
    Imath::half h = 2.25f;
    const bool ok = c.x == 4.0f && c.y == 6.0f && float(h) == 2.25f;

    Check(result, ok, "PASS Imath stable API");
    return ok;
}

static bool TestZlib(Result& result, std::string& version)
{
    const char* runtime = zlibVersion();
    version = runtime ? runtime : "";
    std::printf("zlib=%s\n", version.c_str());

    const unsigned char source[] = "OpenImageIO prerequisite coexistence";
    const uLong source_len = static_cast<uLong>(std::strlen(reinterpret_cast<const char*>(source)));
    const uLongf bound = compressBound(source_len);
    std::vector<Bytef> compressed(static_cast<size_t>(bound));
    uLongf compressed_len = bound;

    int rc = compress2(compressed.data(), &compressed_len, source, source_len, Z_BEST_SPEED);
    bool ok = rc == Z_OK;

    std::vector<Bytef> restored(static_cast<size_t>(source_len));
    uLongf restored_len = source_len;
    if(ok)
    {
        rc = uncompress(restored.data(), &restored_len, compressed.data(), compressed_len);
        ok = rc == Z_OK && restored_len == source_len
            && std::memcmp(restored.data(), source, static_cast<size_t>(source_len)) == 0;
    }

    Check(result, ok, "PASS zlib stable round-trip");
    return ok;
}

static bool TestLibpng(Result& result, std::string& version)
{
    version = PNG_LIBPNG_VER_STRING;
    std::printf("libpng=%s\n", png_libpng_ver ? png_libpng_ver : "(null)");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info_ptr = nullptr;
    bool ok = png_ptr != nullptr;

    if(ok)
    {
        info_ptr = png_create_info_struct(png_ptr);
        ok = info_ptr != nullptr;
    }

    if(png_ptr)
        png_destroy_write_struct(&png_ptr, info_ptr ? &info_ptr : nullptr);

    Check(result, ok, "PASS libpng stable API");
    return ok;
}

static bool TestLibjpegTurbo(Result& result, std::string& version)
{
    version = LIBJPEG_TURBO_VERSION;
    std::printf("libjpeg-turbo=%s\n", version.c_str());

    jpeg_error_mgr jerr;
    jpeg_compress_struct cinfo;
    std::memset(&cinfo, 0, sizeof(cinfo));
    cinfo.err = jpeg_std_error(&jerr);
    bool ok = cinfo.err != nullptr;

    if(ok)
    {
        jpeg_create_compress(&cinfo);
        ok = cinfo.mem != nullptr && cinfo.is_decompressor == FALSE;
        jpeg_destroy_compress(&cinfo);
    }

    Check(result, ok, "PASS libjpeg-turbo stable API");
    return ok;
}

static bool TestLibtiff(Result& result, std::string& version)
{
    const char* v = TIFFGetVersion();
    version = v ? v : "";
    std::printf("libtiff=%s\n", version.c_str());

    const bool ok = !version.empty() && version.find("4.7.2") != std::string::npos;
    Check(result, ok, "PASS libtiff stable API");
    return ok;
}

static bool TestOpenColorIO(Result& result, std::string& version)
{
    const char* runtime = OCIO::GetVersion();
    version = runtime ? runtime : "";
    std::printf("OpenColorIO=%s\n", version.c_str());

    bool ok = OCIO_VERSION_MAJOR == 2 && OCIO_VERSION_MINOR == 5 && std::strncmp(version.c_str(), "2.5.2", 5) == 0;

    try
    {
        OCIO::ConstConfigRcPtr config = OCIO::Config::Create();
        OCIO::MatrixTransformRcPtr matrix = OCIO::MatrixTransform::Create();
        OCIO::ConstProcessorRcPtr processor = config ? config->getProcessor(matrix) : OCIO::ConstProcessorRcPtr();
        OCIO::ConstCPUProcessorRcPtr cpu = processor ? processor->getDefaultCPUProcessor() : OCIO::ConstCPUProcessorRcPtr();

        float pixel[4] = {0.25f, 0.5f, 0.75f, 1.0f};
        if(cpu)
            cpu->applyRGBA(pixel);
        ok = ok && config && processor && cpu && pixel[0] == 0.25f && pixel[1] == 0.5f && pixel[2] == 0.75f && pixel[3] == 1.0f;
    }
    catch(const OCIO::Exception& e)
    {
        std::printf("OpenColorIO exception: %s\n", e.what());
        ok = false;
    }

    Check(result, ok, "PASS OpenColorIO stable API");
    return ok;
}

static bool TestFmt(Result& result, std::string& version)
{
    const int major = FMT_VERSION / 10000;
    const int minor = (FMT_VERSION / 100) % 100;
    const int patch = FMT_VERSION % 100;
    version = MakeVersionString(major, minor, patch);
    std::printf("fmt=%s\n", version.c_str());

    const std::string text = fmt::format("{} {}", 42, "fmt");
    const bool ok = text == "42 fmt";

    Check(result, ok, "PASS fmt stable API");
    return ok;
}

static bool TestRobinMap(Result& result, std::string& version)
{
    version = MakeVersionString(TSL_RH_VERSION_MAJOR, TSL_RH_VERSION_MINOR, TSL_RH_VERSION_PATCH);
    std::printf("robin-map=%s\n", version.c_str());

    tsl::robin_map<std::string, int> values;
    values["alpha"] = 1;
    values["beta"] = 2;
    values["alpha"] = 3;

    const bool ok = values.size() == 2
        && values.find("alpha") != values.end()
        && values.find("beta") != values.end()
        && values.at("alpha") == 3
        && values.at("beta") == 2;

    Check(result, ok, "PASS robin-map stable API");
    return ok;
}

static bool TestCombinedCoexistence(Result& result, const std::string& openexr_version, const std::string& imath_version,
    const std::string& zlib_ver, const std::string& png_version, const std::string& jpeg_version,
    const std::string& tiff_version, const std::string& ocio_version, const std::string& fmt_version,
    const std::string& robin_version)
{
    tsl::robin_map<std::string, std::string> versions;
    versions["OpenEXR"] = openexr_version;
    versions["Imath"] = imath_version;
    versions["zlib"] = zlib_ver;
    versions["libpng"] = png_version;
    versions["libjpeg-turbo"] = jpeg_version;
    versions["libtiff"] = tiff_version;
    versions["OpenColorIO"] = ocio_version;
    versions["fmt"] = fmt_version;
    versions["robin-map"] = robin_version;

    const std::string summary = fmt::format(
        "coexistence: OpenEXR={} Imath={} zlib={} libpng={} libjpeg-turbo={} libtiff={} OpenColorIO={} fmt={} robin-map={}",
        versions.at("OpenEXR"), versions.at("Imath"), versions.at("zlib"), versions.at("libpng"),
        versions.at("libjpeg-turbo"), versions.at("libtiff"), versions.at("OpenColorIO"), versions.at("fmt"),
        versions.at("robin-map"));

    const bool ok = summary.find("OpenEXR") != std::string::npos
        && summary.find("Imath") != std::string::npos
        && summary.find("zlib") != std::string::npos
        && summary.find("libpng") != std::string::npos
        && summary.find("libjpeg-turbo") != std::string::npos
        && summary.find("libtiff") != std::string::npos
        && summary.find("OpenColorIO") != std::string::npos
        && summary.find("fmt") != std::string::npos
        && summary.find("robin-map") != std::string::npos;

    std::printf("%s\n", summary.c_str());
    Check(result, ok, "PASS combined coexistence");
    return ok;
}

int main()
{
    Result result;

    Check(result, true, "PASS OpenEXR include marker");
    Check(result, true, "PASS OpenColorIO include marker");

    std::string openexr_version;
    std::string imath_version;
    std::string zlib_ver;
    std::string png_version;
    std::string jpeg_version;
    std::string tiff_version;
    std::string ocio_version;
    std::string fmt_version;
    std::string robin_version;

    TestOpenEXR(result, openexr_version);
    TestImath(result, imath_version);
    TestZlib(result, zlib_ver);
    TestLibpng(result, png_version);
    TestLibjpegTurbo(result, jpeg_version);
    TestLibtiff(result, tiff_version);
    TestOpenColorIO(result, ocio_version);
    TestFmt(result, fmt_version);
    TestRobinMap(result, robin_version);
    TestCombinedCoexistence(result, openexr_version, imath_version, zlib_ver, png_version, jpeg_version,
        tiff_version, ocio_version, fmt_version, robin_version);

    std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
    return result.failed ? 1 : 0;
}
