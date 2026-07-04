#include "OpenExrIO.h"

#include <stddef.h>
#include <string.h>

using namespace Upp;

static bool SetError(String* error, const char* op, exr_result_t r)
{
	if(error)
		*error = Format("%s failed (%d): %s", op, (int)r, exr_get_error_code_as_string(r));
	return false;
}

static bool SetError(String* error, const char* text)
{
	if(error)
		*error = text;
	return false;
}

static bool ValidateImage(const ExrRgbaImageF& image, String* error)
{
	if(image.width <= 0 || image.height <= 0)
		return SetError(error, "image dimensions must be positive");
	if(image.pixels.GetCount() != image.width * image.height)
		return SetError(error, "pixel count does not match width*height");
	return true;
}

static bool ChannelSeen(const char* name, bool& r, bool& g, bool& b, bool& a)
{
	if(strcmp(name, "R") == 0) { if(r) return false; r = true; return true; }
	if(strcmp(name, "G") == 0) { if(g) return false; g = true; return true; }
	if(strcmp(name, "B") == 0) { if(b) return false; b = true; return true; }
	if(strcmp(name, "A") == 0) { if(a) return false; a = true; return true; }
	return false;
}

static bool AssignEncodeChannels(exr_encode_pipeline_t& encode, const ExrRgbaImageF& image, int chunk_y, String* error)
{
	bool seen_r = false, seen_g = false, seen_b = false, seen_a = false;
	for(int i = 0; i < encode.channel_count; ++i) {
		const char* name = encode.channels[i].channel_name;
		if(!name)
			return SetError(error, "missing channel name in encode pipeline");
		if(!ChannelSeen(name, seen_r, seen_g, seen_b, seen_a))
			return SetError(error, "unexpected or duplicate channel in encode pipeline");
		const uint8_t* base = reinterpret_cast<const uint8_t*>(&image.pixels[chunk_y * image.width]);
		if(strcmp(name, "R") == 0)
			base += offsetof(ExrRgbaF, r);
		else if(strcmp(name, "G") == 0)
			base += offsetof(ExrRgbaF, g);
		else if(strcmp(name, "B") == 0)
			base += offsetof(ExrRgbaF, b);
		else if(strcmp(name, "A") == 0)
			base += offsetof(ExrRgbaF, a);
		encode.channels[i].user_data_type = EXR_PIXEL_FLOAT;
		encode.channels[i].user_bytes_per_element = (int16_t)sizeof(float);
		encode.channels[i].user_pixel_stride = (int32_t)sizeof(ExrRgbaF);
		encode.channels[i].user_line_stride = (int32_t)(image.width * (int)sizeof(ExrRgbaF));
		encode.channels[i].encode_from_ptr = base;
	}
	if(!seen_r || !seen_g || !seen_b || !seen_a)
		return SetError(error, "missing required RGBA channel in encode pipeline");
	return true;
}

static bool AssignDecodeChannels(exr_decode_pipeline_t& decode, ExrRgbaImageF& image, int chunk_y, String* error)
{
	bool seen_r = false, seen_g = false, seen_b = false, seen_a = false;
	for(int i = 0; i < decode.channel_count; ++i) {
		const char* name = decode.channels[i].channel_name;
		if(!name)
			return SetError(error, "missing channel name in decode pipeline");
		if(strcmp(name, "R") != 0 && strcmp(name, "G") != 0 && strcmp(name, "B") != 0 && strcmp(name, "A") != 0)
			return SetError(error, "unexpected channel in decode pipeline");
		if(strcmp(name, "R") == 0) { if(seen_r) return SetError(error, "duplicate channel in decode pipeline"); seen_r = true; }
		else if(strcmp(name, "G") == 0) { if(seen_g) return SetError(error, "duplicate channel in decode pipeline"); seen_g = true; }
		else if(strcmp(name, "B") == 0) { if(seen_b) return SetError(error, "duplicate channel in decode pipeline"); seen_b = true; }
		else if(strcmp(name, "A") == 0) { if(seen_a) return SetError(error, "duplicate channel in decode pipeline"); seen_a = true; }
		uint8_t* base = reinterpret_cast<uint8_t*>(&image.pixels[chunk_y * image.width]);
		if(strcmp(name, "R") == 0)
			base += offsetof(ExrRgbaF, r);
		else if(strcmp(name, "G") == 0)
			base += offsetof(ExrRgbaF, g);
		else if(strcmp(name, "B") == 0)
			base += offsetof(ExrRgbaF, b);
		else if(strcmp(name, "A") == 0)
			base += offsetof(ExrRgbaF, a);
		decode.channels[i].user_data_type = EXR_PIXEL_FLOAT;
		decode.channels[i].user_bytes_per_element = (int16_t)sizeof(float);
		decode.channels[i].user_pixel_stride = (int32_t)sizeof(ExrRgbaF);
		decode.channels[i].user_line_stride = (int32_t)(image.width * (int)sizeof(ExrRgbaF));
		decode.channels[i].decode_to_ptr = base;
	}
	if(!seen_r || !seen_g || !seen_b)
		return SetError(error, "missing required RGB channel in decode pipeline");
	return true;
}

bool SaveExrRgbaF(const char* path, const ExrRgbaImageF& image, bool output_half, bool use_zip, String* error)
{
	if(!path || !*path)
		return SetError(error, "path is empty");
	if(!ValidateImage(image, error))
		return false;

	exr_context_t ctxt = 0;
	exr_result_t r = exr_start_write(&ctxt, path, EXR_WRITE_FILE_DIRECTLY, 0);
	if(r != EXR_ERR_SUCCESS || !ctxt)
		return SetError(error, "exr_start_write", r);

	int part_index = -1;
	r = exr_add_part(ctxt, 0, EXR_STORAGE_SCANLINE, &part_index);
	if(r != EXR_ERR_SUCCESS) {
		exr_finish(&ctxt);
		return SetError(error, "exr_add_part", r);
	}

	r = exr_initialize_required_attr_simple(ctxt, part_index, image.width, image.height,
		use_zip ? EXR_COMPRESSION_ZIP : EXR_COMPRESSION_NONE);
	if(r != EXR_ERR_SUCCESS) {
		exr_finish(&ctxt);
		return SetError(error, "exr_initialize_required_attr_simple", r);
	}

	const exr_pixel_type_t file_type = output_half ? EXR_PIXEL_HALF : EXR_PIXEL_FLOAT;
	for(const char* name : {"R", "G", "B", "A"}) {
		r = exr_add_channel(ctxt, part_index, name, file_type, EXR_PERCEPTUALLY_LINEAR, 1, 1);
		if(r != EXR_ERR_SUCCESS) {
			exr_finish(&ctxt);
			return SetError(error, "exr_add_channel", r);
		}
	}

	r = exr_write_header(ctxt);
	if(r != EXR_ERR_SUCCESS) {
		exr_finish(&ctxt);
		return SetError(error, "exr_write_header", r);
	}

	for(int y = 0; y < image.height; ) {
		exr_chunk_info_t cinfo;
		r = exr_write_scanline_chunk_info(ctxt, part_index, y, &cinfo);
		if(r != EXR_ERR_SUCCESS) {
			exr_finish(&ctxt);
			return SetError(error, "exr_write_scanline_chunk_info", r);
		}
		exr_encode_pipeline_t encode = EXR_ENCODE_PIPELINE_INITIALIZER;
		r = exr_encoding_initialize(ctxt, part_index, &cinfo, &encode);
		if(r != EXR_ERR_SUCCESS) {
			exr_encoding_destroy(ctxt, &encode);
			exr_finish(&ctxt);
			return SetError(error, "exr_encoding_initialize", r);
		}
		if(!AssignEncodeChannels(encode, image, y, error)) {
			exr_encoding_destroy(ctxt, &encode);
			exr_finish(&ctxt);
			return false;
		}
		r = exr_encoding_choose_default_routines(ctxt, part_index, &encode);
		if(r != EXR_ERR_SUCCESS) {
			exr_encoding_destroy(ctxt, &encode);
			exr_finish(&ctxt);
			return SetError(error, "exr_encoding_choose_default_routines", r);
		}
		r = exr_encoding_run(ctxt, part_index, &encode);
		if(r != EXR_ERR_SUCCESS) {
			exr_encoding_destroy(ctxt, &encode);
			exr_finish(&ctxt);
			return SetError(error, "exr_encoding_run", r);
		}
		r = exr_encoding_destroy(ctxt, &encode);
		if(r != EXR_ERR_SUCCESS) {
			exr_finish(&ctxt);
			return SetError(error, "exr_encoding_destroy", r);
		}
		y += cinfo.height;
	}

	r = exr_finish(&ctxt);
	if(r != EXR_ERR_SUCCESS)
		return SetError(error, "exr_finish", r);
	return true;
}

bool LoadExrRgbaF(const char* path, ExrRgbaImageF& image, String* error)
{
	if(!path || !*path)
		return SetError(error, "path is empty");

	exr_context_t ctxt = 0;
	exr_result_t r = exr_start_read(&ctxt, path, 0);
	if(r != EXR_ERR_SUCCESS || !ctxt)
		return SetError(error, "exr_start_read", r);

	int count = 0;
	r = exr_get_count(ctxt, &count);
	if(r != EXR_ERR_SUCCESS || count != 1) {
		exr_finish(&ctxt);
		return SetError(error, "exr_get_count", r);
	}

	exr_storage_t storage = EXR_STORAGE_UNKNOWN;
	r = exr_get_storage(ctxt, 0, &storage);
	if(r != EXR_ERR_SUCCESS || storage != EXR_STORAGE_SCANLINE) {
		exr_finish(&ctxt);
		return SetError(error, "exr_get_storage", r);
	}

	exr_attr_box2i_t dw;
	r = exr_get_data_window(ctxt, 0, &dw);
	if(r != EXR_ERR_SUCCESS) {
		exr_finish(&ctxt);
		return SetError(error, "exr_get_data_window", r);
	}

	image.width = (int)(dw.max.x - dw.min.x + 1);
	image.height = (int)(dw.max.y - dw.min.y + 1);
	if(image.width <= 0 || image.height <= 0) {
		exr_finish(&ctxt);
		return SetError(error, "invalid data window dimensions");
	}
	image.pixels.SetCount(image.width * image.height);
	for(ExrRgbaF& p : image.pixels) {
		p.r = 0.0f;
		p.g = 0.0f;
		p.b = 0.0f;
		p.a = 1.0f;
	}

	for(int y = 0; y < image.height; ) {
		exr_chunk_info_t cinfo;
		r = exr_read_scanline_chunk_info(ctxt, 0, y, &cinfo);
		if(r != EXR_ERR_SUCCESS) {
			exr_finish(&ctxt);
			return SetError(error, "exr_read_scanline_chunk_info", r);
		}
		exr_decode_pipeline_t decode = EXR_DECODE_PIPELINE_INITIALIZER;
		r = exr_decoding_initialize(ctxt, 0, &cinfo, &decode);
		if(r != EXR_ERR_SUCCESS) {
			exr_decoding_destroy(ctxt, &decode);
			exr_finish(&ctxt);
			return SetError(error, "exr_decoding_initialize", r);
		}
		if(!AssignDecodeChannels(decode, image, y, error)) {
			exr_decoding_destroy(ctxt, &decode);
			exr_finish(&ctxt);
			return false;
		}
		r = exr_decoding_choose_default_routines(ctxt, 0, &decode);
		if(r != EXR_ERR_SUCCESS) {
			exr_decoding_destroy(ctxt, &decode);
			exr_finish(&ctxt);
			return SetError(error, "exr_decoding_choose_default_routines", r);
		}
		r = exr_decoding_run(ctxt, 0, &decode);
		if(r != EXR_ERR_SUCCESS) {
			exr_decoding_destroy(ctxt, &decode);
			exr_finish(&ctxt);
			return SetError(error, "exr_decoding_run", r);
		}
		r = exr_decoding_destroy(ctxt, &decode);
		if(r != EXR_ERR_SUCCESS) {
			exr_finish(&ctxt);
			return SetError(error, "exr_decoding_destroy", r);
		}
		y += cinfo.height;
	}

	r = exr_finish(&ctxt);
	if(r != EXR_ERR_SUCCESS)
		return SetError(error, "exr_finish", r);
	return true;
}
