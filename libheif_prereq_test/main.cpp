#include <Core/Core.h>
#include <libheif/heif.h>

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
    const char* version = heif_get_version();
    Check(state, version && std::strcmp(version, "1.23.1") == 0,
          "runtime libheif version is 1.23.1");
    Check(state, heif_get_version_number() == LIBHEIF_NUMERIC_VERSION,
          "runtime libheif numeric version matches headers");

    heif_error init = heif_init(nullptr);
    Check(state, init.code == heif_error_Ok,
          "libheif initialization succeeds");
    Check(state, heif_have_decoder_for_format(heif_compression_AV1) != 0,
          "built-in AV1 decoder is available");
    Check(state, heif_have_decoder_for_format(heif_compression_HEVC) != 0,
          "built-in HEVC decoder is available");
    Check(state, heif_have_encoder_for_format(heif_compression_AV1) == 0,
          "AV1 encoder is deliberately absent");
    Check(state, heif_have_encoder_for_format(heif_compression_HEVC) == 0,
          "HEVC encoder is deliberately absent");

    heif_context* context = heif_context_alloc();
    Check(state, context != nullptr, "HEIF context allocates");
    if(context)
        heif_context_free(context);
    heif_deinit();

    Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
    SetExitCode(state.failed ? 1 : 0);
}
