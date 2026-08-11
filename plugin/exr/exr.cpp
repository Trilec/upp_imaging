#include "exr.h"

#include <OpenImageIO/OIIO.h>

#include <cmath>
#include <climits>

namespace Upp {

namespace {

static bool ReadStream(Stream& stream, String& output)
{
	output.Clear();
	char chunk[64 * 1024];
	for(;;) {
		int count = stream.Get(chunk, sizeof(chunk));
		if(count <= 0)
			break;
		if(output.GetLength() > INT_MAX - count)
			return false;
		output.Cat(chunk, count);
	}
	return !stream.IsError() && !output.IsEmpty();
}

static bool HasOpenExrMagic(const String& encoded)
{
	return encoded.GetLength() >= 4 &&
	       (byte)encoded[0] == 0x76 &&
	       (byte)encoded[1] == 0x2f &&
	       (byte)encoded[2] == 0x31 &&
	       (byte)encoded[3] == 0x01;
}

static String LowerName(const std::string& name)
{
	return ToLower(String(name.c_str()));
}

static int FindChannel(const OIIO::ImageSpec& spec, const char* name)
{
	for(int i = 0; i < spec.nchannels; ++i)
		if(LowerName(spec.channelnames[i]) == name)
			return i;
	return -1;
}

static bool IsGrayName(const String& name)
{
	return name == "y" || name == "l" || name == "gray" || name == "grey";
}

static bool ResolveChannels(const OIIO::ImageSpec& spec,
                            int& red, int& green, int& blue, int& alpha)
{
	red = FindChannel(spec, "r");
	green = FindChannel(spec, "g");
	blue = FindChannel(spec, "b");
	alpha = spec.alpha_channel >= 0 && spec.alpha_channel < spec.nchannels
	      ? spec.alpha_channel : FindChannel(spec, "a");
	if(alpha < 0)
		alpha = FindChannel(spec, "alpha");

	if(red >= 0 && green >= 0 && blue >= 0 &&
	   red != green && red != blue && green != blue)
		return alpha != red && alpha != green && alpha != blue;

	if(spec.nchannels == 1) {
		red = green = blue = 0;
		alpha = -1;
		return true;
	}

	if(spec.nchannels == 2 && !spec.channelnames.empty() &&
	   IsGrayName(LowerName(spec.channelnames[0])) && alpha == 1) {
		red = green = blue = 0;
		return true;
	}

	return false;
}

static byte PreviewByte(float value)
{
	if(!std::isfinite(value) || value <= 0.0f)
		return 0;
	if(value >= 1.0f)
		return 255;
	return (byte)std::floor(value * 255.0f + 0.5f);
}

static bool HasUnsupportedStructure(OIIO::ImageInput& input,
                                    const OIIO::ImageSpec& primary)
{
	if(primary.deep || primary.depth != 1 || primary.z != 0)
		return true;
	OIIO::ImageSpec probe;
	if(input.seek_subimage(1, 0, probe))
		return true;
	input.geterror();
	if(input.seek_subimage(0, 1, probe))
		return true;
	input.geterror();
	return false;
}

} // namespace

EXRRaster::EXRRaster()
{
	format.SetRGBAStraight();
	size = Size(0, 0);
}

bool EXRRaster::Create()
{
	size = Size(0, 0);
	pixels.Clear();
	info = Info();

	String encoded;
	if(!ReadStream(GetStream(), encoded) || !HasOpenExrMagic(encoded))
		return false;

	UppImaging::InitializeOpenImageIO();
	OIIO::Filesystem::IOMemReader reader(encoded.Begin(), encoded.GetLength());
	OIIO::ImageInput::unique_ptr input = OIIO::ImageInput::open("stream.exr", nullptr, &reader);
	if(!input)
		return false;

	const OIIO::ImageSpec primary = input->spec();
	if(primary.width <= 0 || primary.height <= 0 || primary.nchannels <= 0 ||
	   HasUnsupportedStructure(*input, primary)) {
		input->close();
		input.reset();
		return false;
	}

	int red, green, blue, alpha;
	if(!ResolveChannels(primary, red, green, blue, alpha)) {
		input->close();
		input.reset();
		return false;
	}

	int64 pixel_count = (int64)primary.width * primary.height;
	if(pixel_count <= 0 || pixel_count > INT_MAX ||
	   (primary.nchannels > 0 && pixel_count > INT_MAX / primary.nchannels)) {
		input->close();
		input.reset();
		return false;
	}

	Vector<float> samples;
	samples.SetCount((int)(pixel_count * primary.nchannels));
	bool read = input->read_image(0, 0, 0, -1, OIIO::TypeDesc::FLOAT, samples.Begin());
	bool closed = input->close();
	input.reset();
	if(!read || !closed)
		return false;

	pixels.SetCount((int)pixel_count);
	for(int64 i = 0; i < pixel_count; ++i) {
		const float* source = samples.Begin() + i * primary.nchannels;
		RGBA& destination = pixels[(int)i];
		destination.r = PreviewByte(source[red]);
		destination.g = PreviewByte(source[green]);
		destination.b = PreviewByte(source[blue]);
		destination.a = alpha >= 0 ? PreviewByte(source[alpha]) : 255;
	}

	size = Size(primary.width, primary.height);
	info = Info();
	info.bpp = 32;
	info.colors = 256 * 256 * 256;
	info.dots = size;
	info.kind = alpha >= 0 ? IMAGE_ALPHA : IMAGE_OPAQUE;
	return true;
}

Size EXRRaster::GetSize()
{
	return size;
}

Raster::Info EXRRaster::GetInfo()
{
	return info;
}

Raster::Line EXRRaster::GetLine(int line)
{
	if(line < 0 || line >= size.cy) {
		SetError();
		return Line();
	}
	return Line(reinterpret_cast<const byte*>(pixels.Begin() + line * size.cx), this, false);
}

const RasterFormat *EXRRaster::GetFormat()
{
	return &format;
}

INITIALIZER(EXRRaster)
{
	StreamRaster::Register<EXRRaster>();
}

} // namespace Upp
