#include <stdio.h>
#include <string.h>

#include <openexr_core_src/openexr.h>

static int fail(const char* label, int& failed)
{
	printf("%s: FAIL\n", label);
	++failed;
	return 1;
}

static int check_ok(const char* label, int& passed)
{
	printf("%s: OK\n", label);
	++passed;
	return 0;
}

static void setup_channel(exr_coding_channel_info_t& ch, const float* base)
{
	ch.user_data_type = EXR_PIXEL_FLOAT;
	ch.user_bytes_per_element = (int16_t)sizeof(float);
	ch.user_pixel_stride = (int32_t)sizeof(float);
	ch.user_line_stride = (int32_t)(2 * sizeof(float));
	ch.encode_from_ptr = reinterpret_cast<const uint8_t*>(base);
}

static void setup_decode_channel(exr_coding_channel_info_t& ch, float* base)
{
	ch.user_data_type = EXR_PIXEL_FLOAT;
	ch.user_bytes_per_element = (int16_t)sizeof(float);
	ch.user_pixel_stride = (int32_t)sizeof(float);
	ch.user_line_stride = (int32_t)(2 * sizeof(float));
	ch.decode_to_ptr = reinterpret_cast<uint8_t*>(base);
}

int main()
{
	int passed = 0;
	int failed = 0;
	const char* path = "E:/apps/github/upp_imaging/out/openexr_core_roundtrip.exr";
	const float input[4] = {0.0f, 0.5f, 1.0f, 2.0f};
	float output[4] = {0.f, 0.f, 0.f, 0.f};

	exr_context_t wctxt = 0;
	exr_result_t r = exr_start_write(&wctxt, path, EXR_WRITE_FILE_DIRECTLY, 0);
	if(r != EXR_ERR_SUCCESS || !wctxt) return fail("OpenEXRCore write context", failed);
	check_ok("OpenEXRCore write context", passed);

	int part_index = -1;
	r = exr_add_part(wctxt, 0, EXR_STORAGE_SCANLINE, &part_index);
	if(r != EXR_ERR_SUCCESS || part_index != 0) {
		exr_finish(&wctxt);
		return fail("OpenEXRCore required attrs", failed);
	}
	r = exr_initialize_required_attr_simple(wctxt, part_index, 2, 2, EXR_COMPRESSION_NONE);
	if(r != EXR_ERR_SUCCESS) {
		exr_finish(&wctxt);
		return fail("OpenEXRCore required attrs", failed);
	}
	r = exr_add_channel(wctxt, part_index, "Y", EXR_PIXEL_HALF, EXR_PERCEPTUALLY_LINEAR, 1, 1);
	if(r != EXR_ERR_SUCCESS) {
		exr_finish(&wctxt);
		return fail("OpenEXRCore required attrs", failed);
	}
	check_ok("OpenEXRCore required attrs", passed);

	r = exr_write_header(wctxt);
	if(r != EXR_ERR_SUCCESS) {
		exr_finish(&wctxt);
		return fail("OpenEXRCore write header", failed);
	}
	check_ok("OpenEXRCore write header", passed);

	for(int y = 0; y < 2; ++y) {
		exr_chunk_info_t cinfo;
		exr_encode_pipeline_t encode = EXR_ENCODE_PIPELINE_INITIALIZER;
		r = exr_write_scanline_chunk_info(wctxt, part_index, y, &cinfo);
		if(r != EXR_ERR_SUCCESS) { exr_encoding_destroy(wctxt, &encode); exr_finish(&wctxt); return fail("OpenEXRCore pixel encode", failed); }
		r = exr_encoding_initialize(wctxt, part_index, &cinfo, &encode);
		if(r != EXR_ERR_SUCCESS) { exr_encoding_destroy(wctxt, &encode); exr_finish(&wctxt); return fail("OpenEXRCore pixel encode", failed); }
		if(encode.channel_count != 1 || !encode.channels || !encode.channels[0].channel_name || strcmp(encode.channels[0].channel_name, "Y") != 0) {
			exr_encoding_destroy(wctxt, &encode);
			exr_finish(&wctxt);
			return fail("OpenEXRCore pixel encode", failed);
		}
		setup_channel(encode.channels[0], &input[y * 2]);
		r = exr_encoding_choose_default_routines(wctxt, part_index, &encode);
		if(r != EXR_ERR_SUCCESS) { exr_encoding_destroy(wctxt, &encode); exr_finish(&wctxt); return fail("OpenEXRCore pixel encode", failed); }
		r = exr_encoding_run(wctxt, part_index, &encode);
		if(r != EXR_ERR_SUCCESS) { exr_encoding_destroy(wctxt, &encode); exr_finish(&wctxt); return fail("OpenEXRCore pixel encode", failed); }
		r = exr_encoding_destroy(wctxt, &encode);
		if(r != EXR_ERR_SUCCESS) { exr_finish(&wctxt); return fail("OpenEXRCore pixel encode", failed); }
	}

	r = exr_finish(&wctxt);
	if(r != EXR_ERR_SUCCESS) return fail("OpenEXRCore pixel encode", failed);
	check_ok("OpenEXRCore pixel encode", passed);

	exr_context_t rctxt = 0;
	r = exr_start_read(&rctxt, path, 0);
	if(r != EXR_ERR_SUCCESS || !rctxt) return fail("OpenEXRCore read header", failed);

	int count = 0;
	r = exr_get_count(rctxt, &count);
	if(r != EXR_ERR_SUCCESS || count != 1) { exr_finish(&rctxt); return fail("OpenEXRCore read header", failed); }

	exr_attr_box2i_t dw;
	r = exr_get_data_window(rctxt, 0, &dw);
	if(r != EXR_ERR_SUCCESS || dw.min.x != 0 || dw.min.y != 0 || dw.max.x != 1 || dw.max.y != 1) {
		exr_finish(&rctxt);
		return fail("OpenEXRCore read header", failed);
	}

	for(int y = 0; y < 2; ++y) {
		exr_chunk_info_t cinfo;
		exr_decode_pipeline_t decode = EXR_DECODE_PIPELINE_INITIALIZER;
		r = exr_read_scanline_chunk_info(rctxt, 0, y, &cinfo);
		if(r != EXR_ERR_SUCCESS) { exr_decoding_destroy(rctxt, &decode); exr_finish(&rctxt); return fail("OpenEXRCore pixel decode", failed); }
		r = exr_decoding_initialize(rctxt, 0, &cinfo, &decode);
		if(r != EXR_ERR_SUCCESS) { exr_decoding_destroy(rctxt, &decode); exr_finish(&rctxt); return fail("OpenEXRCore pixel decode", failed); }
		if(decode.channel_count != 1 || !decode.channels || !decode.channels[0].channel_name || strcmp(decode.channels[0].channel_name, "Y") != 0) {
			exr_decoding_destroy(rctxt, &decode);
			exr_finish(&rctxt);
			return fail("OpenEXRCore pixel decode", failed);
		}
		setup_decode_channel(decode.channels[0], &output[y * 2]);
		r = exr_decoding_choose_default_routines(rctxt, 0, &decode);
		if(r != EXR_ERR_SUCCESS) { exr_decoding_destroy(rctxt, &decode); exr_finish(&rctxt); return fail("OpenEXRCore pixel decode", failed); }
		r = exr_decoding_run(rctxt, 0, &decode);
		if(r != EXR_ERR_SUCCESS) { exr_decoding_destroy(rctxt, &decode); exr_finish(&rctxt); return fail("OpenEXRCore pixel decode", failed); }
		r = exr_decoding_destroy(rctxt, &decode);
		if(r != EXR_ERR_SUCCESS) { exr_finish(&rctxt); return fail("OpenEXRCore pixel decode", failed); }
	}

	r = exr_finish(&rctxt);
	if(r != EXR_ERR_SUCCESS) return fail("OpenEXRCore pixel decode", failed);
	check_ok("OpenEXRCore read header", passed);
	check_ok("OpenEXRCore pixel decode", passed);

	if(output[0] != input[0] || output[1] != input[1] || output[2] != input[2] || output[3] != input[3]) {
		printf("OpenEXRCore pixels verified: FAIL\n");
		++failed;
	} else {
		printf("OpenEXRCore pixels verified: OK\n");
		++passed;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
