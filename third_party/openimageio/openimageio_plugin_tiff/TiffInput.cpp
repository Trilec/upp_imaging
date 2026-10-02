// Repository read-policy overlay; the pinned upstream source stays unchanged.
#include "AllocationPolicy.h"
#define TIFFOpenOptionsAlloc UppImaging::TiffReadOptions
#define TIFFClientOpen UppImaging::TiffReadClient
#include "../openimageio_plugins_src/upstream/src/tiff.imageio/tiffinput.cpp"
#undef TIFFClientOpen
#undef TIFFOpenOptionsAlloc
