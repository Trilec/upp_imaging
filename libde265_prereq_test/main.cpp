#include <Core/Core.h>
#include <libde265/de265.h>

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
    const char* version = de265_get_version();
    Check(state, version && std::strcmp(version, "1.1.1") == 0,
          "runtime libde265 version is 1.1.1");
    Check(state, de265_get_version_number() == 0x01010100,
          "numeric libde265 version is 0x01010100");
    Check(state, de265_init() == DE265_OK,
          "explicit static library initialization succeeds");
    Check(state, de265_free() == DE265_OK,
          "explicit static library shutdown succeeds");

    de265_decoder_context* context = de265_new_decoder();
    Check(state, context != nullptr, "decoder context opens");
    if(context)
        de265_set_parameter_int(context, DE265_DECODER_PARAM_ACCELERATION_CODE,
                                de265_acceleration_SCALAR);
    int more = 0;
    de265_error idle = context ? de265_decode(context, &more)
                               : DE265_ERROR_LIBRARY_INITIALIZATION_FAILED;
    Check(state, idle == DE265_ERROR_WAITING_FOR_INPUT_DATA,
          "empty decoder waits for input data");
    Check(state, context && de265_free_decoder(context) == DE265_OK,
          "decoder context closes cleanly");

    Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
