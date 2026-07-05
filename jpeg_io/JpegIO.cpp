#include "JpegIO.h"

#include <stdio.h>
#include <setjmp.h>
#include <string.h>

#include <libjpeg_turbo/jpeglib.h>

using namespace Upp;

struct JpegErrorMgr
{
	jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
	String* error = NULL;
};

static void JpegErrorExit(j_common_ptr cinfo)
{
	JpegErrorMgr* err = (JpegErrorMgr*)cinfo->err;
	char message[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message)(cinfo, message);
	if(err->error)
		*err->error = message;
	longjmp(err->setjmp_buffer, 1);
}

static void JpegOutputMessage(j_common_ptr)
{
}

static void JpegEmitMessage(j_common_ptr, int)
{
}

static bool CheckImage(const JpegRgbImage8& image, String& error)
{
	if(image.width <= 0 || image.height <= 0) {
		error = "invalid JPEG image dimensions";
		return false;
	}
	const int64 pixels = (int64)image.width * (int64)image.height;
	if(pixels <= 0 || pixels > INT_MAX) {
		error = "JPEG image pixel count overflow";
		return false;
	}
	if(image.pixels.GetCount() != (int)pixels) {
		error = "JPEG image pixel count mismatch";
		return false;
	}
	return true;
}

static bool CheckSaveOptions(const JpegSaveOptions& options, String& error)
{
	if(options.quality < 1 || options.quality > 100) {
		error = "invalid JPEG quality";
		return false;
	}
	return true;
}

static void ApplySubsampling(j_compress_ptr cinfo, JpegSubsampling subsampling)
{
	switch(subsampling) {
	case JpegSubsampling::S444:
		cinfo->comp_info[0].h_samp_factor = 1;
		cinfo->comp_info[0].v_samp_factor = 1;
		cinfo->comp_info[1].h_samp_factor = 1;
		cinfo->comp_info[1].v_samp_factor = 1;
		cinfo->comp_info[2].h_samp_factor = 1;
		cinfo->comp_info[2].v_samp_factor = 1;
		break;
	case JpegSubsampling::S422:
		cinfo->comp_info[0].h_samp_factor = 2;
		cinfo->comp_info[0].v_samp_factor = 1;
		cinfo->comp_info[1].h_samp_factor = 1;
		cinfo->comp_info[1].v_samp_factor = 1;
		cinfo->comp_info[2].h_samp_factor = 1;
		cinfo->comp_info[2].v_samp_factor = 1;
		break;
	case JpegSubsampling::S420:
		cinfo->comp_info[0].h_samp_factor = 2;
		cinfo->comp_info[0].v_samp_factor = 2;
		cinfo->comp_info[1].h_samp_factor = 1;
		cinfo->comp_info[1].v_samp_factor = 1;
		cinfo->comp_info[2].h_samp_factor = 1;
		cinfo->comp_info[2].v_samp_factor = 1;
		break;
	}
}

void JpegRgbImage8::Clear()
{
	width = 0;
	height = 0;
	pixels.Clear();
}

bool JpegRgbImage8::IsValid() const
{
	return width > 0 && height > 0 && pixels.GetCount() == (int64)width * height;
}

bool SaveJpegRgb8(const char* path, const JpegRgbImage8& image, const JpegSaveOptions& options, String* error)
{
	if(error)
		error->Clear();
	if(path == NULL || !*path) {
		if(error)
			*error = "empty JPEG path";
		return false;
	}
	String validation_error;
	if(!CheckImage(image, validation_error)) {
		if(error)
			*error = validation_error;
		return false;
	}
	if(!CheckSaveOptions(options, validation_error)) {
		if(error)
			*error = validation_error;
		return false;
	}
	const size_t row_stride = (size_t)image.width * 3;
	if(row_stride == 0 || row_stride > (size_t)INT_MAX) {
		if(error)
			*error = "JPEG row stride overflow";
		return false;
	}

	FILE* fp = fopen(path, "wb");
	if(!fp) {
		if(error)
			*error = "cannot open JPEG output file";
		return false;
	}

	jpeg_compress_struct cinfo;
	JpegErrorMgr jerr;
	memset(&cinfo, 0, sizeof(cinfo));
	memset(&jerr, 0, sizeof(jerr));
	jerr.error = error;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = JpegErrorExit;
	jerr.pub.output_message = JpegOutputMessage;
	jerr.pub.emit_message = JpegEmitMessage;

	if(setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_compress(&cinfo);
		fclose(fp);
		return false;
	}

	jpeg_CreateCompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));
	jpeg_stdio_dest(&cinfo, fp);
	cinfo.image_width = (JDIMENSION)image.width;
	cinfo.image_height = (JDIMENSION)image.height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, options.quality, TRUE);
	cinfo.optimize_coding = options.optimize_coding ? TRUE : FALSE;
	if(options.progressive)
		jpeg_simple_progression(&cinfo);
	ApplySubsampling(&cinfo, options.subsampling);
	jpeg_start_compress(&cinfo, TRUE);

	while(cinfo.next_scanline < cinfo.image_height) {
		JSAMPROW row = (JSAMPROW)reinterpret_cast<const JSAMPLE*>(image.pixels.Begin() + (size_t)cinfo.next_scanline * (size_t)image.width);
		jpeg_write_scanlines(&cinfo, &row, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	fclose(fp);
	return true;
}

bool LoadJpegRgb8(const char* path, JpegRgbImage8& image, String* error)
{
	if(error)
		error->Clear();
	image.Clear();
	if(path == NULL || !*path) {
		if(error)
			*error = "empty JPEG path";
		return false;
	}

	FILE* fp = fopen(path, "rb");
	if(!fp) {
		if(error)
			*error = "cannot open JPEG input file";
		return false;
	}

	jpeg_decompress_struct dinfo;
	JpegErrorMgr jerr;
	memset(&dinfo, 0, sizeof(dinfo));
	memset(&jerr, 0, sizeof(jerr));
	jerr.error = error;
	dinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = JpegErrorExit;
	jerr.pub.output_message = JpegOutputMessage;
	jerr.pub.emit_message = JpegEmitMessage;

	if(setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&dinfo);
		fclose(fp);
		image.Clear();
		return false;
	}

	jpeg_CreateDecompress(&dinfo, JPEG_LIB_VERSION, sizeof(dinfo));
	jpeg_stdio_src(&dinfo, fp);
	if(jpeg_read_header(&dinfo, TRUE) != JPEG_HEADER_OK) {
		if(error)
			*error = "JPEG header read failed";
		jpeg_destroy_decompress(&dinfo);
		fclose(fp);
		return false;
	}
	if(dinfo.jpeg_color_space != JCS_GRAYSCALE && dinfo.jpeg_color_space != JCS_RGB && dinfo.jpeg_color_space != JCS_YCbCr) {
		if(error)
			*error = "unsupported JPEG color space";
		jpeg_destroy_decompress(&dinfo);
		fclose(fp);
		return false;
	}
	dinfo.out_color_space = JCS_RGB;
	jpeg_start_decompress(&dinfo);
	if(dinfo.output_components != 3) {
		if(error)
			*error = "unexpected JPEG output components";
		jpeg_finish_decompress(&dinfo);
		jpeg_destroy_decompress(&dinfo);
		fclose(fp);
		return false;
	}
	if(dinfo.output_width == 0 || dinfo.output_height == 0) {
		if(error)
			*error = "JPEG has zero dimensions";
		jpeg_finish_decompress(&dinfo);
		jpeg_destroy_decompress(&dinfo);
		fclose(fp);
		return false;
	}
	const int64 pixels = (int64)dinfo.output_width * (int64)dinfo.output_height;
	if(pixels <= 0 || pixels > INT_MAX) {
		if(error)
			*error = "JPEG image pixel count overflow";
		jpeg_finish_decompress(&dinfo);
		jpeg_destroy_decompress(&dinfo);
		fclose(fp);
		return false;
	}

	image.width = (int)dinfo.output_width;
	image.height = (int)dinfo.output_height;
	image.pixels.SetCount((int)pixels);
	const size_t row_stride = (size_t)image.width * 3;
	Vector<byte> row;
	row.SetCount((int)row_stride);
	while(dinfo.output_scanline < dinfo.output_height) {
		JSAMPROW row_ptr = row.Begin();
		jpeg_read_scanlines(&dinfo, &row_ptr, 1);
		memcpy(image.pixels.Begin() + (size_t)(dinfo.output_scanline - 1) * (size_t)image.width, row.Begin(), row_stride);
	}

	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);
	fclose(fp);
	return true;
}
