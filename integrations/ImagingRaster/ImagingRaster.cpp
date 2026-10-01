#include "ImagingRaster.h"

#include <OpenImageIO/OIIO.h>

#include <cmath>
#include <climits>
#include <cstring>

namespace Upp {

namespace {

static bool ReadStream(Stream& stream, String& output, int64 limit)
{
	output.Clear();
	if(limit <= 0 || limit > INT_MAX)
		return false;
	int64 length = stream.GetSize(), position = stream.GetPos();
	if(length >= position && position >= 0 && length - position > limit)
		return false;
	char chunk[64 * 1024];
	for(;;) {
		int count = stream.Get(chunk, sizeof(chunk));
		if(count <= 0)
			break;
		if(output.GetLength() > limit - count)
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


static bool MatchesSignature(const String& name, const String& data)
{
	if(name == "openexr") return HasOpenExrMagic(data);
	if(name == "png") return data.GetLength() >= 8 && memcmp(data.Begin(), "\x89PNG\r\n\x1a\n", 8) == 0;
	if(name == "jpegxl") return data.GetLength() >= 2 &&
		(((byte)data[0] == 0xff && (byte)data[1] == 0x0a) ||
		 (data.GetLength() >= 12 && data.Mid(4, 4) == "JXL "));
	if(name == "hdr") return data.StartsWith("#?RADIANCE") || data.StartsWith("#?RGBE");
	if(name == "dpx") return data.StartsWith("SDPX") || data.StartsWith("XPDS");
	if(name == "cineon") return data.GetLength() >= 4 &&
		(((byte)data[0] == 0x80 && (byte)data[1] == 0x2a && (byte)data[2] == 0x5f && (byte)data[3] == 0xd7) ||
		 ((byte)data[0] == 0xd7 && (byte)data[1] == 0x5f && (byte)data[2] == 0x2a && (byte)data[3] == 0x80));
	if(name == "webp") return data.GetLength() >= 12 && data.StartsWith("RIFF") && data.Mid(8, 4) == "WEBP";
	if(name == "heif") return data.GetLength() >= 12 && data.Mid(4, 4) == "ftyp";
	if(name == "tiff") return data.GetLength() >= 4 &&
		((data[0] == 'I' && data[1] == 'I' && ((byte)data[2] == 42 || (byte)data[2] == 43) && data[3] == 0) ||
		 (data[0] == 'M' && data[1] == 'M' && data[2] == 0 && ((byte)data[3] == 42 || (byte)data[3] == 43)));
	return true; // Camera RAW has several unrelated container signatures.
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
	if((int)spec.channelnames.size() < spec.nchannels)
		return false;
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
	if(input.seek_subimage(1, 0))
		return true;
	input.geterror();
	if(input.seek_subimage(0, 1))
		return true;
	input.geterror();
	return false;
}

} // namespace

ImagingRaster::ImagingRaster(const char* reader, const char* suffix)
	: backend(reader), extension(suffix)
{
	format.SetRGBAStraight();
	size = Size(0, 0);
}

bool ImagingRaster::Create()
{
	size = Size(0, 0);
	pixels.Clear();
	info = Info();

	if(limits.decoded_bytes <= 0 || limits.decoded_bytes > INT_MAX ||
	   limits.pixels <= 0 || limits.dimension <= 0 || limits.channels <= 0)
		return false;
	String encoded;
	if(!ReadStream(GetStream(), encoded, limits.encoded_bytes) ||
	   !MatchesSignature(backend, encoded))
		return false;

	UppImaging::InitializeOpenImageIO();
	OIIO::Filesystem::IOMemReader reader(encoded.Begin(), encoded.GetLength());
	// RAW's native reader needs a file. The bounded encoded buffer is staged
	// only for readers without IOProxy support and removed on every exit.
	struct TemporaryFile {
		String path;
		~TemporaryFile() { if(!path.IsEmpty()) DeleteFile(path); }
	} temporary;
	OIIO::ImageInput::unique_ptr input = OIIO::ImageInput::create(backend.Begin());
	if(!input) {
		OIIO::geterror();
		return false;
	}
	String filename = "stream" + extension;
	if(input->supports("ioproxy")) {
		if(!input->set_ioproxy(&reader))
			return false;
	}
	else {
		temporary.path = GetTempFileName();
		if(temporary.path.IsEmpty() || !SaveFile(temporary.path, encoded))
			return false;
		filename = temporary.path;
	}
	OIIO::ImageSpec config, opened;
	config.attribute("oiio:UnassociatedAlpha", 1);
	config.attribute("raw:max_raw_memory_mb", (int)max<int64>(1, limits.decoded_bytes / (1024 * 1024)));
	if(!input->open(filename.Begin(), opened, config)) {
		input->geterror();
		return false;
	}

	const OIIO::ImageSpec primary = input->spec();
	if(primary.width <= 0 || primary.height <= 0 || primary.nchannels <= 0 ||
	   primary.width > limits.dimension || primary.height > limits.dimension ||
	   primary.nchannels > limits.channels ||
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
	if(pixel_count <= 0 || pixel_count > limits.pixels || pixel_count > INT_MAX ||
	   pixel_count > INT_MAX / primary.nchannels ||
	   limits.decoded_bytes <= 0 ||
	   pixel_count > limits.decoded_bytes / (sizeof(float) * primary.nchannels + sizeof(RGBA))) {
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

Size ImagingRaster::GetSize()
{
	return size;
}

Raster::Info ImagingRaster::GetInfo()
{
	return info;
}

Raster::Line ImagingRaster::GetLine(int line)
{
	if(line < 0 || line >= size.cy) {
		SetError();
		return Line();
	}
	return Line(reinterpret_cast<const byte*>(pixels.Begin() + line * size.cx), this, false);
}

const RasterFormat *ImagingRaster::GetFormat()
{
	return &format;
}


} // namespace Upp
