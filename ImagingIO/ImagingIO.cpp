#include "ImagingIO.h"

#include <OpenImageIO/OIIO.h>

#include <cstring>
#include <filesystem>
#include <atomic>

namespace Upp {
namespace Imaging {

using namespace OIIO;

static void Reset(Diagnostics* d) { if(d) d->Clear(); }
static Result Fail(ResultCode code, Diagnostics* d, String message, String context, const char* diagnostic)
{
	if(d) d->Error(message, diagnostic, context);
	return Result::Failure(code, message, context);
}
static void Warn(Diagnostics* d, String message, String context, const char* code) { if(d) d->Add(DiagnosticSeverity::Warning, message, code, context); }

static String Extension(const String& path)
{
	int slash = max(path.ReverseFind('/'), path.ReverseFind('\\'));
	int dot = path.ReverseFind('.');
	return dot > slash ? ToLower(path.Mid(dot)) : String();
}

static bool TypeToCore(const TypeDesc& type, SampleType& out)
{
	if(type == TypeDesc::UINT8) { out = SampleType::UInt8; return true; }
	if(type == TypeDesc::UINT16) { out = SampleType::UInt16; return true; }
	if(type == TypeDesc::HALF) { out = SampleType::Float16; return true; }
	if(type == TypeDesc::FLOAT) { out = SampleType::Float32; return true; }
	return false;
}

static TypeDesc CoreToType(SampleType type)
{
	switch(type) {
	case SampleType::UInt8: return TypeDesc::UINT8;
	case SampleType::UInt16: return TypeDesc::UINT16;
	case SampleType::Float16: return TypeDesc::HALF;
	case SampleType::Float32: return TypeDesc::FLOAT;
	default: return TypeDesc::UNKNOWN;
	}
}

static bool IsAlphaName(const String& name) { String n = ToLower(name); return n == "a" || n == "alpha"; }
static bool IsName(const String& name, const char* expected) { return ToLower(name) == expected; }

static void Classify(ImageSpec& spec)
{
	int alpha = spec.alpha_channel;
	if(spec.channels == 1) { if(alpha == 0 || IsAlphaName(spec.channel_names[0])) spec.channel_layout = ChannelLayout::MultiChannel; else { spec.channel_layout = ChannelLayout::Gray; spec.alpha_channel = -1; } return; }
	if(spec.channels == 2 && (alpha == 1 || IsAlphaName(spec.channel_names[1]))) { spec.channel_layout = ChannelLayout::GrayAlpha; spec.alpha_channel = 1; return; }
	if(spec.channels == 3 && alpha < 0 && IsName(spec.channel_names[0], "r") && IsName(spec.channel_names[1], "g") && IsName(spec.channel_names[2], "b")) { spec.channel_layout = ChannelLayout::RGB; spec.alpha_channel = -1; return; }
	if(spec.channels == 4 && (alpha < 0 || alpha == 3) && IsName(spec.channel_names[0], "r") && IsName(spec.channel_names[1], "g") && IsName(spec.channel_names[2], "b") && IsAlphaName(spec.channel_names[3])) { spec.channel_layout = ChannelLayout::RGBA; spec.alpha_channel = 3; return; }
	spec.channel_layout = ChannelLayout::MultiChannel;
}

static bool AddAttribute(const ParamValue& attribute, Metadata& metadata, Diagnostics* diagnostics)
{
	const TypeDesc& type = attribute.type();
	String key = attribute.name().c_str();
	if(type == TypeDesc::STRING) { metadata.Set(key, Value(attribute.get_string())); return true; }
	if(type == TypeDesc::INT) { metadata.Set(key, Value(attribute.get<int>())); return true; }
	if(type == TypeDesc::INT64) { metadata.Set(key, Value((int64)attribute.get<int64_t>())); return true; }
	if(type == TypeDesc::FLOAT || type == TypeDesc::DOUBLE) { metadata.Set(key, Value(attribute.get<double>())); return true; }
	if(type.arraylen > 0 && type.basetype == TypeDesc::INT) {
		ValueArray array;
		for(int i = 0; i < type.arraylen; i++) array.Add(attribute.get<int>(i));
		metadata.Set(key, Value(array));
		return true;
	}
	if(type.arraylen > 0 && (type.basetype == TypeDesc::FLOAT || type.basetype == TypeDesc::DOUBLE)) {
		ValueArray array;
		for(int i = 0; i < type.arraylen; i++) array.Add(attribute.get<double>(i));
		metadata.Set(key, Value(array));
		return true;
	}
	Warn(diagnostics, "unsupported metadata omitted", key, "IMGIO_METADATA");
	return false;
}

static void AddMetadata(const OIIO::ImageSpec& source, Metadata& metadata, Diagnostics* diagnostics)
{
	for(const ParamValue& attribute : source.extra_attribs) AddAttribute(attribute, metadata, diagnostics);
}

static bool ToBackendAttribute(const String& key, const Value& value, OIIO::ImageSpec& target, Diagnostics* diagnostics)
{
	static const char* reserved[] = {"width", "height", "depth", "nchannels", "channelnames", "alpha_channel", "PixelAspectRatio", "screenWindowWidth", "screenWindowCenter", "compression", "lineOrder"};
	for(const char* name : reserved) if(key == name) { Warn(diagnostics, "backend-managed metadata omitted", key, "IMGIO_METADATA"); return false; }
	if(value.Is<String>()) target.attribute(key.Begin(), value.To<String>().Begin());
	else if(value.Is<int>()) target.attribute(key.Begin(), value.To<int>());
	else if(value.Is<int64>()) { int64_t v = value.To<int64>(); if(v < INT_MIN || v > INT_MAX) { Warn(diagnostics, "int64 metadata exceeds backend INT range and was omitted", key, "IMGIO_METADATA"); return false; } target.attribute(key.Begin(), (int)v); }
	else if(value.Is<double>()) { double v = value.To<double>(); target.attribute(key.Begin(), TypeDesc::DOUBLE, &v); }
	else if(value.Is<ValueArray>()) {
		ValueArray array = value.To<ValueArray>();
		Vector<double> numbers;
		for(const Value& item : array) { if(!item.Is<double>()) { Warn(diagnostics, "unsupported metadata array omitted", key, "IMGIO_METADATA"); return false; } numbers.Add(item.To<double>()); }
		if(!numbers.IsEmpty()) target.attribute(key.Begin(), TypeDesc(TypeDesc::DOUBLE, (int)numbers.GetCount()), numbers.Begin());
	}
	else { Warn(diagnostics, "unsupported metadata omitted", key, "IMGIO_METADATA"); return false; }
	return true;
}

Result LoadImageFile(const String& path, ImageData& output, Diagnostics* diagnostics)
{
	Reset(diagnostics);
	if(path.IsEmpty()) return Fail(ResultCode::InvalidArgument, diagnostics, "image path is empty", "path", "IMGIO_PATH");
	String ext = Extension(path);
	if(ext != ".exr" && ext != ".png") return Fail(ResultCode::Unsupported, diagnostics, "only EXR and PNG are supported", path, "IMGIO_FORMAT");
	UppImaging::InitializeOpenImageIO();
	ImageInput::unique_ptr input = ImageInput::open(path.Begin());
	if(!input) return Fail(ResultCode::IOError, diagnostics, geterror().c_str(), path, "IMGIO_OPEN");
	OIIO::ImageSpec source = input->spec(0, 0);
	if(source.format == TypeDesc::UNKNOWN) return Fail(ResultCode::IOError, diagnostics, "unable to inspect image specification", path, "IMGIO_SPEC");
	if(ext == ".exr") {
		if(input->spec_dimensions(1, 0).format != TypeDesc::UNKNOWN) return Fail(ResultCode::Unsupported, diagnostics, "multipart images are unsupported", path, "IMGIO_STRUCTURE");
		if(input->spec_dimensions(0, 1).format != TypeDesc::UNKNOWN) return Fail(ResultCode::Unsupported, diagnostics, "mipmapped images are unsupported", path, "IMGIO_STRUCTURE");
	}
	if(source.depth != 1 || source.z != 0 || source.deep) return Fail(ResultCode::Unsupported, diagnostics, "deep or volume images are unsupported", path, "IMGIO_STRUCTURE");
	if(source.nchannels <= 0 || source.width <= 0 || source.height <= 0) return Fail(ResultCode::Unsupported, diagnostics, "image has invalid dimensions or channels", path, "IMGIO_STRUCTURE");
	SampleType sample;
	if(!source.channelformats.empty()) {
		for(const TypeDesc& channel_type : source.channelformats) if(channel_type != source.channelformats[0]) return Fail(ResultCode::Unsupported, diagnostics, "mixed channel sample formats are unsupported", path, "IMGIO_SAMPLE");
		if(!TypeToCore(source.channelformats[0], sample)) return Fail(ResultCode::Unsupported, diagnostics, "unsupported sample format", path, "IMGIO_SAMPLE");
	}
	else if(!TypeToCore(source.format, sample)) return Fail(ResultCode::Unsupported, diagnostics, "unsupported sample format", path, "IMGIO_SAMPLE");
	if(ext == ".png" && (source.x != 0 || source.y != 0)) return Fail(ResultCode::Unsupported, diagnostics, "PNG data window origin must be zero", path, "IMGIO_SPEC");
	int64 right64 = (int64)source.x + source.width - 1, bottom64 = (int64)source.y + source.height - 1;
	if(right64 < INT_MIN || right64 > INT_MAX || bottom64 < INT_MIN || bottom64 > INT_MAX) return Fail(ResultCode::Overflow, diagnostics, "data window cannot be represented", path, "IMGIO_SPEC");
	ImageData candidate;
	candidate.spec.data_window = {source.x, source.y, (int)right64, (int)bottom64};
	candidate.spec.depth = 1;
	candidate.spec.channels = source.nchannels;
	candidate.spec.sample_type = sample;
	candidate.spec.alpha_channel = source.alpha_channel;
	candidate.spec.channel_names.Clear();
	for(const std::string& name : source.channelnames) candidate.spec.channel_names.Add(name.c_str());
	Classify(candidate.spec);
	if(!candidate.spec.IsValid()) return Fail(ResultCode::InvalidSpecification, diagnostics, "backend specification is not representable", path, "IMGIO_SPEC");
	if(ext == ".png" && candidate.spec.channel_layout == ChannelLayout::MultiChannel) return Fail(ResultCode::Unsupported, diagnostics, "PNG non-canonical channels are unsupported", path, "IMGIO_CHANNELS");
	if(!candidate.buffer.Allocate(candidate.spec)) return Fail(ResultCode::AllocationFailure, diagnostics, "unable to allocate image pixels", path, "IMGIO_PIXELS");
	int64 xend = (int64)source.x + source.width, yend = (int64)source.y + source.height;
	if(xend > INT_MAX || yend > INT_MAX) return Fail(ResultCode::Overflow, diagnostics, "image coordinates overflow backend range", path, "IMGIO_SPEC");
	if(!input->read_image(0, 0, 0, source.nchannels, source.format, candidate.buffer.Begin(), AutoStride, AutoStride, AutoStride)) return Fail(ResultCode::IOError, diagnostics, input->geterror().c_str(), path, "IMGIO_PIXELS");
	AddMetadata(source, candidate.metadata, diagnostics);
	if(!candidate.IsValid()) return Fail(ResultCode::InternalFailure, diagnostics, "loaded image failed Core validation", path, "IMGIO_SPEC");
	if(!input->close()) return Fail(ResultCode::IOError, diagnostics, input->geterror().c_str(), path, "IMGIO_CLEANUP");
	input.reset();
	output = pick(candidate);
	return Result::Success();
}

Result SaveImageFile(const String& path, const ImageData& image, Diagnostics* diagnostics)
{
	Reset(diagnostics);
	if(path.IsEmpty()) return Fail(ResultCode::InvalidArgument, diagnostics, "image path is empty", "path", "IMGIO_PATH");
	String ext = Extension(path);
	if(ext != ".exr" && ext != ".png") return Fail(ResultCode::Unsupported, diagnostics, "only EXR and PNG are supported", path, "IMGIO_FORMAT");
	if(!image.IsValid()) return Fail(ResultCode::InvalidSpecification, diagnostics, "image data is invalid", path, "IMGIO_SPEC");
	if(ext == ".exr" && image.spec.sample_type != SampleType::Float16 && image.spec.sample_type != SampleType::Float32) return Fail(ResultCode::Unsupported, diagnostics, "EXR output supports Float16 and Float32 only", path, "IMGIO_SAMPLE");
	if(ext == ".png" && (image.spec.sample_type != SampleType::UInt8 && image.spec.sample_type != SampleType::UInt16)) return Fail(ResultCode::Unsupported, diagnostics, "PNG output supports UInt8 and UInt16 only", path, "IMGIO_SAMPLE");
	if(ext == ".png" && (image.spec.data_window.left != 0 || image.spec.data_window.top != 0 || image.spec.channel_layout == ChannelLayout::MultiChannel)) return Fail(ResultCode::Unsupported, diagnostics, "PNG output requires zero origin and canonical channels", path, "IMGIO_SPEC");
	if(image.spec.channel_layout == ChannelLayout::MultiChannel && ext == ".png") return Fail(ResultCode::Unsupported, diagnostics, "PNG multichannel output is unsupported", path, "IMGIO_CHANNELS");
	int64 width, height;
	image.spec.GetWidth(width); image.spec.GetHeight(height);
	if(width > INT_MAX || height > INT_MAX || image.spec.channels <= 0) return Fail(ResultCode::Overflow, diagnostics, "image dimensions cannot be represented by backend", path, "IMGIO_SPEC");
	OIIO::ImageSpec target((int)width, (int)height, image.spec.channels, CoreToType(image.spec.sample_type));
	target.x = image.spec.data_window.left; target.y = image.spec.data_window.top;
	target.channelnames.clear();
	if(image.spec.channel_layout == ChannelLayout::Gray) target.channelnames.emplace_back("Y");
	else if(image.spec.channel_layout == ChannelLayout::GrayAlpha) { target.channelnames.emplace_back("Y"); target.channelnames.emplace_back("A"); }
	else if(image.spec.channel_layout == ChannelLayout::RGB) { target.channelnames = {"R", "G", "B"}; }
	else if(image.spec.channel_layout == ChannelLayout::RGBA) { target.channelnames = {"R", "G", "B", "A"}; }
	else for(const String& name : image.spec.channel_names) target.channelnames.emplace_back(name.Begin());
	target.alpha_channel = image.spec.alpha_channel;
	for(int i = 0; i < image.metadata.Items().GetCount(); i++) ToBackendAttribute(image.metadata.Items().GetKey(i), image.metadata.Items()[i], target, diagnostics);
	OIIO::ImageBuf backend(target, OIIO::InitializePixels::No);
	if(!backend.set_pixels(backend.roi(), CoreToType(image.spec.sample_type), const_cast<byte*>(image.buffer.Begin()), AutoStride, AutoStride, AutoStride)) return Fail(ResultCode::InternalFailure, diagnostics, backend.geterror().c_str(), path, "IMGIO_PIXELS");
	UppImaging::InitializeOpenImageIO();
	std::string error;
	static std::atomic<unsigned> serial{0};
	std::filesystem::path final_path(path.Begin());
	if(std::filesystem::is_directory(final_path)) return Fail(ResultCode::InvalidArgument, diagnostics, "destination is a directory", path, "IMGIO_REPLACE");
	String temp = Format("%s.imagingio-%d%s", path.Begin(), (int)++serial, ext.Begin());
	std::filesystem::path temp_path(temp.Begin());
	if(!UppImaging::SaveImage(temp.Begin(), backend, &error)) { std::error_code ec; std::filesystem::remove(temp_path, ec); return Fail(ResultCode::IOError, diagnostics, error.c_str(), path, "IMGIO_WRITE"); }
	std::error_code ec;
	std::filesystem::path backup_path = final_path; backup_path += ".imagingio-backup";
	bool had_target = std::filesystem::exists(final_path);
	if(had_target && !std::filesystem::is_regular_file(final_path)) { std::filesystem::remove(temp_path, ec); return Fail(ResultCode::InvalidArgument, diagnostics, "destination is not a regular file", path, "IMGIO_REPLACE"); }
	if(had_target) { std::filesystem::remove(backup_path, ec); std::filesystem::rename(final_path, backup_path, ec); if(ec) { std::filesystem::remove(temp_path, ec); return Fail(ResultCode::IOError, diagnostics, "unable to stage existing destination", path, "IMGIO_REPLACE"); } }
	std::filesystem::rename(temp_path, final_path, ec);
	if(ec) { if(had_target) { std::error_code restore; std::filesystem::rename(backup_path, final_path, restore); } std::filesystem::remove(temp_path, ec); return Fail(ResultCode::IOError, diagnostics, "unable to commit destination", path, "IMGIO_REPLACE"); }
	if(had_target) { std::filesystem::remove(backup_path, ec); if(ec) Warn(diagnostics, "temporary backup cleanup failed", backup_path.string().c_str(), "IMGIO_CLEANUP"); }
	return Result::Success();
}

} // namespace Imaging
} // namespace Upp
