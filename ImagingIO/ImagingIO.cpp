#include "ImagingIO.h"

#include <OpenImageIO/OIIO.h>

#include <atomic>
#include <climits>
#include <filesystem>

namespace Upp {
namespace Imaging {

using namespace OIIO;

namespace {

static void Reset(Diagnostics* diagnostics)
{
	if(diagnostics)
		diagnostics->Clear();
}

static Result Fail(ResultCode code, Diagnostics* diagnostics, String message,
                   String context, const char* diagnostic)
{
	if(diagnostics)
		diagnostics->Error(message, diagnostic, context);
	return Result::Failure(code, message, context);
}

static void Warn(Diagnostics* diagnostics, String message, String context,
                 const char* code)
{
	if(diagnostics)
		diagnostics->Add(DiagnosticSeverity::Warning, message, code, context);
}

static String Extension(const String& path)
{
	int slash = max(path.ReverseFind('/'), path.ReverseFind('\\'));
	int dot = path.ReverseFind('.');
	return dot > slash ? ToLower(path.Mid(dot)) : String();
}

static bool IsHDR(const String& extension)
{
	return extension == ".hdr" || extension == ".rgbe";
}

static bool IsSupportedExtension(const String& extension)
{
	return extension == ".exr" || extension == ".png" ||
	       extension == ".jxl" || IsHDR(extension) ||
	       extension == ".dpx" || extension == ".cin";
}

static bool RequiresZeroOrigin(const String& extension)
{
	return extension == ".png" || extension == ".jxl" || IsHDR(extension);
}

static const char* FormatName(const String& extension)
{
	if(extension == ".exr") return "EXR";
	if(extension == ".png") return "PNG";
	if(extension == ".jxl") return "JPEG XL";
	if(IsHDR(extension)) return "Radiance HDR/RGBE";
	if(extension == ".dpx") return "DPX";
	if(extension == ".cin") return "Cineon";
	return "image";
}

static bool TypeToCore(const TypeDesc& type, SampleType& out)
{
	if(type == TypeDesc::UINT8)  { out = SampleType::UInt8;   return true; }
	if(type == TypeDesc::UINT16) { out = SampleType::UInt16;  return true; }
	if(type == TypeDesc::HALF)   { out = SampleType::Float16; return true; }
	if(type == TypeDesc::FLOAT)  { out = SampleType::Float32; return true; }
	return false;
}

static TypeDesc CoreToType(SampleType type)
{
	switch(type) {
	case SampleType::UInt8:   return TypeDesc::UINT8;
	case SampleType::UInt16:  return TypeDesc::UINT16;
	case SampleType::Float16: return TypeDesc::HALF;
	case SampleType::Float32: return TypeDesc::FLOAT;
	default:                  return TypeDesc::UNKNOWN;
	}
}

static bool IsName(const String& name, const char* expected)
{
	return ToLower(name) == expected;
}

static bool IsAlphaName(const String& name)
{
	String value = ToLower(name);
	return value == "a" || value == "alpha";
}

static bool IsGrayName(const String& name)
{
	String value = ToLower(name);
	return value == "y" || value == "l" || value == "i" ||
	       value == "gray" || value == "grey";
}

static bool Classify(ImageSpec& spec)
{
	if(spec.channel_names.GetCount() != spec.channels)
		return false;
	for(const String& name : spec.channel_names)
		if(name.IsEmpty())
			return false;

	int alpha = spec.alpha_channel;
	if(alpha < -1 || alpha >= spec.channels)
		return false;

	if(spec.channels == 1) {
		if(alpha == 0 || IsAlphaName(spec.channel_names[0])) {
			spec.channel_layout = ChannelLayout::MultiChannel;
			spec.alpha_channel = 0;
		}
		else {
			spec.channel_layout = ChannelLayout::Gray;
			spec.alpha_channel = -1;
		}
		return true;
	}

	if(spec.channels == 2 &&
	   IsGrayName(spec.channel_names[0]) &&
	   (alpha == 1 || IsAlphaName(spec.channel_names[1]))) {
		spec.channel_layout = ChannelLayout::GrayAlpha;
		spec.alpha_channel = 1;
		return true;
	}

	if(spec.channels == 3 && alpha < 0 &&
	   IsName(spec.channel_names[0], "r") &&
	   IsName(spec.channel_names[1], "g") &&
	   IsName(spec.channel_names[2], "b")) {
		spec.channel_layout = ChannelLayout::RGB;
		spec.alpha_channel = -1;
		return true;
	}

	if(spec.channels == 4 && (alpha < 0 || alpha == 3) &&
	   IsName(spec.channel_names[0], "r") &&
	   IsName(spec.channel_names[1], "g") &&
	   IsName(spec.channel_names[2], "b") &&
	   IsAlphaName(spec.channel_names[3])) {
		spec.channel_layout = ChannelLayout::RGBA;
		spec.alpha_channel = 3;
		return true;
	}

	spec.channel_layout = ChannelLayout::MultiChannel;
	return true;
}

static bool RestorePrimaryImage(ImageInput& input, OIIO::ImageSpec& primary,
                                Diagnostics* diagnostics, const String& path)
{
	if(input.seek_subimage(0, 0, primary))
		return true;
	String error = input.geterror().c_str();
	if(error.IsEmpty())
		error = "unable to restore primary image after structure inspection";
	if(diagnostics)
		diagnostics->Error(error, "IMGIO_STRUCTURE", path);
	return false;
}

static Result InspectStructure(ImageInput& input, OIIO::ImageSpec& primary,
                               Diagnostics* diagnostics, const String& path)
{
	OIIO::ImageSpec probe;
	bool multipart = input.seek_subimage(1, 0, probe);
	if(!multipart)
		input.geterror();
	if(!RestorePrimaryImage(input, primary, diagnostics, path))
		return Result::Failure(ResultCode::IOError,
		                       "unable to restore primary image", path);
	if(multipart)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "multipart images are unsupported", path,
		            "IMGIO_STRUCTURE");

	bool mipmapped = input.seek_subimage(0, 1, probe);
	if(!mipmapped)
		input.geterror();
	if(!RestorePrimaryImage(input, primary, diagnostics, path))
		return Result::Failure(ResultCode::IOError,
		                       "unable to restore primary image", path);
	if(mipmapped)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "mipmapped images are unsupported", path,
		            "IMGIO_STRUCTURE");

	if(primary.deep || primary.depth != 1 || primary.z != 0)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "deep and volume images are unsupported", path,
		            "IMGIO_STRUCTURE");
	if(primary.width <= 0 || primary.height <= 0 || primary.nchannels <= 0)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "image dimensions and channels must be positive", path,
		            "IMGIO_STRUCTURE");
	return Result::Success();
}

static bool ResolveSampleType(const OIIO::ImageSpec& source,
                              TypeDesc& backend_type, SampleType& core_type)
{
	backend_type = source.format;
	if(!source.channelformats.empty()) {
		backend_type = source.channelformats[0];
		for(const TypeDesc& type : source.channelformats)
			if(type != backend_type)
				return false;
	}
	return TypeToCore(backend_type, core_type);
}

static bool AddAttribute(const ParamValue& attribute, Metadata& metadata,
                         Diagnostics* diagnostics)
{
	const TypeDesc& type = attribute.type();
	String key = attribute.name().c_str();

	if(type == TypeDesc::STRING) {
		metadata.Set(key, Value(attribute.get_string()));
		return true;
	}
	if(type == TypeDesc::INT) {
		metadata.Set(key, Value(attribute.get<int>()));
		return true;
	}
	if(type == TypeDesc::UINT) {
		metadata.Set(key, Value((int64)attribute.get<unsigned int>()));
		return true;
	}
	if(type == TypeDesc::INT64) {
		metadata.Set(key, Value((int64)attribute.get<int64_t>()));
		return true;
	}
	if(type == TypeDesc::UINT64) {
		uint64_t value = attribute.get<uint64_t>();
		if(value > (uint64_t)INT64_MAX) {
			Warn(diagnostics, "unsigned metadata exceeds Core int64 range and was omitted",
			     key, "IMGIO_METADATA");
			return false;
		}
		metadata.Set(key, Value((int64)value));
		return true;
	}
	if(type == TypeDesc::FLOAT || type == TypeDesc::DOUBLE) {
		metadata.Set(key, Value(attribute.get<double>()));
		return true;
	}

	if(type.basevalues() > 1 && type.basetype == TypeDesc::INT) {
		ValueArray values;
		for(size_t i = 0; i < type.basevalues(); ++i)
			values.Add(attribute.get<int>((int)i));
		metadata.Set(key, Value(values));
		return true;
	}
	if(type.basevalues() > 1 && type.basetype == TypeDesc::INT64) {
		ValueArray values;
		for(size_t i = 0; i < type.basevalues(); ++i)
			values.Add((int64)attribute.get<int64_t>((int)i));
		metadata.Set(key, Value(values));
		return true;
	}
	if(type.basevalues() > 1 &&
	   (type.basetype == TypeDesc::FLOAT || type.basetype == TypeDesc::DOUBLE)) {
		ValueArray values;
		for(size_t i = 0; i < type.basevalues(); ++i)
			values.Add(attribute.get<double>((int)i));
		metadata.Set(key, Value(values));
		return true;
	}
	if(type.arraylen > 0 && type.basetype == TypeDesc::STRING) {
		ValueArray values;
		for(int i = 0; i < type.arraylen; ++i)
			values.Add(String(attribute.get_string_indexed(i)));
		metadata.Set(key, Value(values));
		return true;
	}

	Warn(diagnostics, "unsupported metadata type was omitted", key,
	     "IMGIO_METADATA");
	return false;
}

static void AddMetadata(const OIIO::ImageSpec& source, Metadata& metadata,
                        Diagnostics* diagnostics)
{
	for(const ParamValue& attribute : source.extra_attribs)
		AddAttribute(attribute, metadata, diagnostics);
}

static bool IsBackendManagedMetadata(const String& key)
{
	String value = ToLower(key);
	static const char* names[] = {
		"width", "height", "depth", "full_x", "full_y", "full_z",
		"full_width", "full_height", "full_depth", "nchannels",
		"channelnames", "alpha_channel", "z_channel", "deep",
		"oiio:bitspersample", "oiio:bitsperchannel", "oiio:subimages",
		"oiio:miplevel", "oiio:unassociatedalpha", "compression",
		"lineorder", "chunkcount", "tiles", "tile_width", "tile_height",
		"tile_depth"
	};
	for(const char* name : names)
		if(value == name)
			return true;
	return false;
}

static bool ToBackendAttribute(const String& key, const Value& value,
                               OIIO::ImageSpec& target,
                               Diagnostics* diagnostics)
{
	if(IsBackendManagedMetadata(key)) {
		Warn(diagnostics, "backend-managed metadata was not re-emitted", key,
		     "IMGIO_METADATA");
		return false;
	}

	if(value.Is<String>()) {
		String text = value.To<String>();
		target.attribute(key.Begin(), text.Begin());
		return true;
	}
	if(value.Is<int>()) {
		target.attribute(key.Begin(), value.To<int>());
		return true;
	}
	if(value.Is<int64>()) {
		int64 number = value.To<int64>();
		if(number < INT_MIN || number > INT_MAX) {
			Warn(diagnostics,
			     "int64 metadata exceeds the portable backend integer range and was omitted",
			     key, "IMGIO_METADATA");
			return false;
		}
		target.attribute(key.Begin(), (int)number);
		return true;
	}
	if(value.Is<double>()) {
		double number = value.To<double>();
		target.attribute(key.Begin(), TypeDesc::DOUBLE, &number);
		return true;
	}
	if(value.Is<bool>()) {
		Warn(diagnostics, "boolean metadata is not emitted by ImagingIO", key,
		     "IMGIO_METADATA");
		return false;
	}
	if(value.Is<ValueArray>()) {
		ValueArray array = value;
		if(array.IsEmpty()) {
			Warn(diagnostics, "empty metadata array was omitted", key,
			     "IMGIO_METADATA");
			return false;
		}

		if(array[0].Is<int>()) {
			Vector<int> values;
			for(const Value& item : array) {
				if(!item.Is<int>()) {
					Warn(diagnostics, "heterogeneous metadata array was omitted", key,
					     "IMGIO_METADATA");
					return false;
				}
				values.Add(item.To<int>());
			}
			target.attribute(key.Begin(), TypeDesc(TypeDesc::INT, values.GetCount()),
			                 values.Begin());
			return true;
		}
		if(array[0].Is<int64>()) {
			Vector<int> values;
			for(const Value& item : array) {
				if(!item.Is<int64>()) {
					Warn(diagnostics, "heterogeneous metadata array was omitted", key,
					     "IMGIO_METADATA");
					return false;
				}
				int64 number = item.To<int64>();
				if(number < INT_MIN || number > INT_MAX) {
					Warn(diagnostics,
					     "int64 metadata array exceeds the portable backend integer range and was omitted",
					     key, "IMGIO_METADATA");
					return false;
				}
				values.Add((int)number);
			}
			target.attribute(key.Begin(), TypeDesc(TypeDesc::INT, values.GetCount()),
			                 values.Begin());
			return true;
		}
		if(array[0].Is<double>()) {
			Vector<double> values;
			for(const Value& item : array) {
				if(!item.Is<double>()) {
					Warn(diagnostics, "heterogeneous metadata array was omitted", key,
					     "IMGIO_METADATA");
					return false;
				}
				values.Add(item.To<double>());
			}
			target.attribute(key.Begin(), TypeDesc(TypeDesc::DOUBLE, values.GetCount()),
			                 values.Begin());
			return true;
		}

		Warn(diagnostics, "unsupported metadata array was omitted", key,
		     "IMGIO_METADATA");
		return false;
	}

	Warn(diagnostics, "unsupported metadata value was omitted", key,
	     "IMGIO_METADATA");
	return false;
}

static void SetCanonicalChannelNames(const ImageSpec& source,
                                     OIIO::ImageSpec& target,
                                     Diagnostics* diagnostics)
{
	Vector<String> expected;
	switch(source.channel_layout) {
	case ChannelLayout::Gray:
		expected.Add("Y");
		break;
	case ChannelLayout::GrayAlpha:
		expected.Add("Y"); expected.Add("A");
		break;
	case ChannelLayout::RGB:
		expected.Add("R"); expected.Add("G"); expected.Add("B");
		break;
	case ChannelLayout::RGBA:
		expected.Add("R"); expected.Add("G"); expected.Add("B"); expected.Add("A");
		break;
	case ChannelLayout::MultiChannel:
		for(const String& name : source.channel_names)
			expected.Add(name);
		break;
	default:
		break;
	}

	bool compatible = source.channel_names.IsEmpty();
	if(source.channel_names.GetCount() == expected.GetCount()) {
		compatible = true;
		for(int i = 0; i < expected.GetCount(); ++i) {
			if(source.channel_layout == ChannelLayout::Gray)
				compatible = compatible && IsGrayName(source.channel_names[i]);
			else if(source.channel_layout == ChannelLayout::GrayAlpha)
				compatible = compatible && (i == 0 ? IsGrayName(source.channel_names[i])
				                                  : IsAlphaName(source.channel_names[i]));
			else
				compatible = compatible && ToLower(source.channel_names[i]) == ToLower(expected[i]);
		}
	}

	if(!source.channel_names.IsEmpty() && !compatible)
		Warn(diagnostics, "contradictory canonical channel names were replaced",
		     "channel_names", "IMGIO_CHANNELS");

	target.channelnames.clear();
	const Vector<String>& names = compatible && !source.channel_names.IsEmpty()
	                            ? source.channel_names : expected;
	for(const String& name : names)
		target.channelnames.emplace_back(name.Begin());
}

static Result ValidateLoadedPolicy(const String& extension,
                                   const ImageSpec& spec,
                                   Diagnostics* diagnostics,
                                   const String& path)
{
	if((extension == ".png" || extension == ".jxl") &&
	   spec.channel_layout == ChannelLayout::MultiChannel)
		return Fail(ResultCode::Unsupported, diagnostics,
		            String(FormatName(extension)) +
		            " arbitrary multichannel images are unsupported",
		            path, "IMGIO_CHANNELS");

	if(extension == ".jxl" && spec.channel_layout == ChannelLayout::GrayAlpha)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "JPEG XL GrayAlpha is deferred until the backend exposes its alpha semantics reliably",
		            path, "IMGIO_CHANNELS");

	if(IsHDR(extension)) {
		if(spec.sample_type != SampleType::Float32)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "Radiance HDR/RGBE input must decode as Float32",
			            path, "IMGIO_SAMPLE");
		if(spec.channel_layout != ChannelLayout::RGB)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "Radiance HDR/RGBE supports RGB images only",
			            path, "IMGIO_CHANNELS");
	}

	if(extension == ".dpx") {
		if(spec.sample_type != SampleType::UInt8 &&
		   spec.sample_type != SampleType::UInt16)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "the initial DPX slice supports UInt8 and UInt16 only",
			            path, "IMGIO_SAMPLE");
		if(spec.channel_layout != ChannelLayout::RGB)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "the initial DPX slice supports RGB images only",
			            path, "IMGIO_CHANNELS");
	}

	if(extension == ".cin") {
		if(spec.sample_type != SampleType::UInt8 &&
		   spec.sample_type != SampleType::UInt16)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "the initial Cineon slice supports UInt8 and UInt16 input only",
			            path, "IMGIO_SAMPLE");
		if(spec.channel_layout != ChannelLayout::Gray &&
		   spec.channel_layout != ChannelLayout::RGB)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "the initial Cineon slice supports Gray and RGB input only",
			            path, "IMGIO_CHANNELS");
	}

	return Result::Success();
}

static Result ValidateSavePolicy(const String& extension,
                                 const ImageData& image,
                                 Diagnostics* diagnostics,
                                 const String& path)
{
	if(extension == ".cin")
		return Fail(ResultCode::Unsupported, diagnostics,
		            "Cineon output is not supported by the bundled OpenImageIO backend",
		            path, "IMGIO_FORMAT");

	if(extension == ".exr" &&
	   image.spec.sample_type != SampleType::Float16 &&
	   image.spec.sample_type != SampleType::Float32)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "EXR output supports Float16 and Float32 only", path,
		            "IMGIO_SAMPLE");

	if(extension == ".png" &&
	   image.spec.sample_type != SampleType::UInt8 &&
	   image.spec.sample_type != SampleType::UInt16)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "PNG output supports UInt8 and UInt16 only", path,
		            "IMGIO_SAMPLE");

	if(RequiresZeroOrigin(extension) &&
	   (image.spec.data_window.left != 0 || image.spec.data_window.top != 0))
		return Fail(ResultCode::Unsupported, diagnostics,
		            String(FormatName(extension)) +
		            " output requires a zero data-window origin",
		            path, "IMGIO_SPEC");

	if((extension == ".png" || extension == ".jxl") &&
	   image.spec.channel_layout == ChannelLayout::MultiChannel)
		return Fail(ResultCode::Unsupported, diagnostics,
		            String(FormatName(extension)) +
		            " arbitrary multichannel output is unsupported",
		            path, "IMGIO_CHANNELS");

	if(extension == ".jxl" &&
	   image.spec.channel_layout == ChannelLayout::GrayAlpha)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "JPEG XL GrayAlpha output is deferred until the backend exposes its alpha semantics reliably",
		            path, "IMGIO_CHANNELS");

	if(IsHDR(extension)) {
		if(image.spec.sample_type != SampleType::Float32)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "Radiance HDR/RGBE output requires Float32 samples",
			            path, "IMGIO_SAMPLE");
		if(image.spec.channel_layout != ChannelLayout::RGB)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "Radiance HDR/RGBE output supports RGB only",
			            path, "IMGIO_CHANNELS");
	}

	if(extension == ".dpx") {
		if(image.spec.sample_type != SampleType::UInt8 &&
		   image.spec.sample_type != SampleType::UInt16)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "the initial DPX output slice supports UInt8 and UInt16 only",
			            path, "IMGIO_SAMPLE");
		if(image.spec.channel_layout != ChannelLayout::RGB)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "the initial DPX output slice supports RGB only",
			            path, "IMGIO_CHANNELS");
		if(image.spec.data_window.left < 0 || image.spec.data_window.top < 0)
			return Fail(ResultCode::Unsupported, diagnostics,
			            "DPX output requires a non-negative data-window origin",
			            path, "IMGIO_SPEC");
	}

	return Result::Success();
}

static String TemporaryPath(const String& path, const String& extension,
                            unsigned serial)
{
	std::filesystem::path final_path(path.Begin());
	std::filesystem::path directory = final_path.parent_path();
	std::string filename = final_path.filename().string();
	std::string temporary = filename + ".imagingio-" +
	                        std::to_string(serial) + extension.Begin();
	return (directory / temporary).string().c_str();
}

static Result WriteTemporaryFile(const String& temporary,
                                 const OIIO::ImageSpec& target,
                                 const ImageData& image,
                                 Diagnostics* diagnostics,
                                 const String& final_path)
{
	ImageOutput::unique_ptr output = ImageOutput::create(temporary.Begin());
	if(!output)
		return Fail(ResultCode::IOError, diagnostics,
		            OIIO::geterror().c_str(), final_path, "IMGIO_WRITE");
	if(!output->open(temporary.Begin(), target))
		return Fail(ResultCode::IOError, diagnostics,
		            output->geterror().c_str(), final_path, "IMGIO_WRITE");
	if(!output->write_image(CoreToType(image.spec.sample_type),
	                        image.buffer.Begin()))
		return Fail(ResultCode::IOError, diagnostics,
		            output->geterror().c_str(), final_path, "IMGIO_WRITE");
	if(!output->close())
		return Fail(ResultCode::IOError, diagnostics,
		            output->geterror().c_str(), final_path, "IMGIO_CLEANUP");
	output.reset();
	return Result::Success();
}

static Result VerifyTemporaryFile(const String& temporary,
                                  const ImageData& image,
                                  Diagnostics* diagnostics,
                                  const String& final_path)
{
	ImageInput::unique_ptr input = ImageInput::open(temporary.Begin());
	if(!input)
		return Fail(ResultCode::IOError, diagnostics,
		            OIIO::geterror().c_str(), final_path, "IMGIO_VERIFY");

	OIIO::ImageSpec source = input->spec(0, 0);
	int64 width;
	int64 height;
	int64 bytes;
	if(!image.spec.GetWidth(width) || !image.spec.GetHeight(height) ||
	   !image.spec.GetByteCount(bytes) || bytes < 0 || bytes > INT_MAX)
		return Fail(ResultCode::Overflow, diagnostics,
		            "temporary image verification size cannot be represented",
		            final_path, "IMGIO_VERIFY");

	TypeDesc backend_type;
	SampleType sample_type;
	if(source.width != width || source.height != height ||
	   source.nchannels != image.spec.channels ||
	   source.x != image.spec.data_window.left ||
	   source.y != image.spec.data_window.top ||
	   !ResolveSampleType(source, backend_type, sample_type) ||
	   sample_type != image.spec.sample_type)
		return Fail(ResultCode::IOError, diagnostics,
		            "temporary image specification differs from the requested image",
		            final_path, "IMGIO_VERIFY");

	Vector<byte> decoded;
	decoded.SetCount((int)bytes);
	if(!input->read_image(0, 0, 0, source.nchannels, backend_type,
	                      decoded.Begin(), AutoStride, AutoStride, AutoStride))
		return Fail(ResultCode::IOError, diagnostics,
		            input->geterror().c_str(), final_path, "IMGIO_VERIFY");
	if(!input->close())
		return Fail(ResultCode::IOError, diagnostics,
		            input->geterror().c_str(), final_path, "IMGIO_VERIFY");
	input.reset();
	return Result::Success();
}

static bool RemovePath(const std::filesystem::path& path,
                       Diagnostics* diagnostics, const char* code)
{
	std::error_code error;
	if(!std::filesystem::exists(path, error))
		return true;
	if(std::filesystem::remove(path, error))
		return true;
	Warn(diagnostics, "temporary path cleanup failed",
	     path.string().c_str(), code);
	return false;
}

} // namespace

Result LoadImageFile(const String& path, ImageData& output,
                     Diagnostics* diagnostics)
{
	Reset(diagnostics);
	if(path.IsEmpty())
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "image path is empty", "path", "IMGIO_PATH");

	String extension = Extension(path);
	if(!IsSupportedExtension(extension))
		return Fail(ResultCode::Unsupported, diagnostics,
		            "unsupported image format", path, "IMGIO_FORMAT");

	UppImaging::InitializeOpenImageIO();
	OIIO::ImageSpec read_config;
	if(extension == ".png")
		read_config.attribute("oiio:UnassociatedAlpha", 1);
	ImageInput::unique_ptr input = ImageInput::open(path.Begin(), &read_config);
	if(!input)
		return Fail(ResultCode::IOError, diagnostics,
		            OIIO::geterror().c_str(), path, "IMGIO_OPEN");

	OIIO::ImageSpec source = input->spec(0, 0);
	if(source.format == TypeDesc::UNKNOWN)
		return Fail(ResultCode::IOError, diagnostics,
		            "unable to inspect image specification", path,
		            "IMGIO_SPEC");

	Result structure = InspectStructure(*input, source, diagnostics, path);
	if(!structure)
		return structure;

	TypeDesc backend_type;
	SampleType sample_type;
	if(!ResolveSampleType(source, backend_type, sample_type))
		return Fail(ResultCode::Unsupported, diagnostics,
		            "mixed or unsupported channel sample formats are unsupported",
		            path, "IMGIO_SAMPLE");

	if(RequiresZeroOrigin(extension) && (source.x != 0 || source.y != 0))
		return Fail(ResultCode::Unsupported, diagnostics,
		            String(FormatName(extension)) + " data-window origin must be zero",
		            path, "IMGIO_SPEC");

	int64 right = (int64)source.x + (int64)source.width - 1;
	int64 bottom = (int64)source.y + (int64)source.height - 1;
	int64 xend = (int64)source.x + (int64)source.width;
	int64 yend = (int64)source.y + (int64)source.height;
	if(right < INT_MIN || right > INT_MAX ||
	   bottom < INT_MIN || bottom > INT_MAX ||
	   xend < INT_MIN || xend > INT_MAX ||
	   yend < INT_MIN || yend > INT_MAX)
		return Fail(ResultCode::Overflow, diagnostics,
		            "image coordinates cannot be represented", path,
		            "IMGIO_SPEC");

	ImageData candidate;
	candidate.spec.data_window = {source.x, source.y, (int)right, (int)bottom};
	candidate.spec.depth = 1;
	candidate.spec.channels = source.nchannels;
	candidate.spec.sample_type = sample_type;
	candidate.spec.alpha_channel = source.alpha_channel;
	for(const std::string& name : source.channelnames)
		candidate.spec.channel_names.Add(name.c_str());

	if(!Classify(candidate.spec))
		return Fail(ResultCode::Unsupported, diagnostics,
		            "channel names or alpha index are not representable", path,
		            "IMGIO_CHANNELS");

	Result policy = ValidateLoadedPolicy(extension, candidate.spec,
	                                   diagnostics, path);
	if(!policy)
		return policy;

	if(!candidate.spec.IsValid())
		return Fail(ResultCode::InvalidSpecification, diagnostics,
		            "backend image specification is not representable", path,
		            "IMGIO_SPEC");
	if(!candidate.buffer.Allocate(candidate.spec))
		return Fail(ResultCode::AllocationFailure, diagnostics,
		            "unable to allocate image pixels", path,
		            "IMGIO_PIXELS");

	int64 expected_bytes;
	if(!candidate.spec.GetByteCount(expected_bytes) ||
	   expected_bytes != candidate.buffer.GetByteCount())
		return Fail(ResultCode::InternalFailure, diagnostics,
		            "allocated pixel size does not match the image specification",
		            path, "IMGIO_SPEC");

	if(!input->read_image(0, 0, 0, source.nchannels, backend_type,
	                      candidate.buffer.Begin(), AutoStride, AutoStride,
	                      AutoStride))
		return Fail(ResultCode::IOError, diagnostics,
		            input->geterror().c_str(), path, "IMGIO_PIXELS");

	AddMetadata(source, candidate.metadata, diagnostics);
	if(!candidate.IsValid())
		return Fail(ResultCode::InternalFailure, diagnostics,
		            "loaded image failed Core validation", path,
		            "IMGIO_SPEC");

	if(!input->close())
		return Fail(ResultCode::IOError, diagnostics,
		            input->geterror().c_str(), path, "IMGIO_CLEANUP");
	input.reset();

	output = pick(candidate);
	return Result::Success();
}

Result SaveImageFile(const String& path, const ImageData& image,
                     Diagnostics* diagnostics)
{
	Reset(diagnostics);
	if(path.IsEmpty())
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "image path is empty", "path", "IMGIO_PATH");

	String extension = Extension(path);
	if(!IsSupportedExtension(extension))
		return Fail(ResultCode::Unsupported, diagnostics,
		            "unsupported image format", path, "IMGIO_FORMAT");
	if(!image.IsValid())
		return Fail(ResultCode::InvalidSpecification, diagnostics,
		            "image data is invalid", path, "IMGIO_SPEC");

	Result policy = ValidateSavePolicy(extension, image, diagnostics, path);
	if(!policy)
		return policy;

	int64 width;
	int64 height;
	int64 bytes;
	if(!image.spec.GetWidth(width) || !image.spec.GetHeight(height) ||
	   !image.spec.GetByteCount(bytes) ||
	   width > INT_MAX || height > INT_MAX ||
	   bytes != image.buffer.GetByteCount())
		return Fail(ResultCode::Overflow, diagnostics,
		            "image dimensions or byte count cannot be represented",
		            path, "IMGIO_SPEC");

	OIIO::ImageSpec target((int)width, (int)height, image.spec.channels,
	                       CoreToType(image.spec.sample_type));
	target.x = image.spec.data_window.left;
	target.y = image.spec.data_window.top;
	target.z = 0;
	target.depth = 1;
	target.alpha_channel = image.spec.alpha_channel;
	if(extension == ".png" && target.alpha_channel != -1)
		target.attribute("oiio:UnassociatedAlpha", 1);
	if(extension == ".jxl")
		target.attribute("compression", "jpegxl:100");
	SetCanonicalChannelNames(image.spec, target, diagnostics);
	for(int i = 0; i < image.metadata.Items().GetCount(); ++i)
		ToBackendAttribute(image.metadata.Items().GetKey(i),
		                   image.metadata.Items()[i], target, diagnostics);

	UppImaging::InitializeOpenImageIO();
	static std::atomic<unsigned> serial{0};
	unsigned transaction = ++serial;
	String temporary = TemporaryPath(path, extension, transaction);
	std::filesystem::path final_path(path.Begin());
	std::filesystem::path temporary_path(temporary.Begin());
	std::filesystem::path backup_path = final_path;
	backup_path += ".imagingio-backup-" + std::to_string(transaction);

	Result write = WriteTemporaryFile(temporary, target, image, diagnostics, path);
	if(!write) {
		RemovePath(temporary_path, diagnostics, "IMGIO_CLEANUP");
		return write;
	}

	Result verify = VerifyTemporaryFile(temporary, image, diagnostics, path);
	if(!verify) {
		RemovePath(temporary_path, diagnostics, "IMGIO_CLEANUP");
		return verify;
	}

	std::error_code error;
	bool target_exists = std::filesystem::exists(final_path, error);
	if(error) {
		RemovePath(temporary_path, diagnostics, "IMGIO_CLEANUP");
		return Fail(ResultCode::IOError, diagnostics,
		            "unable to inspect destination", path, "IMGIO_REPLACE");
	}
	if(target_exists && !std::filesystem::is_regular_file(final_path, error)) {
		RemovePath(temporary_path, diagnostics, "IMGIO_CLEANUP");
		return Fail(ResultCode::Unsupported, diagnostics,
		            "destination exists but is not a regular file", path,
		            "IMGIO_REPLACE");
	}

	if(target_exists) {
		std::filesystem::rename(final_path, backup_path, error);
		if(error) {
			RemovePath(temporary_path, diagnostics, "IMGIO_CLEANUP");
			return Fail(ResultCode::IOError, diagnostics,
			            "unable to stage the existing destination", path,
			            "IMGIO_REPLACE");
		}
	}

	error.clear();
	std::filesystem::rename(temporary_path, final_path, error);
	if(error) {
		String primary = "unable to promote the completed temporary image";
		if(target_exists) {
			std::error_code restore_error;
			std::filesystem::rename(backup_path, final_path, restore_error);
			if(restore_error)
				Warn(diagnostics, "destination restoration failed",
				     final_path.string().c_str(), "IMGIO_REPLACE");
		}
		RemovePath(temporary_path, diagnostics, "IMGIO_CLEANUP");
		return Fail(ResultCode::IOError, diagnostics, primary, path,
		            "IMGIO_REPLACE");
	}

	if(target_exists)
		RemovePath(backup_path, diagnostics, "IMGIO_CLEANUP");
	return Result::Success();
}

} // namespace Imaging
} // namespace Upp
