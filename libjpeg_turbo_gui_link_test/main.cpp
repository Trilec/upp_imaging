#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

#include <libjpeg_turbo/jpeglib.h>

using namespace Upp;

GUI_APP_MAIN
{
	jpeg_error_mgr jerr;
	jpeg_compress_struct cinfo;
	jpeg_decompress_struct dinfo;
	memset(&cinfo, 0, sizeof(cinfo));
	memset(&dinfo, 0, sizeof(dinfo));
	cinfo.err = jpeg_std_error(&jerr);
	dinfo.err = jpeg_std_error(&jerr);
	jpeg_CreateCompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));
	jpeg_CreateDecompress(&dinfo, JPEG_LIB_VERSION, sizeof(dinfo));
	jpeg_destroy_compress(&cinfo);
	jpeg_destroy_decompress(&dinfo);
}
