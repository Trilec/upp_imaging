#include <stdio.h>
#include <string.h>

#include <libpng_src/png.h>

int main()
{
	int passed = 0;
	int failed = 0;

	printf("libpng compile version: %s\n", PNG_LIBPNG_VER_STRING);
	if(strcmp(PNG_LIBPNG_VER_STRING, "1.6.58") == 0)
		passed++;
	else
		failed++;

	printf("libpng runtime version: %s\n", png_libpng_ver);
	if(strcmp(png_libpng_ver, "1.6.58") == 0)
		passed++;
	else
		failed++;

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr) {
		png_destroy_write_struct(&png_ptr, NULL);
		printf("png create/destroy smoke: OK\n");
		passed++;
	}
	else {
		printf("png create/destroy smoke: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
