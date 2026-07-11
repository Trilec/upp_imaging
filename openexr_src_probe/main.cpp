#include <stdio.h>

#include <openexr_src/Imf.h>

#ifndef UPP_IMAGING_LOCAL_OPENEXR_SOURCE_INCLUDE
#error local OpenEXR source package not selected
#endif

int main()
{
	int passed = 0;
	int failed = 0;

	printf("OpenEXR version: %d.%d.%d\n", OPENEXR_VERSION_MAJOR, OPENEXR_VERSION_MINOR, OPENEXR_VERSION_PATCH);
	if(OPENEXR_VERSION_MAJOR == 3 && OPENEXR_VERSION_MINOR == 4 && OPENEXR_VERSION_PATCH == 13)
		passed++;
	else
		failed++;

	Imf::Header header(4, 3);
	if(header.dataWindow().min.x == 0 && header.dataWindow().min.y == 0 && header.dataWindow().max.x == 3 && header.dataWindow().max.y == 2)
		passed++;
	else
		failed++;

	header.channels().insert("R", Imf::Channel(Imf::HALF));
	if(header.channels().findChannel("R") != 0)
		passed++;
	else
		failed++;

	Imf::FrameBuffer frameBuffer;
	float pixel[4] = {0.25f, 0.5f, 0.75f, 1.0f};
	frameBuffer.insert("R", Imf::Slice(Imf::FLOAT, reinterpret_cast<char*>(pixel), sizeof(float), sizeof(float)));
	if(frameBuffer.findSlice("R") != 0)
		passed++;
	else
		failed++;

	printf("OpenEXR strict source probe: %s\n", failed ? "FAIL" : "OK");
	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
