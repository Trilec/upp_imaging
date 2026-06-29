#include <stdio.h>
#include <string.h>
#include <vector>

#include <libpng_src/png.h>

struct MemBuffer {
	std::vector<unsigned char> data;
	size_t pos = 0;
};

static void PngWrite(png_structp png_ptr, png_bytep data, png_size_t length)
{
	MemBuffer *buffer = (MemBuffer *)png_get_io_ptr(png_ptr);
	buffer->data.insert(buffer->data.end(), data, data + length);
}

static void PngFlush(png_structp)
{
}

static void PngRead(png_structp png_ptr, png_bytep data, png_size_t length)
{
	MemBuffer *buffer = (MemBuffer *)png_get_io_ptr(png_ptr);
	if(buffer->pos + length > buffer->data.size())
		png_error(png_ptr, "read past end of buffer");
	memcpy(data, buffer->data.data() + buffer->pos, length);
	buffer->pos += length;
}

int main()
{
	int passed = 0;
	int failed = 0;

	const unsigned char source_pixels[16] = {
		255, 0, 0, 255,
		0, 255, 0, 255,
		0, 0, 255, 255,
		255, 255, 255, 128,
	};

	MemBuffer encoded;
	png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop write_info = write_ptr ? png_create_info_struct(write_ptr) : NULL;
	if(write_ptr && write_info && !setjmp(png_jmpbuf(write_ptr))) {
		png_set_write_fn(write_ptr, &encoded, PngWrite, PngFlush);
		png_set_IHDR(write_ptr, write_info, 2, 2, 8, PNG_COLOR_TYPE_RGBA,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		png_write_info(write_ptr, write_info);
		png_bytep rows[2];
		rows[0] = (png_bytep)source_pixels;
		rows[1] = (png_bytep)(source_pixels + 8);
		png_write_image(write_ptr, rows);
		png_write_end(write_ptr, write_info);
		passed++;
	}
	else {
		failed++;
	}
	png_destroy_write_struct(&write_ptr, &write_info);

	int width = 0;
	int height = 0;
	int bit_depth = 0;
	int color_type = 0;
	unsigned char decoded_pixels[16] = { 0 };
	MemBuffer input = encoded;
	png_structp read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop read_info = read_ptr ? png_create_info_struct(read_ptr) : NULL;
	if(read_ptr && read_info && !setjmp(png_jmpbuf(read_ptr))) {
		png_set_read_fn(read_ptr, &input, PngRead);
		png_read_info(read_ptr, read_info);
		width = (int)png_get_image_width(read_ptr, read_info);
		height = (int)png_get_image_height(read_ptr, read_info);
		bit_depth = png_get_bit_depth(read_ptr, read_info);
		color_type = png_get_color_type(read_ptr, read_info);
		if(color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(read_ptr);
		if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand_gray_1_2_4_to_8(read_ptr);
		if(png_get_valid(read_ptr, read_info, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(read_ptr);
		if(!(color_type & PNG_COLOR_MASK_ALPHA))
			png_set_add_alpha(read_ptr, 0xFF, PNG_FILLER_AFTER);
		if(bit_depth == 16)
			png_set_strip_16(read_ptr);
		if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(read_ptr);
		png_read_update_info(read_ptr, read_info);
		png_bytep rows[2];
		rows[0] = decoded_pixels;
		rows[1] = decoded_pixels + 8;
		png_read_image(read_ptr, rows);
		png_read_end(read_ptr, read_info);
		if(width == 2 && height == 2 && png_get_bit_depth(read_ptr, read_info) == 8)
			passed++;
		else
			failed++;
		if(memcmp(decoded_pixels, source_pixels, sizeof(source_pixels)) == 0) {
			printf("libpng_src round trip: OK\n");
			printf("pixels verified: OK\n");
			passed++;
		}
		else {
			printf("libpng_src round trip: FAIL\n");
			printf("pixels verified: FAIL\n");
			failed++;
		}
	}
	else {
		printf("libpng_src round trip: FAIL\n");
		printf("pixels verified: FAIL\n");
		failed += 2;
	}
	png_destroy_read_struct(&read_ptr, &read_info, NULL);

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
