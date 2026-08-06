#include <ImagingIO/ImagingIO.h>
#include <OpenImageIO/OIIO.h>

#include <filesystem>
#include <cstring>

using namespace Upp;
using namespace Upp::Imaging;
using namespace OIIO;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const char* name)
{
	Cout() << (condition ? "PASS " : "FAIL ") << name << '\n';
	(condition ? state.passed : state.failed)++;
}

static bool SameBytes(const ImageBuffer& a, const ImageBuffer& b)
{
	return a.GetByteCount() == b.GetByteCount() &&
	       (!a.GetByteCount() || memcmp(a.Begin(), b.Begin(), a.GetByteCount()) == 0);
}

static bool HasCode(const Diagnostics& diagnostics, const char* code)
{
	for(const DiagnosticEntry& entry : diagnostics.Entries())
		if(entry.code == code)
			return true;
	return false;
}

static ImageData Sentinel()
{
	ImageData image;
	image.spec.data_window = {0, 0, 0, 0};
	image.spec.depth = 1;
	image.spec.channels = 1;
	image.spec.channel_layout = ChannelLayout::Gray;
	image.spec.sample_type = SampleType::UInt8;
	image.spec.alpha_channel = -1;
	image.spec.channel_names.Add("Y");
	image.buffer.Allocate(image.spec);
	image.buffer.Begin()[0] = 93;
	return image;
}

static bool WriteOrdinary(const String& path, OIIO::ImageSpec spec,
                          TypeDesc type, const void* pixels)
{
	ImageOutput::unique_ptr output = ImageOutput::create(path.Begin());
	if(!output || !output->open(path.Begin(), spec))
		return false;
	bool ok = output->write_image(type, pixels);
	ok = output->close() && ok;
	output.reset();
	return ok;
}

static bool WriteMultipart(const String& path)
{
	float first[4] = {1, 0, 0, 1};
	float second[4] = {0, 1, 0, 1};
	OIIO::ImageSpec a(1, 1, 4, TypeDesc::FLOAT);
	a.channelnames = {"R", "G", "B", "A"};
	a.alpha_channel = 3;
	OIIO::ImageSpec b = a;
	ImageOutput::unique_ptr output = ImageOutput::create(path.Begin());
	if(!output || !output->open(path.Begin(), a, ImageOutput::Create))
		return false;
	if(!output->write_image(TypeDesc::FLOAT, first))
		return false;
	if(!output->open(path.Begin(), b, ImageOutput::AppendSubimage))
		return false;
	bool ok = output->write_image(TypeDesc::FLOAT, second);
	ok = output->close() && ok;
	output.reset();
	return ok;
}

static bool WriteMipmapped(const String& path)
{
	float level0[16] = {0};
	float level1[4] = {0};
	OIIO::ImageSpec full(2, 2, 4, TypeDesc::FLOAT);
	full.channelnames = {"R", "G", "B", "A"};
	full.alpha_channel = 3;
	OIIO::ImageSpec mip(1, 1, 4, TypeDesc::FLOAT);
	mip.channelnames = full.channelnames;
	mip.alpha_channel = 3;
	ImageOutput::unique_ptr output = ImageOutput::create(path.Begin());
	if(!output || !output->open(path.Begin(), full, ImageOutput::Create))
		return false;
	if(!output->write_image(TypeDesc::FLOAT, level0))
		return false;
	if(!output->open(path.Begin(), mip, ImageOutput::AppendMIPLevel))
		return false;
	bool ok = output->write_image(TypeDesc::FLOAT, level1);
	ok = output->close() && ok;
	output.reset();
	return ok;
}

static bool WriteMixedChannels(const String& path)
{
	float pixels[2] = {0.25f, 0.75f};
	OIIO::ImageSpec spec(1, 1, 2, TypeDesc::FLOAT);
	spec.channelnames = {"R", "G"};
	spec.channelformats = {TypeDesc::FLOAT, TypeDesc::HALF};
	return WriteOrdinary(path, spec, TypeDesc::FLOAT, pixels);
}

static bool WriteDeep(const String& path)
{
	OIIO::ImageSpec spec(1, 1, 1, TypeDesc::FLOAT);
	spec.channelnames = {"Z"};
	spec.deep = true;
	ImageOutput::unique_ptr output = ImageOutput::create(path.Begin());
	if(!output || !output->open(path.Begin(), spec))
		return false;
	std::vector<TypeDesc> types = {TypeDesc::FLOAT};
	std::vector<std::string> names = {"Z"};
	DeepData deep;
	deep.init(1, 1, types, names);
	deep.set_samples(0, 1);
	deep.set_deep_value(0, 0, 0, 2.0f);
	bool ok = output->write_deep_image(deep);
	ok = output->close() && ok;
	output.reset();
	return ok;
}

static void CheckRejected(State& state, const String& path,
                          const char* label, const char* code)
{
	ImageData before = Sentinel();
	ImageData output = before;
	Diagnostics diagnostics;
	Result result = LoadImageFile(path, output, &diagnostics);
	Check(state, result.code == ResultCode::Unsupported, Format("%s rejected", label));
	Check(state, output.spec.data_window == before.spec.data_window &&
	      SameBytes(output.buffer, before.buffer),
	      Format("%s preserves output", label));
	Check(state, HasCode(diagnostics, code), Format("%s stable code", label));
}

CONSOLE_APP_MAIN
{
	State state;
	String root = AppendFileName(GetTempPath(), "imaging_io_oiio_contract");
	std::filesystem::path root_path(root.Begin());
	std::error_code error;
	std::filesystem::remove_all(root_path, error);
	std::filesystem::create_directories(root_path, error);
	UppImaging::InitializeOpenImageIO();

	String rgba_path = AppendFileName(root, "independent.exr");
	float rgba[16] = {
		0.1f, 0.2f, 0.3f, 1.0f,
		0.4f, 0.5f, 0.6f, 1.0f,
		0.7f, 0.8f, 0.9f, 1.0f,
		1.0f, 0.5f, 0.25f, 1.0f
	};
	OIIO::ImageSpec rgba_spec(2, 2, 4, TypeDesc::FLOAT);
	rgba_spec.x = -3;
	rgba_spec.y = 4;
	rgba_spec.channelnames = {"R", "G", "B", "A"};
	rgba_spec.alpha_channel = 3;
	rgba_spec.attribute("test:string", "independent");
	rgba_spec.attribute("test:int", 17);
	Check(state, WriteOrdinary(rgba_path, rgba_spec, TypeDesc::FLOAT, rgba),
	      "independent EXR fixture creation");

	ImageData loaded;
	Diagnostics diagnostics;
	Result result = LoadImageFile(rgba_path, loaded, &diagnostics);
	Check(state, result.IsOk(), "adapter loads independent EXR");
	Check(state, loaded.spec.data_window == DataWindow{-3, 4, -2, 5} &&
	      loaded.spec.sample_type == SampleType::Float32 &&
	      loaded.spec.channel_layout == ChannelLayout::RGBA &&
	      loaded.spec.alpha_channel == 3,
	      "independent EXR structure");
	Check(state, loaded.metadata.Get("test:string") == Value("independent") &&
	      loaded.metadata.Get("test:int") == Value(17),
	      "independent EXR metadata");
	Check(state, loaded.buffer.GetByteCount() == sizeof(rgba) &&
	      memcmp(loaded.buffer.Begin(), rgba, sizeof(rgba)) == 0,
	      "independent EXR pixels");

	String alpha_path = AppendFileName(root, "alpha-only.exr");
	float alpha[2] = {0.25f, 0.75f};
	OIIO::ImageSpec alpha_spec(2, 1, 1, TypeDesc::FLOAT);
	alpha_spec.channelnames = {"A"};
	alpha_spec.alpha_channel = 0;
	Check(state, WriteOrdinary(alpha_path, alpha_spec, TypeDesc::FLOAT, alpha),
	      "alpha-only fixture creation");
	ImageData alpha_loaded;
	Check(state, LoadImageFile(alpha_path, alpha_loaded, &diagnostics).IsOk() &&
	      alpha_loaded.spec.channel_layout == ChannelLayout::MultiChannel &&
	      alpha_loaded.spec.alpha_channel == 0,
	      "alpha-only classification");

	String indexed_path = AppendFileName(root, "indexed-alpha.exr");
	float indexed[4] = {0.5f, 1.0f, 0.25f, 0.75f};
	OIIO::ImageSpec indexed_spec(2, 1, 2, TypeDesc::FLOAT);
	indexed_spec.channelnames = {"Y", "coverage"};
	indexed_spec.alpha_channel = 1;
	Check(state, WriteOrdinary(indexed_path, indexed_spec, TypeDesc::FLOAT, indexed),
	      "alpha-index fixture creation");
	ImageData indexed_loaded;
	Check(state, LoadImageFile(indexed_path, indexed_loaded, &diagnostics).IsOk() &&
	      indexed_loaded.spec.channel_layout == ChannelLayout::GrayAlpha &&
	      indexed_loaded.spec.alpha_channel == 1,
	      "backend alpha index drives GrayAlpha classification");

	String adapter_path = AppendFileName(root, "adapter.exr");
	Check(state, SaveImageFile(adapter_path, loaded, &diagnostics).IsOk(),
	      "adapter EXR save");
	ImageInput::unique_ptr reopened = ImageInput::open(adapter_path.Begin());
	Check(state, reopened && reopened->spec().x == -3 && reopened->spec().y == 4 &&
	      reopened->spec().format == TypeDesc::FLOAT &&
	      reopened->spec().channelnames == rgba_spec.channelnames &&
	      reopened->spec().alpha_channel == 3,
	      "OIIO inspects adapter EXR specification");
	float reopened_pixels[16] = {0};
	Check(state, reopened && reopened->read_image(TypeDesc::FLOAT, reopened_pixels) &&
	      memcmp(reopened_pixels, rgba, sizeof(rgba)) == 0,
	      "OIIO reads adapter EXR pixels");
	if(reopened) {
		reopened->close();
		reopened.reset();
	}

	String multipart = AppendFileName(root, "multipart.exr");
	Check(state, WriteMultipart(multipart), "multipart fixture creation");
	CheckRejected(state, multipart, "multipart EXR", "IMGIO_STRUCTURE");

	String mipmapped = AppendFileName(root, "mipmapped.exr");
	Check(state, WriteMipmapped(mipmapped), "mip fixture creation");
	CheckRejected(state, mipmapped, "mipmapped EXR", "IMGIO_STRUCTURE");

	String deep = AppendFileName(root, "deep.exr");
	Check(state, WriteDeep(deep), "deep fixture creation");
	CheckRejected(state, deep, "deep EXR", "IMGIO_STRUCTURE");

	String mixed = AppendFileName(root, "mixed.exr");
	Check(state, WriteMixedChannels(mixed), "mixed-channel fixture creation");
	CheckRejected(state, mixed, "mixed-channel EXR", "IMGIO_SAMPLE");

	std::filesystem::remove_all(root_path, error);
	Check(state, !std::filesystem::exists(root_path), "independent fixture cleanup");
	Cout() << "SUMMARY passed=" << state.passed
	       << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
