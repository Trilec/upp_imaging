#include <Core/Core.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/bsf.h>
#include <libavcodec/codec.h>
#include <libavcodec/version.h>
}

using namespace Upp;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const char* label)
{
	Cout() << (condition ? "PASS " : "FAIL ") << label << '\n';
	(condition ? state.passed : state.failed)++;
}

CONSOLE_APP_MAIN
{
	State state;

	Check(state, avcodec_version() == LIBAVCODEC_VERSION_INT,
	      "libavcodec runtime version matches headers");
	Check(state, String(avcodec_license()).Find("LGPL") >= 0,
	      "libavcodec reports LGPL configuration");
	String configuration = avcodec_configuration();
	Check(state, configuration.Find("--enable-decoder=h264") >= 0 &&
	             configuration.Find("--disable-asm") >= 0,
	      "libavcodec reports the bounded scalar decode configuration");

	const AVCodec* decoder = avcodec_find_decoder(AV_CODEC_ID_H264);
	Check(state, decoder != nullptr, "native H.264 decoder is registered");
	Check(state, decoder && String(decoder->name) == "h264" && av_codec_is_decoder(decoder),
	      "registered H.264 codec is the native decoder");
	Check(state, avcodec_find_encoder(AV_CODEC_ID_H264) == nullptr,
	      "H.264 encoder is not registered");

	void* iterator = nullptr;
	const AVCodec* first = av_codec_iterate(&iterator);
	const AVCodec* second = av_codec_iterate(&iterator);
	Check(state, first == decoder && second == nullptr,
	      "codec registry contains exactly the H.264 decoder");

	AVCodecParserContext* parser = av_parser_init(AV_CODEC_ID_H264);
	Check(state, parser == nullptr,
	      "public H.264 parser remains disabled");
	if(parser)
		av_parser_close(parser);

	void* bsf_iterator = nullptr;
	Check(state, av_bsf_iterate(&bsf_iterator) == nullptr,
	      "bitstream-filter registry remains empty");

	AVCodecContext* context = decoder ? avcodec_alloc_context3(decoder) : nullptr;
	Check(state, context != nullptr, "H.264 decoder context allocation works");
	int open_result = context && decoder ? avcodec_open2(context, decoder, nullptr) : AVERROR(EINVAL);
	Check(state, open_result == 0, "H.264 decoder context opens without external backends");
	avcodec_free_context(&context);
	Check(state, context == nullptr, "H.264 decoder context cleanup is deterministic");

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
