#include <Core/Core.h>

#include <jxl/color_encoding.h>
#include <jxl/decode.h>
#include <jxl/encode.h>
#include <jxl/resizable_parallel_runner.h>
#include <jxl/types.h>
#include <jxl/version.h>

#include <climits>
#include <cstring>

using namespace Upp;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const char* name)
{
	Cout() << (condition ? "PASS " : "FAIL ") << name << '\n';
	(condition ? state.passed : state.failed)++;
}

static bool EncodeLosslessRgb(const byte* pixels, size_t pixel_bytes,
                              Vector<byte>& encoded)
{
	JxlEncoder* encoder = JxlEncoderCreate(nullptr);
	if(!encoder)
		return false;
	void* runner = JxlResizableParallelRunnerCreate(nullptr);
	if(!runner) {
		JxlEncoderDestroy(encoder);
		return false;
	}

	JxlResizableParallelRunnerSetThreads(runner, 2);
	bool ok = JxlEncoderSetParallelRunner(encoder, JxlResizableParallelRunner, runner) == JXL_ENC_SUCCESS;
	JxlBasicInfo info;
	JxlEncoderInitBasicInfo(&info);
	info.xsize = 2;
	info.ysize = 1;
	info.bits_per_sample = 8;
	info.exponent_bits_per_sample = 0;
	info.uses_original_profile = JXL_TRUE;
	ok = ok && JxlEncoderSetBasicInfo(encoder, &info) == JXL_ENC_SUCCESS;

	JxlColorEncoding color = {};
	JxlColorEncodingSetToSRGB(&color, JXL_FALSE);
	ok = ok && JxlEncoderSetColorEncoding(encoder, &color) == JXL_ENC_SUCCESS;

	JxlEncoderFrameSettings* frame = JxlEncoderFrameSettingsCreate(encoder, nullptr);
	ok = ok && frame != nullptr;
	if(frame)
		ok = ok && JxlEncoderSetFrameLossless(frame, JXL_TRUE) == JXL_ENC_SUCCESS;

	const JxlPixelFormat format = {3, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
	if(ok)
		ok = JxlEncoderAddImageFrame(frame, &format, pixels, pixel_bytes) == JXL_ENC_SUCCESS;
	JxlEncoderCloseInput(encoder);

	if(ok) {
		encoded.SetCount(256);
		byte* next = encoded.Begin();
		size_t available = encoded.GetCount();
		for(;;) {
			JxlEncoderStatus status = JxlEncoderProcessOutput(encoder, &next, &available);
			if(status == JXL_ENC_SUCCESS) {
				encoded.SetCount((int)(next - encoded.Begin()));
				break;
			}
			if(status != JXL_ENC_NEED_MORE_OUTPUT) {
				ok = false;
				break;
			}
			const int used = (int)(next - encoded.Begin());
			if(encoded.GetCount() > INT_MAX / 2) {
				ok = false;
				break;
			}
			encoded.SetCount(encoded.GetCount() * 2);
			next = encoded.Begin() + used;
			available = encoded.GetCount() - used;
		}
	}

	JxlResizableParallelRunnerDestroy(runner);
	JxlEncoderDestroy(encoder);
	return ok && !encoded.IsEmpty();
}

static bool DecodeRgb(const Vector<byte>& encoded, Vector<byte>& pixels,
                      uint32& width, uint32& height)
{
	JxlDecoder* decoder = JxlDecoderCreate(nullptr);
	if(!decoder)
		return false;
	void* runner = JxlResizableParallelRunnerCreate(nullptr);
	if(!runner) {
		JxlDecoderDestroy(decoder);
		return false;
	}

	bool ok = JxlDecoderSubscribeEvents(decoder, JXL_DEC_BASIC_INFO | JXL_DEC_FULL_IMAGE) == JXL_DEC_SUCCESS;
	ok = ok && JxlDecoderSetParallelRunner(decoder, JxlResizableParallelRunner, runner) == JXL_DEC_SUCCESS;
	if(ok) {
		JxlDecoderSetInput(decoder, encoded.Begin(), encoded.GetCount());
		JxlDecoderCloseInput(decoder);
	}

	const JxlPixelFormat format = {3, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
	bool complete = false;
	while(ok && !complete) {
		JxlDecoderStatus status = JxlDecoderProcessInput(decoder);
		switch(status) {
		case JXL_DEC_BASIC_INFO: {
			JxlBasicInfo info;
			ok = JxlDecoderGetBasicInfo(decoder, &info) == JXL_DEC_SUCCESS;
			if(ok) {
				width = info.xsize;
				height = info.ysize;
				JxlResizableParallelRunnerSetThreads(
					runner, JxlResizableParallelRunnerSuggestThreads(width, height));
			}
			break;
		}
		case JXL_DEC_NEED_IMAGE_OUT_BUFFER: {
			size_t bytes = 0;
			ok = JxlDecoderImageOutBufferSize(decoder, &format, &bytes) == JXL_DEC_SUCCESS;
			if(ok && bytes <= INT_MAX) {
				pixels.SetCount((int)bytes);
				ok = JxlDecoderSetImageOutBuffer(decoder, &format, pixels.Begin(), bytes) == JXL_DEC_SUCCESS;
			}
			else
				ok = false;
			break;
		}
		case JXL_DEC_FULL_IMAGE:
			break;
		case JXL_DEC_SUCCESS:
			complete = true;
			break;
		default:
			ok = false;
			break;
		}
	}

	JxlResizableParallelRunnerDestroy(runner);
	JxlDecoderDestroy(decoder);
	return ok && complete;
}

static bool RejectMalformed()
{
	const byte invalid[] = {0xff, 0x0a, 0x00, 0x01, 0x02, 0x03};
	JxlDecoder* decoder = JxlDecoderCreate(nullptr);
	if(!decoder)
		return false;
	JxlDecoderSetInput(decoder, invalid, sizeof(invalid));
	JxlDecoderCloseInput(decoder);
	JxlDecoderStatus status = JxlDecoderProcessInput(decoder);
	JxlDecoderDestroy(decoder);
	return status == JXL_DEC_ERROR || status == JXL_DEC_NEED_MORE_INPUT;
}

CONSOLE_APP_MAIN
{
	State state;
	Check(state, JPEGXL_MAJOR_VERSION == 0 && JPEGXL_MINOR_VERSION == 12 && JPEGXL_PATCH_VERSION == 0,
	      "generated version header is 0.12.0");
	Check(state, JxlEncoderVersion() == 12000, "encoder runtime version is 0.12.0");
	Check(state, JxlDecoderVersion() == 12000, "decoder runtime version is 0.12.0");

	void* runner = JxlResizableParallelRunnerCreate(nullptr);
	Check(state, runner != nullptr, "resizable thread runner creates");
	if(runner)
		JxlResizableParallelRunnerDestroy(runner);

	const byte source[] = {0, 127, 255, 255, 64, 1};
	Vector<byte> encoded;
	bool encoded_ok = EncodeLosslessRgb(source, sizeof(source), encoded);
	Check(state, encoded_ok, "lossless RGB8 encode succeeds");
	JxlSignature signature = encoded_ok ? JxlSignatureCheck(encoded.Begin(), encoded.GetCount()) : JXL_SIG_INVALID;
	Check(state, signature == JXL_SIG_CODESTREAM || signature == JXL_SIG_CONTAINER,
	      "encoded bytes have JPEG XL signature");

	Vector<byte> decoded;
	uint32 width = 0;
	uint32 height = 0;
	bool decoded_ok = encoded_ok && DecodeRgb(encoded, decoded, width, height);
	Check(state, decoded_ok && width == 2 && height == 1, "decoder reports 2x1 basic info");
	Check(state, decoded_ok && decoded.GetCount() == (int)sizeof(source) &&
	                   memcmp(decoded.Begin(), source, sizeof(source)) == 0,
	      "lossless RGB8 roundtrip is byte-exact");
	Check(state, RejectMalformed(), "malformed JPEG XL input is rejected");

	Cout() << "SUMMARY passed=" << state.passed << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
