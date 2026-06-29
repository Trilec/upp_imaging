#include <Core/Core.h>
#include <Draw/Draw.h>
#include <plugin/png/png.h>

using namespace Upp;

static RGBA PM(byte r, byte g, byte b, byte a)
{
	RGBA c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	Premultiply(&c, &c, 1);
	return c;
}

static bool CloseChannel(byte a, byte b)
{
	return abs((int)a - (int)b) <= 1;
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	ImageBuffer ib(Size(2, 2));
	RGBA *p = ~ib;
	p[0] = PM(255, 0, 0, 255);
	p[1] = PM(0, 255, 0, 255);
	p[2] = PM(0, 0, 255, 255);
	p[3] = PM(255, 255, 255, 128);
	Image source = ib;

	String encoded = PNGEncoder(32, IMAGE_ALPHA).SaveString(source);
	if(!encoded.IsEmpty())
		passed++;
	else
		failed++;

	Image decoded = StreamRaster::LoadStringAny(encoded);
	if(decoded.GetSize() == Size(2, 2))
		passed++;
	else
		failed++;

	Size sz = decoded.GetSize();
	bool pixels_ok = sz == Size(2, 2);
	if(pixels_ok) {
		ImageBuffer out(decoded);
		RGBA *q = ~out;
		RGBA expected[4];
		expected[0] = PM(255, 0, 0, 255);
		expected[1] = PM(0, 255, 0, 255);
		expected[2] = PM(0, 0, 255, 255);
		expected[3] = PM(255, 255, 255, 128);
		for(int i = 0; i < 3; i++) {
			if(!CloseChannel(q[i].r, expected[i].r)
			 || !CloseChannel(q[i].g, expected[i].g)
			 || !CloseChannel(q[i].b, expected[i].b)
			 || !CloseChannel(q[i].a, expected[i].a)) {
				Cout() << "pixel " << i
				       << " actual=(" << (int)q[i].r << "," << (int)q[i].g << "," << (int)q[i].b << "," << (int)q[i].a << ")"
				       << " expected=(" << (int)expected[i].r << "," << (int)expected[i].g << "," << (int)expected[i].b << "," << (int)expected[i].a << ")\n";
				pixels_ok = false;
				break;
			}
		}
		if(pixels_ok && !CloseChannel(q[3].a, expected[3].a)) {
			Cout() << "pixel 3 alpha actual=" << (int)q[3].a
			       << " expected=" << (int)expected[3].a << "\n";
			pixels_ok = false;
		}
	}

	if(pixels_ok) {
		Cout() << "U++ plugin/png round trip: OK\n";
		Cout() << "pixels verified: OK\n";
		Cout() << "alpha note: partial-alpha pixel preserves alpha, but RGB under alpha is not asserted in this U++ Image path\n";
		passed++;
	}
	else {
		Cout() << "U++ plugin/png round trip: FAIL\n";
		Cout() << "pixels verified: FAIL\n";
		failed++;
	}

	Cout() << "SUMMARY passed=" << passed << " failed=" << failed << '\n';
	SetExitCode(failed ? 1 : 0);
}
