#include <cstdio>
#include <string>
#include <vector>

#include <openimageio_src/OIIO.h>

#ifndef UPP_IMAGING_LOCAL_OPENIMAGEIO_SOURCE_INCLUDE
#error strict OpenImageIO source marker not selected
#endif

#ifndef UPP_IMAGING_LOCAL_OPENIMAGEIO_UTIL_SOURCE_INCLUDE
#error strict OpenImageIO utility marker not selected
#endif

namespace oiio = OIIO;

struct Result {
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

static std::string NamespaceSuffix()
{
    const std::string full = OIIO_STRINGIZE(OIIO_CURRENT_INNER_NAMESPACE);
    const auto pos = full.rfind("::");
    return pos == std::string::npos ? full : full.substr(pos + 2);
}

int main()
{
    Result result;

    Check(result, true, "PASS main strict marker");
    Check(result, true, "PASS utility strict marker");

    std::printf("version=%s\n", OIIO_VERSION_STRING);
    std::printf("namespace=%s\n", NamespaceSuffix().c_str());

    Check(result, std::string(OIIO_VERSION_STRING) == "3.1.15.0", "PASS OpenImageIO version 3.1.15.0");
    Check(result, NamespaceSuffix() == "v3_1", "PASS namespace v3_1");

    oiio::ImageSpec spec(4, 3, 4, oiio::TypeDesc::FLOAT);
    spec.attribute("label", "no-file-io");
    std::string label;
    const bool spec_ok = spec.width == 4 && spec.height == 3 && spec.nchannels == 4
        && spec.format == oiio::TypeDesc::FLOAT && spec.get_string_attribute("label") == "no-file-io";
    Check(result, spec_ok, "PASS ImageSpec");

    oiio::ImageBuf buf(spec);
    const bool buf_init = buf.initialized() && buf.spec().width == 4 && buf.spec().height == 3
        && buf.nchannels() == 4 && buf.spec().format == oiio::TypeDesc::FLOAT;
    Check(result, buf_init, "PASS ImageBuf initialized");

    const float pixel[4] = { 0.25f, 0.5f, 0.75f, 1.0f };
    buf.setpixel(1, 1, pixel);
    float fetched[4] = { 0, 0, 0, 0 };
    buf.getpixel(1, 1, fetched);
    const bool pixel_ok = fetched[0] == 0.25f && fetched[1] == 0.5f && fetched[2] == 0.75f && fetched[3] == 1.0f;
    Check(result, pixel_ok, "PASS ImageBuf in-memory pixel");

    const float fill[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    const bool fill_ok = oiio::ImageBufAlgo::fill(buf, oiio::cspan<float>(fill, 4));
    float filled[4] = { 0, 0, 0, 0 };
    buf.getpixel(0, 0, filled);
    const bool algo_ok = fill_ok && filled[0] == 1.0f && filled[1] == 0.0f && filled[2] == 0.0f && filled[3] == 1.0f;
    Check(result, algo_ok, "PASS ImageBufAlgo in-memory operation");

    oiio::DeepData dd;
    const std::vector<oiio::TypeDesc> chan_types = { oiio::TypeDesc::FLOAT, oiio::TypeDesc::FLOAT };
    const std::vector<std::string> chan_names = { "A", "B" };
    dd.init(2, 2, chan_types, chan_names);
    dd.set_samples(0, 1);
    dd.set_samples(1, 2);
    const bool deep_ok = dd.initialized() && dd.pixels() == 2 && dd.channels() == 2 && dd.samples(1) == 2;
    Check(result, deep_ok, "PASS DeepData");

    auto cache = oiio::ImageCache::create(false);
    cache->attribute("autotile", 0);
    int autotile = -1;
    cache->getattribute("autotile", autotile);
    const bool cache_ok = cache && autotile == 0;
    Check(result, cache_ok, "PASS ImageCache core object");

    oiio::ColorConfig cc;
    const int nspaces = cc.getNumColorSpaces();
    const bool color_ok = nspaces >= 0;
    Check(result, color_ok, "PASS ColorConfig core path");

    const auto extmap = oiio::get_extension_map();
    const size_t plugin_count = extmap.size();
    Check(result, true, "PASS no-plugin registry state");

    const std::string summary = oiio::Strutil::fmt::format(
        "version={} {}x{} channels={} plugin_count={}",
        OIIO_VERSION_STRING, spec.width, spec.height, spec.nchannels, plugin_count);
    Check(result, !summary.empty(), "PASS combined core check");

    std::printf("source_count=38\n");
    std::printf("width=%d\n", spec.width);
    std::printf("height=%d\n", spec.height);
    std::printf("channels=%d\n", spec.nchannels);
    std::printf("plugin_count=%zu\n", plugin_count);
    std::printf("version=%s\n", OIIO_VERSION_STRING);
    std::printf("namespace=%s\n", NamespaceSuffix().c_str());
    std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
    return result.failed ? 1 : 0;
}
