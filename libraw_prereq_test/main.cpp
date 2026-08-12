#include <Core/Core.h>
#include <libraw/libraw.h>
#include <libraw/libraw_version.h>

#include <cstring>

using namespace Upp;

struct State {
    int passed = 0;
    int failed = 0;
};

static void Check(State& state, bool condition, const char* label)
{
    Cout() << (condition ? "PASS " : "FAIL ") << label << '\n';
    condition ? ++state.passed : ++state.failed;
}

CONSOLE_APP_MAIN
{
    State state;

    Check(state,
          LIBRAW_MAJOR_VERSION == 0 && LIBRAW_MINOR_VERSION == 22 &&
          LIBRAW_PATCH_VERSION == 2,
          "compile-time LibRaw version is 0.22.2");
    Check(state, LibRaw::versionNumber() == LIBRAW_VERSION,
          "runtime LibRaw version matches headers");
    const char* version = LibRaw::version();
    Check(state, version && std::strncmp(version, "0.22.2", 6) == 0,
          "runtime version string starts with 0.22.2");

    int camera_count = LibRaw::cameraCount();
    Check(state, camera_count > 100,
          "compiled camera table is populated");
    const char** cameras = LibRaw::cameraList();
    Check(state, cameras && cameras[0] && *cameras[0],
          "camera list exposes a first model");

    LibRaw processor;
    Check(state, processor.output_params_ptr() != nullptr,
          "LibRaw processor constructs with output parameters");

    unsigned char garbage[256] = {};
    for(int i = 0; i < 256; ++i)
        garbage[i] = static_cast<unsigned char>((i * 37 + 11) & 0xff);
    int malformed = processor.open_buffer(garbage, sizeof(garbage));
    Check(state, malformed != LIBRAW_SUCCESS,
          "malformed in-memory input is rejected");
    const char* error = malformed == LIBRAW_SUCCESS ? nullptr : LibRaw::strerror(malformed);
    Check(state, error && *error,
          "malformed input has a LibRaw error string");

    processor.recycle();
    Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
