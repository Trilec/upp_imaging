#include <stdio.h>

#include <openexr_core_src/openexr.h>

int main()
{
	int passed = 0;
	int failed = 0;

	int maj = 0, min = 0, patch = 0;
	const char *extra = 0;
	exr_get_library_version(&maj, &min, &patch, &extra);
	printf("OpenEXRCore version: %d.%d.%d\n", maj, min, patch);
	if(maj == 3 && min == 4 && patch == 13)
		passed++;
	else
		failed++;

	if(EXR_COMPRESSION_ZIP == 3 && EXR_COMPRESSION_ZIPS == 2 && EXR_COMPRESSION_HTJ2K32 == 11 && EXR_COMPRESSION_HTJ2K256 == 10) {
		printf("OpenEXRCore compression enums: OK\n");
		passed++;
	}
	else {
		printf("OpenEXRCore compression enums: FAIL\n");
		failed++;
	}

	printf("OpenEXRCore public headers: OK\n");
	passed++;

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
