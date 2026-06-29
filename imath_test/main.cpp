#include <Core/Core.h>
#include <imath/half.h>
#include <imath/ImathBox.h>
#include <imath/ImathVec.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	Cout() << "Imath user package: OK\n";
	passed++;

	Imath::half h = 2.25f;
	float hf = float(h);
	if(fabsf(hf - 2.25f) < 0.001f) {
		Cout() << "half conversion: OK\n";
		passed++;
	}
	else {
		Cout() << "half conversion: FAIL\n";
		failed++;
	}

	Imath::V3f a(2.0f, 0.0f, -1.0f);
	Imath::V3f b(1.0f, 3.0f, 5.0f);
	Imath::V3f sum = a + b;
	if(sum.x == 3.0f && sum.y == 3.0f && sum.z == 4.0f) {
		Cout() << "V3f math: OK\n";
		passed++;
	}
	else {
		Cout() << "V3f math: FAIL\n";
		failed++;
	}

	Imath::Box2i box(Imath::V2i(0, 0), Imath::V2i(7, 9));
	if(box.max.x == 7 && box.max.y == 9) {
		Cout() << "Box smoke: OK\n";
		passed++;
	}
	else {
		Cout() << "Box smoke: FAIL\n";
		failed++;
	}

	Cout() << "SUMMARY passed=" << passed << " failed=" << failed << '\n';
	SetExitCode(failed ? 1 : 0);
}
