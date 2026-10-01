#ifndef _imaging_raster_test_Fixtures_h_
#define _imaging_raster_test_Fixtures_h_

#include <libtiff/tiffio.h>
#include "../../third_party/openimageio/openimageio_plugins_src/upstream/src/cineon.imageio/libcineon/CineonHeader.h"

// Original synthetic fixtures. No downloaded camera data or writer changes.
static bool WriteCineon(const Upp::String& path)
{
	cineon::Header header;
	header.magicNumber = CINEON_MAGIC_COOKIE;
	const int bytes = sizeof(cineon::GenericHeader) + sizeof(cineon::IndustryHeader);
	header.imageOffset = bytes;
	header.genericSize = sizeof(cineon::GenericHeader);
	header.industrySize = sizeof(cineon::IndustryHeader);
	header.userSize = 0;
	header.fileSize = bytes + 8 * 6 * 3;
	header.imageOrientation = cineon::kLeftToRightTopToBottom;
	header.numberOfElements = 3;
	header.interleave = cineon::kPixel;
	header.packing = cineon::kByteLeft;
	header.dataSign = 0;
	header.imageSense = 0;
	header.endOfLinePadding = header.endOfImagePadding = 0;
	header.xOffset = header.yOffset = 0;
	for(int i = 0; i < 3; ++i) {
		header.chan[i].designator[0] = 0;
		header.chan[i].designator[1] = cineon::kRec709Red + i;
		header.chan[i].bitDepth = 8;
		header.chan[i].pixelsPerLine = 8;
		header.chan[i].linesPerElement = 6;
		header.chan[i].lowData = header.chan[i].lowQuantity = 0;
		header.chan[i].highData = header.chan[i].highQuantity = 255;
	}
	Upp::String encoded((const char*)&header.magicNumber, bytes);
	for(int i = 0; i < 8 * 6; ++i) {
		encoded.Cat((char)64); encoded.Cat((char)128); encoded.Cat((char)192);
	}
	return Upp::SaveFile(path, encoded);
}

static bool WriteDng(const Upp::String& path)
{
	TIFF* tif = TIFFOpen(path.Begin(), "w");
	if(!tif) return false;
	const uint8_t version[4] = {1, 4, 0, 0}, backward[4] = {1, 1, 0, 0};
	const float matrix[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
	const float neutral[3] = {1, 1, 1}, black[1] = {0};
	const uint32_t white[1] = {65535};
	bool ok = TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, uint32_t(64)) &&
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, uint32_t(64)) &&
		TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3) &&
		TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 16) &&
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, 34892) &&
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG) &&
		TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE) &&
		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, uint32_t(64)) &&
		TIFFSetField(tif, TIFFTAG_MAKE, "Upp") &&
		TIFFSetField(tif, TIFFTAG_MODEL, "Synthetic Linear DNG") &&
		TIFFSetField(tif, TIFFTAG_DNGVERSION, version) &&
		TIFFSetField(tif, TIFFTAG_DNGBACKWARDVERSION, backward) &&
		TIFFSetField(tif, TIFFTAG_UNIQUECAMERAMODEL, "Upp Synthetic Linear DNG") &&
		TIFFSetField(tif, TIFFTAG_COLORMATRIX1, 9, matrix) &&
		TIFFSetField(tif, TIFFTAG_CALIBRATIONILLUMINANT1, 21) &&
		TIFFSetField(tif, TIFFTAG_ASSHOTNEUTRAL, 3, neutral) &&
		TIFFSetField(tif, TIFFTAG_BLACKLEVEL, 1, black) &&
		TIFFSetField(tif, TIFFTAG_WHITELEVEL, 1, white);
	uint16_t row[64 * 3];
	for(int y = 0; ok && y < 64; ++y) {
		for(int x = 0; x < 64; ++x) {
			row[x * 3] = 16384 + x * 128;
			row[x * 3 + 1] = 32768 + y * 64;
			row[x * 3 + 2] = 49152;
		}
		ok = TIFFWriteScanline(tif, row, y, 0) >= 0;
	}
	TIFFClose(tif);
	return ok;
}
#endif
