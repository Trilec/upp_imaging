#include <Core/Core.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libjpeg_turbo_src/jpeglib.h"

using namespace Upp;

static bool CheckMemoryApi()
{
	jpeg_compress_struct cinfo;
	jpeg_error_mgr jerr;
	memset(&cinfo, 0, sizeof(cinfo));
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_CreateCompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));
	unsigned char* outbuffer = NULL;
	unsigned long outsize = 0;
	jpeg_mem_dest(&cinfo, &outbuffer, &outsize);
	jpeg_destroy_compress(&cinfo);
	if(outbuffer)
		free(outbuffer);

	jpeg_decompress_struct dinfo;
	jpeg_error_mgr jderr;
	memset(&dinfo, 0, sizeof(dinfo));
	dinfo.err = jpeg_std_error(&jderr);
	jpeg_CreateDecompress(&dinfo, JPEG_LIB_VERSION, sizeof(dinfo));
	unsigned char dummy = 0;
	jpeg_mem_src(&dinfo, &dummy, 1);
	jpeg_destroy_decompress(&dinfo);
	return true;
}

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	printf("libjpeg-turbo version: %s\n", LIBJPEG_TURBO_VERSION);
	printf("libjpeg API version: %d\n", JPEG_LIB_VERSION);
	printf("libjpeg-turbo version number: %d\n", LIBJPEG_TURBO_VERSION_NUMBER);
	if(strcmp(LIBJPEG_TURBO_VERSION, "3.2.0") == 0 && JPEG_LIB_VERSION == 62 && LIBJPEG_TURBO_VERSION_NUMBER == 3002000) {
		printf("version info: OK\n");
		++passed;
	}
	else {
		printf("version info: FAIL\n");
		++failed;
	}

	jpeg_error_mgr jerr;
	if(jpeg_std_error(&jerr)) {
		printf("error manager: OK\n");
		++passed;
	}
	else {
		printf("error manager: FAIL\n");
		++failed;
	}

	jpeg_compress_struct cinfo;
	memset(&cinfo, 0, sizeof(cinfo));
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_CreateCompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));
	printf("compress context: OK\n");
	++passed;
	jpeg_destroy_compress(&cinfo);

	jpeg_decompress_struct dinfo;
	memset(&dinfo, 0, sizeof(dinfo));
	dinfo.err = jpeg_std_error(&jerr);
	jpeg_CreateDecompress(&dinfo, JPEG_LIB_VERSION, sizeof(dinfo));
	printf("decompress context: OK\n");
	++passed;
	jpeg_destroy_decompress(&dinfo);

	if(CheckMemoryApi()) {
		printf("memory API: OK\n");
		++passed;
	}
	else {
		printf("memory API: FAIL\n");
		++failed;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
