#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <openexr_core/openexr.h>

struct Pixel
{
	float r;
	float g;
	float b;
	float a;
};

static int fail_call(const char* label, exr_result_t r, int& failed)
{
	printf("%s: FAIL (%d) %s\n", label, (int)r, exr_get_error_code_as_string(r));
	++failed;
	return 1;
}

static int fail_msg(const char* label, int& failed)
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

static Pixel make_pixel(int x, int y)
{
	static const float vals[5] = {0.0f, 0.25f, 0.5f, 1.0f, 2.0f};
	Pixel p;
	p.r = vals[(x + y) % 5];
	p.g = vals[(x + y + 1) % 5];
	p.b = vals[(x + y + 2) % 5];
	p.a = vals[(x + y + 3) % 5];
	return p;
}

static bool match_channel(const char* name, int& r, int& g, int& b, int& a)
{
	if(strcmp(name, "R") == 0) { if(r >= 0) return false; r = 0; return true; }
	if(strcmp(name, "G") == 0) { if(g >= 0) return false; g = 1; return true; }
	if(strcmp(name, "B") == 0) { if(b >= 0) return false; b = 2; return true; }
	if(strcmp(name, "A") == 0) { if(a >= 0) return false; a = 3; return true; }
	return false;
}

static int map_encode_channels(exr_encode_pipeline_t* encode, const Pixel* base, int width, int height, int& failed)
{
	if(encode->channel_count != 4 || !encode->channels)
		return fail_msg("OpenEXRCore RGBA write header", failed);

	int rm = -1, gm = -1, bm = -1, am = -1;
	for(int i = 0; i < encode->channel_count; ++i) {
		const char* name = encode->channels[i].channel_name;
		if(!name || !match_channel(name, rm, gm, bm, am))
			return fail_msg("OpenEXRCore RGBA write header", failed);
	}
	if(rm < 0 || gm < 0 || bm < 0 || am < 0)
		return fail_msg("OpenEXRCore RGBA write header", failed);

	const size_t stride = sizeof(Pixel);
	const size_t lstride = (size_t)width * sizeof(Pixel);
	for(int i = 0; i < encode->channel_count; ++i) {
		const char* name = encode->channels[i].channel_name;
		const uint8_t* ptr = reinterpret_cast<const uint8_t*>(base);
		if(strcmp(name, "R") == 0) ptr += offsetof(Pixel, r);
		else if(strcmp(name, "G") == 0) ptr += offsetof(Pixel, g);
		else if(strcmp(name, "B") == 0) ptr += offsetof(Pixel, b);
		else if(strcmp(name, "A") == 0) ptr += offsetof(Pixel, a);
		encode->channels[i].user_data_type = EXR_PIXEL_FLOAT;
		encode->channels[i].user_bytes_per_element = (int16_t)sizeof(float);
		encode->channels[i].user_pixel_stride = (int32_t)stride;
		encode->channels[i].user_line_stride = (int32_t)lstride;
		encode->channels[i].encode_from_ptr = ptr;
	}
	(void)height;
	return 0;
}

static int map_decode_channels(exr_decode_pipeline_t* decode, Pixel* base, int width, int height, int& failed)
{
	if(decode->channel_count != 4 || !decode->channels)
		return fail_msg("OpenEXRCore RGBA read header", failed);

	int rm = -1, gm = -1, bm = -1, am = -1;
	for(int i = 0; i < decode->channel_count; ++i) {
		const char* name = decode->channels[i].channel_name;
		if(!name || !match_channel(name, rm, gm, bm, am))
			return fail_msg("OpenEXRCore RGBA read header", failed);
	}
	if(rm < 0 || gm < 0 || bm < 0 || am < 0)
		return fail_msg("OpenEXRCore RGBA read header", failed);

	const size_t stride = sizeof(Pixel);
	const size_t lstride = (size_t)width * sizeof(Pixel);
	for(int i = 0; i < decode->channel_count; ++i) {
		const char* name = decode->channels[i].channel_name;
		uint8_t* ptr = reinterpret_cast<uint8_t*>(base);
		if(strcmp(name, "R") == 0) ptr += offsetof(Pixel, r);
		else if(strcmp(name, "G") == 0) ptr += offsetof(Pixel, g);
		else if(strcmp(name, "B") == 0) ptr += offsetof(Pixel, b);
		else if(strcmp(name, "A") == 0) ptr += offsetof(Pixel, a);
		decode->channels[i].user_data_type = EXR_PIXEL_FLOAT;
		decode->channels[i].user_bytes_per_element = (int16_t)sizeof(float);
		decode->channels[i].user_pixel_stride = (int32_t)stride;
		decode->channels[i].user_line_stride = (int32_t)lstride;
		decode->channels[i].decode_to_ptr = ptr;
	}
	(void)height;
	return 0;
}

int main()
{
	int passed = 0;
	int failed = 0;
	const char* path = "E:/apps/github/upp_imaging/out/openexr_core_rgba_zip.exr";
	const int width = 4;
	const int height = 4;
	Pixel input[height][width] = {};
	Pixel output[height][width] = {};

	for(int y = 0; y < height; ++y)
		for(int x = 0; x < width; ++x)
			input[y][x] = make_pixel(x, y);

	check_ok("OpenEXRCore user package", passed);

	exr_context_t wctxt = 0;
	exr_result_t r = exr_start_write(&wctxt, path, EXR_WRITE_FILE_DIRECTLY, 0);
	if(r != EXR_ERR_SUCCESS || !wctxt)
		return fail_call("OpenEXRCore RGBA write context", r, failed);

	int part_index = -1;
	r = exr_add_part(wctxt, 0, EXR_STORAGE_SCANLINE, &part_index);
	if(r != EXR_ERR_SUCCESS || part_index != 0)
	{
		exr_finish(&wctxt);
		return fail_call("OpenEXRCore RGBA write context", r, failed);
	}
	r = exr_initialize_required_attr_simple(wctxt, part_index, width, height, EXR_COMPRESSION_ZIP);
	if(r != EXR_ERR_SUCCESS)
	{
		exr_finish(&wctxt);
		return fail_call("OpenEXRCore RGBA write context", r, failed);
	}
	r = exr_add_channel(wctxt, part_index, "R", EXR_PIXEL_HALF, EXR_PERCEPTUALLY_LINEAR, 1, 1);
	if(r != EXR_ERR_SUCCESS) { exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA write context", r, failed); }
	r = exr_add_channel(wctxt, part_index, "G", EXR_PIXEL_HALF, EXR_PERCEPTUALLY_LINEAR, 1, 1);
	if(r != EXR_ERR_SUCCESS) { exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA write context", r, failed); }
	r = exr_add_channel(wctxt, part_index, "B", EXR_PIXEL_HALF, EXR_PERCEPTUALLY_LINEAR, 1, 1);
	if(r != EXR_ERR_SUCCESS) { exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA write context", r, failed); }
	r = exr_add_channel(wctxt, part_index, "A", EXR_PIXEL_HALF, EXR_PERCEPTUALLY_LINEAR, 1, 1);
	if(r != EXR_ERR_SUCCESS) { exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA write context", r, failed); }
	r = exr_write_header(wctxt);
	if(r != EXR_ERR_SUCCESS) { exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA write header", r, failed); }
	check_ok("OpenEXRCore RGBA write header", passed);

	for(int y = 0; y < height; ) {
		exr_chunk_info_t cinfo;
		exr_encode_pipeline_t encode = EXR_ENCODE_PIPELINE_INITIALIZER;
		r = exr_write_scanline_chunk_info(wctxt, part_index, y, &cinfo);
		if(r != EXR_ERR_SUCCESS) { exr_encoding_destroy(wctxt, &encode); exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA ZIP encode", r, failed); }
		r = exr_encoding_initialize(wctxt, part_index, &cinfo, &encode);
		if(r != EXR_ERR_SUCCESS) { exr_encoding_destroy(wctxt, &encode); exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA ZIP encode", r, failed); }
		if(map_encode_channels(&encode, &input[y][0], width, height, failed) != 0) { exr_encoding_destroy(wctxt, &encode); exr_finish(&wctxt); return 1; }
		r = exr_encoding_choose_default_routines(wctxt, part_index, &encode);
		if(r != EXR_ERR_SUCCESS) { exr_encoding_destroy(wctxt, &encode); exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA ZIP encode", r, failed); }
		r = exr_encoding_run(wctxt, part_index, &encode);
		if(r != EXR_ERR_SUCCESS) { exr_encoding_destroy(wctxt, &encode); exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA ZIP encode", r, failed); }
		r = exr_encoding_destroy(wctxt, &encode);
		if(r != EXR_ERR_SUCCESS) { exr_finish(&wctxt); return fail_call("OpenEXRCore RGBA ZIP encode", r, failed); }
		y += cinfo.height;
	}
	check_ok("OpenEXRCore RGBA ZIP encode", passed);
	r = exr_finish(&wctxt);
	if(r != EXR_ERR_SUCCESS) return fail_call("OpenEXRCore RGBA ZIP encode", r, failed);

	exr_context_t rctxt = 0;
	r = exr_start_read(&rctxt, path, 0);
	if(r != EXR_ERR_SUCCESS || !rctxt) return fail_call("OpenEXRCore RGBA read header", r, failed);

	int count = 0;
	r = exr_get_count(rctxt, &count);
	if(r != EXR_ERR_SUCCESS || count != 1) { exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA read header", r, failed); }
	exr_storage_t storage = EXR_STORAGE_UNKNOWN;
	r = exr_get_storage(rctxt, 0, &storage);
	if(r != EXR_ERR_SUCCESS || storage != EXR_STORAGE_SCANLINE) { exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA read header", r, failed); }
	exr_compression_t compression = EXR_COMPRESSION_LAST_TYPE;
	r = exr_get_compression(rctxt, 0, &compression);
	if(r != EXR_ERR_SUCCESS || compression != EXR_COMPRESSION_ZIP) { exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA read header", r, failed); }
	exr_attr_box2i_t dw;
	r = exr_get_data_window(rctxt, 0, &dw);
	if(r != EXR_ERR_SUCCESS || dw.min.x != 0 || dw.min.y != 0 || dw.max.x != 3 || dw.max.y != 3) { exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA read header", r, failed); }
	const exr_attr_chlist_t* chlist = 0;
	r = exr_get_channels(rctxt, 0, &chlist);
	if(r != EXR_ERR_SUCCESS || !chlist || chlist->num_channels != 4) { exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA read header", r, failed); }
	int have_r = 0, have_g = 0, have_b = 0, have_a = 0;
	for(int i = 0; i < chlist->num_channels; ++i) {
		const char* name = chlist->entries[i].name.str;
		if(strcmp(name, "R") == 0) have_r = 1;
		else if(strcmp(name, "G") == 0) have_g = 1;
		else if(strcmp(name, "B") == 0) have_b = 1;
		else if(strcmp(name, "A") == 0) have_a = 1;
		else { exr_finish(&rctxt); return fail_msg("OpenEXRCore RGBA read header", failed); }
	}
	if(!have_r || !have_g || !have_b || !have_a) { exr_finish(&rctxt); return fail_msg("OpenEXRCore RGBA read header", failed); }
	check_ok("OpenEXRCore RGBA read header", passed);

	int scanlines_per_chunk = 0;
	r = exr_get_scanlines_per_chunk(rctxt, 0, &scanlines_per_chunk);
	if(r != EXR_ERR_SUCCESS || scanlines_per_chunk <= 0) { exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA read header", r, failed); }
	printf("OpenEXRCore RGBA chunk height: %d\n", scanlines_per_chunk);

	for(int y = 0; y < height; ) {
		exr_chunk_info_t cinfo;
		exr_decode_pipeline_t decode = EXR_DECODE_PIPELINE_INITIALIZER;
		r = exr_read_scanline_chunk_info(rctxt, 0, y, &cinfo);
		if(r != EXR_ERR_SUCCESS) { exr_decoding_destroy(rctxt, &decode); exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA ZIP decode", r, failed); }
		r = exr_decoding_initialize(rctxt, 0, &cinfo, &decode);
		if(r != EXR_ERR_SUCCESS) { exr_decoding_destroy(rctxt, &decode); exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA ZIP decode", r, failed); }
		if(map_decode_channels(&decode, &output[y][0], width, height, failed) != 0) { exr_decoding_destroy(rctxt, &decode); exr_finish(&rctxt); return 1; }
		r = exr_decoding_choose_default_routines(rctxt, 0, &decode);
		if(r != EXR_ERR_SUCCESS) { exr_decoding_destroy(rctxt, &decode); exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA ZIP decode", r, failed); }
		r = exr_decoding_run(rctxt, 0, &decode);
		if(r != EXR_ERR_SUCCESS) { exr_decoding_destroy(rctxt, &decode); exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA ZIP decode", r, failed); }
		r = exr_decoding_destroy(rctxt, &decode);
		if(r != EXR_ERR_SUCCESS) { exr_finish(&rctxt); return fail_call("OpenEXRCore RGBA ZIP decode", r, failed); }
		y += cinfo.height;
	}
	check_ok("OpenEXRCore RGBA ZIP decode", passed);
	r = exr_finish(&rctxt);
	if(r != EXR_ERR_SUCCESS) return fail_call("OpenEXRCore RGBA ZIP decode", r, failed);

	for(int y = 0; y < height; ++y)
		for(int x = 0; x < width; ++x) {
			const Pixel& a = input[y][x];
			const Pixel& b = output[y][x];
			if(a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a) {
				printf("OpenEXRCore RGBA pixels verified: FAIL\n");
				++failed;
				printf("SUMMARY passed=%d failed=%d\n", passed, failed);
				return 1;
			}
		}
	check_ok("OpenEXRCore RGBA pixels verified", passed);

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
