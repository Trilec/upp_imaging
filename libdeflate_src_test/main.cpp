#include <stdio.h>
#include <string.h>

#include <libdeflate_src/libdeflate.h>

int main()
{
	int passed = 0;
	int failed = 0;

	const unsigned char source[] = "upp_imaging libdeflate strict test";
	size_t source_len = sizeof(source) - 1;
	unsigned char compressed[256];
	unsigned char restored[256];

	struct libdeflate_compressor *comp = libdeflate_alloc_compressor(6);
	struct libdeflate_decompressor *decomp = libdeflate_alloc_decompressor();
	if(comp && decomp) {
		printf("libdeflate source package: OK\n");
		passed++;
	}
	else {
		printf("libdeflate source package: FAIL\n");
		failed++;
	}

	if(comp && decomp) {
		size_t out_len = libdeflate_zlib_compress(comp, source, source_len, compressed, sizeof(compressed));
		size_t actual = 0;
		enum libdeflate_result r = out_len ? libdeflate_zlib_decompress(decomp, compressed, out_len, restored, source_len, &actual) : LIBDEFLATE_BAD_DATA;
		if(out_len && r == LIBDEFLATE_SUCCESS && actual == source_len && memcmp(restored, source, source_len) == 0) {
			printf("compress/decompress round trip: OK\n");
			passed++;
		}
		else {
			printf("compress/decompress round trip: FAIL\n");
			failed++;
		}
	}
	else {
		printf("compress/decompress round trip: FAIL\n");
		failed++;
	}

	libdeflate_free_compressor(comp);
	libdeflate_free_decompressor(decomp);

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
