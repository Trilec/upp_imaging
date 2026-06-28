#include <Core/Core.h>
#include <libpng/png.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	Cout() << "libpng compile version: " << PNG_LIBPNG_VER_STRING << '\n';
	if(strcmp(PNG_LIBPNG_VER_STRING, "1.6.58") == 0)
		passed++;
	else
		failed++;

	Cout() << "libpng runtime version: " << png_libpng_ver << '\n';
	if(strcmp(png_libpng_ver, "1.6.58") == 0)
		passed++;
	else
		failed++;

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr) {
		png_destroy_write_struct(&png_ptr, NULL);
		Cout() << "png create/destroy smoke: OK\n";
		passed++;
	}
	else {
		Cout() << "png create/destroy smoke: FAIL\n";
		failed++;
	}

	Cout() << "SUMMARY passed=" << passed << " failed=" << failed << '\n';
	SetExitCode(failed ? 1 : 0);
}
