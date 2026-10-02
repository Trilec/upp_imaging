#ifndef _libpng_roundtrip_security_h_
#define _libpng_roundtrip_security_h_

#include <vector>
#include <cstdint>
#include <cstring>

// A CRC-valid malformed metadata fixture for GHSA-qvg3-h654-xq3j.
// Native Debug rejection is regression evidence, not an ASan memory-safety claim.
static void SecurityBE32(std::vector<unsigned char>& out, uint32_t value)
{
	for(int shift = 24; shift >= 0; shift -= 8) out.push_back(value >> shift);
}
static void SecurityChunk(std::vector<unsigned char>& out, const char* type,
                          const std::vector<unsigned char>& data)
{
	SecurityBE32(out, (uint32_t)data.size());
	size_t start = out.size();
	out.insert(out.end(), type, type + 4);
	out.insert(out.end(), data.begin(), data.end());
	uint32_t crc = 0xffffffffu;
	for(size_t i = start; i < out.size(); ++i) {
		crc ^= out[i];
		for(int bit = 0; bit < 8; ++bit)
			crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
	}
	SecurityBE32(out, crc ^ 0xffffffffu);
}
struct SecurityPngInput { std::vector<unsigned char> data; size_t pos = 0; };
static void SecurityPngError(png_structp png, png_const_charp) { png_longjmp(png, 1); }
static void SecurityPngWarning(png_structp, png_const_charp) {}
static void SecurityPngRead(png_structp png, png_bytep data, png_size_t length)
{
	auto* in = (SecurityPngInput*)png_get_io_ptr(png);
	if(length > in->data.size() - in->pos) png_error(png, "fixture ended");
	memcpy(data, in->data.data() + in->pos, length);
	in->pos += length;
}
static bool SecurityPngAbandonedMetadata(const unsigned char* normal, size_t length)
{
	if(length < 33) return false;
	SecurityPngInput in;
	in.data.assign(normal, normal + 33); // signature and IHDR
	SecurityChunk(in.data, "zTXt", {'k', 0, 0, 0x88, 0x1c, 0x11, 0x22, 0x33});
	std::vector<unsigned char> text(4096, 'a'); text[0] = 'k'; text[1] = 0;
	SecurityChunk(in.data, "tEXt", text); // reallocates the prior chunk buffer
	in.data.insert(in.data.end(), normal + 33, normal + length);
	bool rejected = false;
	volatile bool metadata_read = false;
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
	                                      SecurityPngError, SecurityPngWarning);
	png_infop info = png ? png_create_info_struct(png) : nullptr;
	if(!png || !info) { png_destroy_read_struct(&png, &info, nullptr); return false; }
	if(setjmp(png_jmpbuf(png))) rejected = metadata_read;
	else {
		png_set_read_fn(png, &in, SecurityPngRead);
		png_read_info(png, info);
		metadata_read = true;
		png_read_end(png, info); // deliberately skip rows: advisory's call sequence
	}
	png_destroy_read_struct(&png, &info, nullptr);
	return rejected;
}

#endif
