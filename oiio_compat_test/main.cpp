#include <oiio/OIIO.h>

#include <cstdio>

using namespace OIIO;
using namespace UppImaging;

static bool Check(bool condition, const char* label, int& passed, int& failed)
{
    std::printf("%s %s\n", condition ? "PASS" : "FAIL", label);
    condition ? ++passed : ++failed;
    return condition;
}

int main()
{
    int passed = 0;
    int failed = 0;
    Check(sizeof(ImageSpec) > 0, "compatibility header exposes OIIO types", passed, failed);
    InitializeOpenImageIO();
    Check(get_string_attribute("input_format_list").find("openexr") != std::string::npos,
          "OpenEXR registered", passed, failed);
    Check(get_string_attribute("input_format_list").find("png") != std::string::npos,
          "PNG registered", passed, failed);
    std::printf("SUMMARY passed=%d failed=%d\n", passed, failed);
    return failed ? 1 : 0;
}
