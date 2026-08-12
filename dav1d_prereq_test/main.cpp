#include <Core/Core.h>
#include <dav1d/dav1d.h>
#include <dav1d/version.h>

#include <cstring>

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
    const char* version = dav1d_version();
    Check(state, version && std::strncmp(version, "1.5.4", 5) == 0,
          "runtime dav1d version is 1.5.4");
    unsigned api = dav1d_version_api();
    Check(state, DAV1D_API_MAJOR(api) == 7 &&
                 DAV1D_API_MINOR(api) == 0 &&
                 DAV1D_API_PATCH(api) == 0,
          "dav1d API version is 7.0.0");

    Dav1dSettings settings;
    dav1d_default_settings(&settings);
    Check(state, settings.apply_grain == 1 && settings.all_layers == 1,
          "default settings initialize deterministically");

    settings.n_threads = 1;
    settings.apply_grain = 0;
    Dav1dContext* context = nullptr;
    Check(state, dav1d_open(&context, &settings) == 0 && context,
          "scalar decoder context opens");

    uint8_t garbage[96];
    for(int i = 0; i < 96; ++i)
        garbage[i] = (uint8_t)((i * 41 + 17) & 0xff);
    Dav1dSequenceHeader header;
    Check(state, dav1d_parse_sequence_header(&header, garbage, sizeof(garbage)) < 0,
          "malformed AV1 sequence header is rejected");

    dav1d_close(&context);
    Check(state, context == nullptr, "decoder context closes cleanly");

    Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
