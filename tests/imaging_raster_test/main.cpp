#include <plugin/exr/exr.h>
#include <plugin/imaging_png/imaging_png.h>
#include <plugin/imaging_jpeg/imaging_jpeg.h>
#include <plugin/imaging_jxl/imaging_jxl.h>
#include <plugin/imaging_hdr/imaging_hdr.h>
#include <plugin/imaging_dpx/imaging_dpx.h>
#include <plugin/imaging_cineon/imaging_cineon.h>
#include <plugin/imaging_raw/imaging_raw.h>
#include <plugin/imaging_webp/imaging_webp.h>
#include <plugin/imaging_heif/imaging_heif.h>
#include <plugin/imaging_tiff/imaging_tiff.h>
#include <OpenImageIO/OIIO.h>
#include <ImagingIO/ImagingIO.h>
#include "Fixtures.h"
#include <filesystem>
#include <cstring>
#include <cmath>

using namespace Upp;

struct State { int passed = 0, failed = 0; };
static void Check(State& s, bool ok, const String& label)
{
	Cout() << (ok ? "PASS " : "FAIL ") << label << '\n';
	(ok ? s.passed : s.failed)++;
}
static bool Pixel(ImagingRaster& raster, RGBA& pixel)
{
	if(raster.GetSize().IsEmpty()) return false;
	Raster::Line line = raster.GetLine(0);
	if(!line.GetRawData()) return false;
	memcpy(&pixel, line.GetRawData(), sizeof(pixel));
	return true;
}
static bool Near(int a, int b) { return abs(a - b) <= 6; }

template<class T>
static String Generated(State& s, const String& root, const char* extension)
{
	String path = AppendFileName(root, String("rgb.") + extension);
	OIIO::ImageSpec spec(8, 6, 3, OIIO::TypeDesc::FLOAT);
	Vector<float> pixels; pixels.SetCount(8 * 6 * 3);
	for(int i = 0; i < pixels.GetCount(); i += 3) {
		pixels[i] = 0.25f; pixels[i + 1] = 0.5f; pixels[i + 2] = 0.75f;
	}
	OIIO::ImageBuf image(spec, pixels.Begin());
	std::string error;
	bool saved = UppImaging::SaveImage(path.Begin(), image, &error);
	Check(s, saved, String(extension) + " fixture saved");
	if(!saved) Cout() << error.c_str() << '\n';
	String encoded = LoadFile(path);
	T raster;
	StringStream stream(encoded);
	bool opened = raster.Open(stream);
	Check(s, opened && raster.GetSize() == Size(8, 6), String(extension) + " direct stream dimensions");
	RGBA pixel = {};
	Check(s, opened && Pixel(raster, pixel) && Near(pixel.r, 64) &&
	         Near(pixel.g, 128) && Near(pixel.b, 191) && pixel.a == 255,
	      String(extension) + " decoded RGB preview");
	Image registered = StreamRaster::LoadStringAny(encoded);
	Check(s, registered.GetSize() == Size(8, 6), String(extension) + " registered memory loading");
	Image file = StreamRaster::LoadFileAny(path);
	Check(s, file.GetSize() == Size(8, 6), String(extension) + " registered file loading");
	return encoded;
}

static void Heif(State& s, const char* file, Size size)
{
	String encoded = LoadFile(file);
	ImagingHEIFRaster raster;
	StringStream stream(encoded);
	Check(s, !encoded.IsEmpty() && raster.Open(stream) && raster.GetSize() == size,
	      String(file) + " direct HEIF-family preview");
	Image image = StreamRaster::LoadStringAny(encoded);
	Check(s, image.GetSize() == size, String(file) + " registered HEIF-family preview");
}

CONSOLE_APP_MAIN
{
	State s;
	UppImaging::InitializeOpenImageIO();
	String root = AppendFileName(GetCurrentDirectory(), "build/windows-x64/samples");
	RealizeDirectory(root);
	Generated<EXRRaster>(s, root, "exr");
	String png = Generated<ImagingPNGRaster>(s, root, "png");
	Generated<ImagingJPEGRaster>(s, root, "jpg");
	using namespace Upp::Imaging;
	ImageData jpeg, alias, gray, gray_read;
	bool jpeg_loaded = LoadImageFile(AppendFileName(root, "rgb.jpg"), jpeg).IsOk();
	Check(s, jpeg_loaded && jpeg.spec.sample_type == SampleType::UInt8 &&
	         jpeg.spec.channel_layout == ChannelLayout::RGB &&
	         Near(jpeg.buffer.Begin()[0], 64) && Near(jpeg.buffer.Begin()[1], 128),
	      "ImagingIO JPEG loads RGB pixels");
	String alias_path = AppendFileName(root, "rgb.jpeg");
	Check(s, jpeg_loaded && SaveImageFile(alias_path, jpeg).IsOk() &&
	         LoadImageFile(alias_path, alias).IsOk() &&
	         Near(alias.buffer.Begin()[0], jpeg.buffer.Begin()[0]),
	      "ImagingIO JPEG alias saves and reloads");
	if(jpeg_loaded) {
		gray.spec = jpeg.spec;
		gray.spec.channel_layout = ChannelLayout::Gray;
		gray.spec.channels = 1;
		gray.spec.channel_names.Clear(); gray.spec.channel_names.Add("Y");
		gray.spec.alpha_channel = -1;
		gray.buffer.Allocate(gray.spec);
		memset(gray.buffer.Begin(), 128, (size_t)gray.buffer.GetByteCount());
	}
	String gray_path = AppendFileName(root, "gray.jpg");
	Check(s, jpeg_loaded && SaveImageFile(gray_path, gray).IsOk() &&
	         LoadImageFile(gray_path, gray_read).IsOk() &&
	         gray_read.spec.channel_layout == ChannelLayout::Gray &&
	         Near(gray_read.buffer.Begin()[0], 128),
	      "ImagingIO JPEG Gray roundtrip");
	ImageData rgba;
	if(jpeg_loaded) {
		rgba.spec = jpeg.spec; rgba.spec.channel_layout = ChannelLayout::RGBA;
		rgba.spec.channels = 4;
		rgba.spec.channel_names.Add("A"); rgba.spec.alpha_channel = 3;
		rgba.buffer.Allocate(rgba.spec);
		memset(rgba.buffer.Begin(), 255, (size_t)rgba.buffer.GetByteCount());
	}
	Check(s, jpeg_loaded && SaveImageFile(AppendFileName(root, "alpha.jpg"), rgba).code == ResultCode::Unsupported,
	      "ImagingIO JPEG rejects alpha without silently discarding it");
	Generated<ImagingJXLRaster>(s, root, "jxl");
	Generated<ImagingHDRRaster>(s, root, "hdr");
	Generated<ImagingDPXRaster>(s, root, "dpx");
	Generated<ImagingWebPRaster>(s, root, "webp");
	Generated<ImagingTIFFRaster>(s, root, "tif");
	Heif(s, "third_party/codecs/libheif_src/upstream/examples/example.avif", Size(800, 533));
	Heif(s, "third_party/codecs/libheif_src/upstream/tests/data/rainbow-451x461.heic", Size(451, 461));
	const char* compound_path = "third_party/codecs/libheif_src/upstream/examples/example.heic";
	auto compound = OIIO::ImageInput::open(compound_path);
	bool multipart = compound && compound->seek_subimage(1, 0);
	Check(s, multipart, "upstream example HEIC verified as multiple images");
	if(compound) {
		Cout() << "HEIC native dimensions " << compound->spec().width << "x" << compound->spec().height << '\n';
		compound->close();
	}
	StringStream compound_stream(LoadFile(compound_path));
	ImagingHEIFRaster compound_raster;
	Check(s, !compound_raster.Open(compound_stream), "multiple-image HEIC rejected by single-image preview contract");


	String cineon_path = AppendFileName(root, "rgb.cin");
	Check(s, WriteCineon(cineon_path), "synthetic Cineon fixture saved");
	StringStream cineon_stream(LoadFile(cineon_path));
	ImagingCineonRaster cineon_fixture;
	RGBA cineon_pixel = {};
	Check(s, cineon_fixture.Open(cineon_stream) && cineon_fixture.GetSize() == Size(8, 6) &&
	         Pixel(cineon_fixture, cineon_pixel) && cineon_pixel.r == 64 &&
	         cineon_pixel.g == 128 && cineon_pixel.b == 192,
	      "Cineon direct preview exact RGB pixels");
	Check(s, StreamRaster::LoadFileAny(cineon_path).GetSize() == Size(8, 6), "Cineon registered file loading");

	String dng_path = AppendFileName(root, "synthetic.dng");
	Check(s, WriteDng(dng_path), "synthetic linear camera DNG fixture saved");
	StringStream dng_stream(LoadFile(dng_path));
	ImagingRAWRaster raw_fixture;
	RGBA raw_pixel = {};
	Check(s, raw_fixture.Open(dng_stream) && raw_fixture.GetSize() == Size(64, 64) &&
	         Pixel(raw_fixture, raw_pixel) && raw_pixel.a == 255 &&
	         (raw_pixel.r || raw_pixel.g || raw_pixel.b), "RAW temporary-file adapter decodes DNG preview");
	Check(s, StreamRaster::LoadFileAny(dng_path).GetSize() == Size(64, 64), "DNG registered file loading");


	Check(s, SaveFile(AppendFileName(root, "example.avif"), LoadFile("third_party/codecs/libheif_src/upstream/examples/example.avif")) &&
	         SaveFile(AppendFileName(root, "rainbow.heic"), LoadFile("third_party/codecs/libheif_src/upstream/tests/data/rainbow-451x461.heic")),
	      "manual HEIF-family samples retained");
	OIIO::ImageSpec alpha_spec(1, 1, 4, OIIO::TypeDesc::FLOAT);
	alpha_spec.attribute("oiio:UnassociatedAlpha", 1);
	float alpha_values[] = {0.25f, 0.5f, 0.75f, 0.5f};
	OIIO::ImageBuf alpha_image(alpha_spec, alpha_values);
	String alpha_path = AppendFileName(root, "alpha.png");
	std::string alpha_error;
	Check(s, UppImaging::SaveImage(alpha_path.Begin(), alpha_image, &alpha_error), "PNG alpha sample retained");
	ImagingPNGRaster alpha_raster;
	StringStream alpha_stream(LoadFile(alpha_path));
	RGBA alpha_pixel = {};
	Check(s, alpha_raster.Open(alpha_stream) && Pixel(alpha_raster, alpha_pixel) &&
	         alpha_pixel.r == 64 && alpha_pixel.g == 128 && alpha_pixel.b == 191 &&
	         alpha_pixel.a == 128, "PNG preview retains straight alpha without double multiplication");

	ImagingPNGRaster raster;
	ImagingRasterLimits limits;
	limits.encoded_bytes = png.GetLength() - 1;
	raster.SetLimits(limits);
	StringStream encoded_limit(png);
	Check(s, !raster.Open(encoded_limit) && encoded_limit.GetPos() == 0 &&
	         raster.GetSize() == Size(0, 0), "encoded size cap rejects before reading");

	limits = ImagingRasterLimits(); limits.pixels = 47;
	raster.SetLimits(limits);
	StringStream pixel_limit(png);
	Check(s, !raster.Open(pixel_limit) && raster.GetSize() == Size(0, 0),
	      "pixel allocation cap rejects without publishing state");
	limits = ImagingRasterLimits(); limits.decoded_bytes = 48 * 16 - 1;
	raster.SetLimits(limits);
	StringStream byte_limit(png);
	Check(s, !raster.Open(byte_limit) && raster.GetSize() == Size(0, 0),
	      "combined float plus RGBA allocation cap rejects");
	limits = ImagingRasterLimits(); limits.dimension = 7;
	raster.SetLimits(limits);
	StringStream dimension_limit(png);
	Check(s, !raster.Open(dimension_limit), "per-dimension preview cap rejects");

	ImagingRAWRaster raw;
	ImagingCineonRaster cineon;
	StringStream invalid_raw("invalid camera raw"), invalid_cin("invalid cineon");
	Check(s, !raw.Open(invalid_raw), "RAW file adapter rejects malformed input");
	Check(s, !cineon.Open(invalid_cin), "Cineon stream adapter rejects malformed input");
	Check(s, StreamRaster::LoadStringAny("not an image").IsEmpty(), "registered readers reject unknown input");

	Cout() << "Manual samples retained at " << root << '\n';
	Cout() << "SUMMARY passed=" << s.passed << " failed=" << s.failed << '\n';
	SetExitCode(s.failed ? 1 : 0);
}
