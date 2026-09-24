#include <Core/Core.h>
#include <libpng/png.h>

using namespace Upp;

struct MemBuffer {
	Vector<byte> data;
	int pos = 0;
};

static void PngWrite(png_structp png_ptr, png_bytep data, png_size_t length)
{
	MemBuffer *buffer = (MemBuffer *)png_get_io_ptr(png_ptr);
	int old_count = buffer->data.GetCount();
	buffer->data.SetCount(old_count + (int)length);
	memcpy(buffer->data.Begin() + old_count, data, (size_t)length);
}

static void PngFlush(png_structp)
{
}

static void PngRead(png_structp png_ptr, png_bytep data, png_size_t length)
{
	MemBuffer *buffer = (MemBuffer *)png_get_io_ptr(png_ptr);
	if(buffer->pos + (int)length > buffer->data.GetCount()) {
		png_error(png_ptr, "read past end of buffer");
	}
	memcpy(data, buffer->data.Begin() + buffer->pos, (size_t)length);
	buffer->pos += (int)length;
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	const byte source_pixels[16] = {
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

	byte decoded_pixels[16] = { 0 };
	MemBuffer input;
	input.data <<= encoded.data;
	input.pos = 0;
	png_structp read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop read_info = read_ptr ? png_create_info_struct(read_ptr) : NULL;
	if(read_ptr && read_info && !setjmp(png_jmpbuf(read_ptr))) {
		png_set_read_fn(read_ptr, &input, PngRead);
		png_read_info(read_ptr, read_info);
		int width = (int)png_get_image_width(read_ptr, read_info);
		int height = (int)png_get_image_height(read_ptr, read_info);
		int bit_depth = png_get_bit_depth(read_ptr, read_info);
		int color_type = png_get_color_type(read_ptr, read_info);
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
			Cout() << "libpng round trip: OK\n";
			Cout() << "pixels verified: OK\n";
			passed++;
		}
		else {
			Cout() << "libpng round trip: FAIL\n";
			Cout() << "pixels verified: FAIL\n";
			failed++;
		}
	}
	else {
		Cout() << "libpng round trip: FAIL\n";
		Cout() << "pixels verified: FAIL\n";
		failed += 2;
	}
	png_destroy_read_struct(&read_ptr, &read_info, NULL);

	Cout() << "SUMMARY passed=" << passed << " failed=" << failed << '\n';
	SetExitCode(failed ? 1 : 0);
}
