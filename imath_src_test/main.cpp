#include <math.h>
#include <stdio.h>

#include <imath_src/half.h>
#include <imath_src/ImathBox.h>
#include <imath_src/ImathMatrix.h>
#include <imath_src/ImathVec.h>

int main()
{
	int passed = 0;
	int failed = 0;

	printf("Imath version: %s\n", IMATH_VERSION_STRING);
	if(IMATH_VERSION_MAJOR == 3 && IMATH_VERSION_MINOR == 2 && IMATH_VERSION_PATCH == 2)
		passed++;
	else
		failed++;

	Imath::half h = 1.5f;
	float hf = float(h);
	if(fabsf(hf - 1.5f) < 0.001f) {
		printf("half conversion: OK\n");
		passed++;
	}
	else {
		printf("half conversion: FAIL\n");
		failed++;
	}

	Imath::V3f a(1.0f, 2.0f, 3.0f);
	Imath::V3f b(4.0f, 5.0f, 6.0f);
	Imath::V3f sum = a + b;
	Imath::V3f cross = a.cross(b);
	float dot = a.dot(b);
	if(sum.x == 5.0f && sum.y == 7.0f && sum.z == 9.0f && cross.x == -3.0f && cross.y == 6.0f && cross.z == -3.0f && dot == 32.0f) {
		printf("V3f math: OK\n");
		passed++;
	}
	else {
		printf("V3f math: FAIL\n");
		failed++;
	}

	Imath::Box2i box(Imath::V2i(1, 2), Imath::V2i(3, 4));
	Imath::M44f m;
		m.makeIdentity();
	if(box.min.x == 1 && box.min.y == 2 && box.max.x == 3 && box.max.y == 4 && m[0][0] == 1.0f && m[1][1] == 1.0f && m[2][2] == 1.0f && m[3][3] == 1.0f) {
		printf("Box/M44 smoke: OK\n");
		passed++;
	}
	else {
		printf("Box/M44 smoke: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
