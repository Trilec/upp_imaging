#include <Core/Core.h>
#include <webp/decode.h>
#include <webp/demux.h>
#include <webp/encode.h>
#include <webp/mux.h>

#include <cstring>

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
	Check(state, WebPGetDecoderVersion() == 0x010600,
	      "decoder runtime version is 1.6.0");
	Check(state, WebPGetEncoderVersion() == 0x010600,
	      "encoder runtime version is 1.6.0");
	Check(state, WebPGetDemuxVersion() == 0x010600,
	      "demux runtime version is 1.6.0");
	Check(state, WebPGetMuxVersion() == 0x010600,
	      "mux runtime version is 1.6.0");

	const uint8_t rgb[] = {
		0, 17, 255, 63, 127, 191,
		255, 1, 2, 11, 22, 33
	};
	uint8_t* encoded_rgb = nullptr;
	size_t rgb_size = WebPEncodeLosslessRGB(rgb, 2, 2, 6, &encoded_rgb);
	Check(state, rgb_size > 0 && encoded_rgb,
	      "lossless RGB encode");
	int width = 0, height = 0;
	Check(state, encoded_rgb && WebPGetInfo(encoded_rgb, rgb_size, &width, &height) &&
	             width == 2 && height == 2,
	      "encoded RGB dimensions");
	uint8_t* decoded_rgb = encoded_rgb
	                     ? WebPDecodeRGB(encoded_rgb, rgb_size, &width, &height)
	                     : nullptr;
	Check(state, decoded_rgb && width == 2 && height == 2 &&
	             memcmp(decoded_rgb, rgb, sizeof(rgb)) == 0,
	      "lossless RGB byte-exact roundtrip");
	WebPFree(decoded_rgb);
	WebPFree(encoded_rgb);

	const uint8_t rgba[] = {
		91, 37, 211, 0,    255, 12, 3, 64,
		17, 99, 201, 128,  4, 5, 6, 255
	};
	WebPConfig config;
	WebPPicture picture;
	WebPMemoryWriter writer;
	bool initialized = WebPConfigInit(&config) && WebPPictureInit(&picture);
	if(initialized) {
		config.lossless = 1;
		config.quality = 100.0f;
		config.method = 6;
		config.exact = 1;
		picture.width = 2;
		picture.height = 2;
		picture.use_argb = 1;
		WebPMemoryWriterInit(&writer);
		picture.writer = WebPMemoryWrite;
		picture.custom_ptr = &writer;
	}
	bool exact_encoded = initialized && WebPPictureImportRGBA(&picture, rgba, 8) &&
	                     WebPEncode(&config, &picture) && writer.size > 0;
	Check(state, exact_encoded,
	      "exact lossless RGBA encode");
	uint8_t* decoded_rgba = exact_encoded
	                      ? WebPDecodeRGBA(writer.mem, writer.size, &width, &height)
	                      : nullptr;
	Check(state, decoded_rgba && width == 2 && height == 2,
	      "encoded RGBA dimensions");
	Check(state, decoded_rgba && memcmp(decoded_rgba, rgba, sizeof(rgba)) == 0,
	      "exact RGBA preserves transparent RGB and alpha");
	WebPFree(decoded_rgba);
	if(initialized) {
		WebPPictureFree(&picture);
		WebPMemoryWriterClear(&writer);
	}

	uint8_t malformed[64];
	for(int i = 0; i < 64; ++i)
		malformed[i] = (uint8_t)((i * 43 + 9) & 0xff);
	Check(state, !WebPGetInfo(malformed, sizeof(malformed), nullptr, nullptr),
	      "malformed input rejection");

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
