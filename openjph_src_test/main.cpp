#include <stdio.h>

#include <openjph_src/openjph.h>

int main()
{
	int passed = 0;
	int failed = 0;

	printf("OpenJPH version: %d.%d.%d\n", OPENJPH_VERSION_MAJOR, OPENJPH_VERSION_MINOR, OPENJPH_VERSION_PATCH);
	if(OPENJPH_VERSION_MAJOR == 0 && OPENJPH_VERSION_MINOR == 26 && OPENJPH_VERSION_PATCH == 3)
		passed++;
	else
		failed++;

	ojph::size sz(2, 2);
	ojph::point pt(1, 1);
	if(sz.area() == 4 && pt.x == 1 && pt.y == 1) {
		printf("OpenJPH source package: OK\n");
		passed++;
	}
	else {
		printf("OpenJPH source package: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
