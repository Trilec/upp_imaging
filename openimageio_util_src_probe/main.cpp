#include <cstdio>
#include <string>
#include <vector>

#include <openimageio_util_src/OIIOUtil.h>

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

    Check(result, true, "PASS strict utility marker");

    std::printf("version=%s\n", OIIO_VERSION_STRING);
    std::printf("namespace=%s\n", NamespaceSuffix().c_str());

    Check(result, std::string(OIIO_VERSION_STRING) == "3.1.15.0", "PASS OpenImageIO version 3.1.15.0");
    Check(result, NamespaceSuffix() == "v3_1", "PASS namespace v3_1");

    oiio::TypeDesc td_float(oiio::TypeDesc::FLOAT);
    oiio::TypeDesc td_u8(oiio::TypeDesc::UINT8);
    const bool type_ok = td_float.basetype == oiio::TypeDesc::FLOAT
        && td_float.elementsize() == 4
        && std::string(td_float.c_str()) == "float"
        && td_u8.basetype == oiio::TypeDesc::UINT8
        && td_u8.elementsize() == 1
        && std::string(td_u8.c_str()) == "uint8";
    Check(result, type_ok, "PASS TypeDesc");

    oiio::ustring s1("green");
    oiio::ustring s2("green");
    oiio::ustring s3("blue");
    const bool ustring_ok = s1 == s2 && s1 != s3 && std::string(s1.string()) == "green" && s1.hash() == s2.hash();
    Check(result, ustring_ok, "PASS ustring");

    std::vector<std::string> split_values;
    oiio::Strutil::split("alpha,beta,gamma", split_values, ",");
    const std::string joined = oiio::Strutil::join(split_values, "|");
    const std::string formatted = oiio::Strutil::fmt::format("{}-{}", joined, 7);
    const bool strutil_ok = split_values.size() == 3 && split_values[0] == "alpha" && joined == "alpha|beta|gamma"
        && formatted == "alpha|beta|gamma-7";
    Check(result, strutil_ok, "PASS Strutil");

    const std::string filename = oiio::Filesystem::filename("folder/sub/file.txt");
    const std::string ext = oiio::Filesystem::extension("folder/sub/file.txt");
    const std::string parent = oiio::Filesystem::parent_path("folder/sub/file.txt");
    const std::string replaced = oiio::Filesystem::replace_extension("folder/sub/file.txt", ".exr");
    const bool filesystem_ok = filename == "file.txt" && ext == ".txt" && parent == "folder/sub" && replaced == "folder/sub/file.exr";
    Check(result, filesystem_ok, "PASS Filesystem");

    oiio::Timer timer(false);
    timer.start();
    volatile int sink = 0;
    for(int i = 0; i < 1000; ++i)
        sink += i;
    const double elapsed = timer.stop();
    const bool timer_ok = elapsed >= 0.0;
    Check(result, timer_ok, "PASS Timer");

    oiio::ParamValue pv("samples", 42);
    oiio::ParamValueList pvl;
    pvl.add_or_replace(pv);
    pvl.attribute("label", "cyan");
    const oiio::ParamValue* stored = pvl.find_pv("samples", oiio::TypeDesc::INT);
    const std::string label = std::string(pvl.get_string("label"));
    const bool param_ok = stored && stored->name() == oiio::ustring("samples") && stored->type() == oiio::TypeDesc::INT
        && stored->get_int() == 42 && label == "cyan";
    Check(result, param_ok, "PASS ParamValue");

    const std::string combined = stored ? oiio::Strutil::fmt::format("{}={}", std::string(stored->name().c_str()), stored->get_int())
                                        : std::string();
    const bool combined_ok = stored && combined == "samples=42" && pvl.getattributetype("samples") == oiio::TypeDesc::INT;
    Check(result, combined_ok, "PASS combined utility check");

    std::printf("source_count=18\n");
    std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
    return result.failed ? 1 : 0;
}
