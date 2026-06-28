#include <stdio.h>
#include <string.h>

#include <zlib_src/zlib.h>

int main()
{
	int passed = 0;
	int failed = 0;

	const char *version = zlibVersion();
	printf("zlib source version: %s\n", version ? version : "(null)");
	if(version && strcmp(version, "1.3.2") == 0)
		passed++;
	else
		failed++;

	const char source[] = "upp_imaging strict zlib source test";
	uLong source_len = (uLong)strlen(source);
	uLongf compressed_len = compressBound(source_len);
	unsigned char compressed[256];
	unsigned char restored[256];

	int rc = compress(compressed, &compressed_len, (const Bytef *)source, source_len);
	if(rc == Z_OK) {
		uLongf restored_len = source_len;
		rc = uncompress(restored, &restored_len, compressed, compressed_len);
		if(rc == Z_OK && restored_len == source_len && memcmp(restored, source, (size_t)source_len) == 0) {
			printf("compress/uncompress round trip: OK\n");
			passed++;
		}
		else {
			printf("compress/uncompress round trip: FAIL\n");
			failed++;
		}
	}
	else {
		printf("compress/uncompress round trip: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
