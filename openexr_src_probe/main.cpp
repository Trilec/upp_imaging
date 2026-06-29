#include <stdio.h>

#include <openexr_src/openexr_version.h>
#include <openexr_src/ImfVersion.h>
#include <openexr_src/upstream/ImfForward.h>
#include <imath_src/half.h>

int main()
{
	int passed = 0;
	int failed = 0;

	printf("OpenEXR version: %d.%d.%d\n", OPENEXR_VERSION_MAJOR, OPENEXR_VERSION_MINOR, OPENEXR_VERSION_PATCH);
	if(OPENEXR_VERSION_MAJOR == 3 && OPENEXR_VERSION_MINOR == 4 && OPENEXR_VERSION_PATCH == 13)
		passed++;
	else
		failed++;

	OPENEXR_IMF_NAMESPACE::Header* header = 0;
	IMATH_NAMESPACE::half h = 0.5f;
	if(header == 0 && float(h) > 0.49f && float(h) < 0.51f)
		passed++;
	else
		failed++;

	printf("OpenEXR namespace/header probe: %s\n", failed ? "FAIL" : "OK");
	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
