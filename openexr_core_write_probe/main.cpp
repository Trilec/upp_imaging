#include <stdio.h>

#include <openexr_core_src/openexr.h>

static int fail_step(const char* label, int& failed)
{
	printf("%s: FAIL\n", label);
	++failed;
	return 1;
}

int main()
{
	int passed = 0;
	int failed = 0;

	exr_context_t ctxt = 0;
	exr_result_t r = exr_start_write(
		&ctxt,
		"E:/apps/github/upp_imaging/out/core_write_probe.exr",
		EXR_WRITE_FILE_DIRECTLY,
		0);
	if(r != EXR_ERR_SUCCESS || !ctxt)
		return fail_step("OpenEXRCore write context", failed);
	printf("OpenEXRCore write context: OK\n");
	++passed;

	int part_index = -1;
	r = exr_add_part(ctxt, 0, EXR_STORAGE_SCANLINE, &part_index);
	if(r != EXR_ERR_SUCCESS || part_index != 0)
	{
		fail_step("OpenEXRCore required attrs", failed);
		exr_finish(&ctxt);
		printf("SUMMARY passed=%d failed=%d\n", passed, failed);
		return 1;
	}

	r = exr_initialize_required_attr_simple(ctxt, part_index, 2, 2, EXR_COMPRESSION_NONE);
	if(r != EXR_ERR_SUCCESS)
	{
		fail_step("OpenEXRCore required attrs", failed);
		exr_finish(&ctxt);
		printf("SUMMARY passed=%d failed=%d\n", passed, failed);
		return 1;
	}

	r = exr_add_channel(
		ctxt,
		part_index,
		"Y",
		EXR_PIXEL_HALF,
		EXR_PERCEPTUALLY_LINEAR,
		1,
		1);
	if(r != EXR_ERR_SUCCESS)
	{
		fail_step("OpenEXRCore required attrs", failed);
		exr_finish(&ctxt);
		printf("SUMMARY passed=%d failed=%d\n", passed, failed);
		return 1;
	}

	printf("OpenEXRCore required attrs: OK\n");
	++passed;

	r = exr_write_header(ctxt);
	if(r != EXR_ERR_SUCCESS)
	{
		printf("OpenEXRCore finish write context: FAIL\n");
		++failed;
		exr_finish(&ctxt);
		printf("SUMMARY passed=%d failed=%d\n", passed, failed);
		return 1;
	}

	r = exr_finish(&ctxt);
	if(r != EXR_ERR_SUCCESS)
	{
		printf("OpenEXRCore finish write context: FAIL\n");
		++failed;
	}
	else
	{
		printf("OpenEXRCore finish write context: OK\n");
		++passed;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
