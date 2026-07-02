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

	exr_context_t ctxt = 0;
	exr_result_t r = exr_start_temporary_context(&ctxt, "upp_imaging_core_probe", 0);
	if(r == EXR_ERR_SUCCESS && ctxt) {
		const char* name = 0;
		exr_result_t nr = exr_get_file_name(ctxt, &name);
		exr_result_t fr = exr_finish(&ctxt);
		if(nr == EXR_ERR_SUCCESS && fr == EXR_ERR_SUCCESS) {
			printf("OpenEXRCore temporary context: OK\n");
			passed++;
		} else {
			printf("OpenEXRCore temporary context: FAIL\n");
			failed++;
		}
	} else {
		printf("OpenEXRCore temporary context: FAIL\n");
		failed++;
	}

	printf("OpenEXRCore public headers: OK\n");
	passed++;

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
