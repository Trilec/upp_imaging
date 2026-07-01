#include <Core/Core.h>
#include <openjph/openjph.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	Cout() << "OpenJPH version: " << OPENJPH_VERSION_MAJOR << "." << OPENJPH_VERSION_MINOR << "." << OPENJPH_VERSION_PATCH << "\n";
	if(OPENJPH_VERSION_MAJOR == 0 && OPENJPH_VERSION_MINOR == 26 && OPENJPH_VERSION_PATCH == 3)
		passed++;
	else
		failed++;

	ojph::size sz(4, 4);
	ojph::rect r;
	r.siz = sz;
	if(r.siz.area() == 16) {
		Cout() << "OpenJPH user package: OK\n";
		passed++;
	}
	else {
		Cout() << "OpenJPH user package: FAIL\n";
		failed++;
	}

	Cout() << "SUMMARY passed=" << passed << " failed=" << failed << '\n';
	SetExitCode(failed ? 1 : 0);
}
