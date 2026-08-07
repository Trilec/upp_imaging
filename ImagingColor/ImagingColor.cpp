#include "ImagingColor.h"

#include <OpenColorIO/OpenColorIO.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <exception>

namespace Upp {
namespace Imaging {

namespace OCIO = OCIO_NAMESPACE;

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

static bool SameName(const String& a, const char* b)
{
	return ToLower(a) == b;
}

static Result LoadConfig(const ColorConfigRef& reference,
                         OCIO::ConstConfigRcPtr& config,
                         String& identity,
                         Diagnostics* diagnostics)
{
	config.reset();
	identity.Clear();

	try {
		switch(reference.source) {
		case ColorConfigSource::Builtin:
			if(reference.value.IsEmpty())
				return Fail(ResultCode::InvalidArgument, diagnostics,
				            "builtin OCIO config name is empty", "config",
				            "IMGCOLOR_CONFIG");
			config = OCIO::Config::CreateFromBuiltinConfig(reference.value.Begin());
			identity = reference.value;
			break;
		case ColorConfigSource::File:
			if(reference.value.IsEmpty())
				return Fail(ResultCode::InvalidArgument, diagnostics,
				            "OCIO config file path is empty", "config",
				            "IMGCOLOR_CONFIG");
			config = OCIO::Config::CreateFromFile(reference.value.Begin());
			identity = reference.value;
			break;
		default:
			return Fail(ResultCode::InvalidArgument, diagnostics,
			            "unknown OCIO config source", "config",
			            "IMGCOLOR_CONFIG");
		}

		if(!config)
			return Fail(ResultCode::IOError, diagnostics,
			            "OCIO config creation returned null", identity,
			            "IMGCOLOR_CONFIG");
		config->validate();
		return Result::Success();
	}
	catch(const OCIO::Exception& e) {
		return Fail(reference.source == ColorConfigSource::File
		                ? ResultCode::IOError : ResultCode::InvalidArgument,
		            diagnostics, e.what(), identity.IsEmpty() ? reference.value : identity,
		            "IMGCOLOR_CONFIG");
	}
	catch(const std::exception& e) {
		return Fail(ResultCode::InternalFailure, diagnostics, e.what(),
		            identity.IsEmpty() ? reference.value : identity,
		            "IMGCOLOR_CONFIG");
	}
	catch(...) {
		return Fail(ResultCode::InternalFailure, diagnostics,
		            "OCIO config loading failed",
		            identity.IsEmpty() ? reference.value : identity,
		            "IMGCOLOR_CONFIG");
	}
}

static void AddColorSpaces(const OCIO::ConstConfigRcPtr& config,
                           Vector<String>& names)
{
	for(int i = 0; i < config->getNumColorSpaces(); ++i) {
		const char* name = config->getColorSpaceNameByIndex(i);
		if(name && *name)
			names.Add(name);
	}
}

static void AddDisplays(const OCIO::ConstConfigRcPtr& config,
                        Vector<String>& names)
{
	for(int i = 0; i < config->getNumDisplays(); ++i) {
		const char* name = config->getDisplay(i);
		if(name && *name)
			names.Add(name);
	}
}

static void AddLooks(const OCIO::ConstConfigRcPtr& config,
                     Vector<String>& names)
{
	for(int i = 0; i < config->getNumLooks(); ++i) {
		const char* name = config->getLookNameByIndex(i);
		if(name && *name)
			names.Add(name);
	}
}

static String DefaultSource(const OCIO::ConstConfigRcPtr& config)
{
	const char* value = config->getRoleColorSpace("scene_linear");
	if(value && *value)
		return value;
	value = config->getRoleColorSpace("default");
	if(value && *value)
		return value;
	if(config->getNumColorSpaces() > 0) {
		value = config->getColorSpaceNameByIndex(0);
		if(value && *value)
			return value;
	}
	return String();
}

static Result ValidateColorChannels(const ImageSpec& spec,
                                    int& red, int& green, int& blue,
                                    Diagnostics* diagnostics)
{
	red = green = blue = -1;
	if(spec.channel_layout == ChannelLayout::RGB ||
	   spec.channel_layout == ChannelLayout::RGBA) {
		red = 0;
		green = 1;
		blue = 2;
		return Result::Success();
	}

	if(spec.channel_layout != ChannelLayout::MultiChannel)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "colour transforms require RGB-bearing input",
		            "channel_layout", "IMGCOLOR_CHANNELS");
	if(spec.channel_names.GetCount() != spec.channels)
		return Fail(ResultCode::InvalidSpecification, diagnostics,
		            "multichannel input has no authoritative channel names",
		            "channel_names", "IMGCOLOR_CHANNELS");

	for(int i = 0; i < spec.channel_names.GetCount(); ++i) {
		if(SameName(spec.channel_names[i], "r")) {
			if(red >= 0)
				return Fail(ResultCode::Unsupported, diagnostics,
				            "multichannel input has duplicate R channels",
				            "channel_names", "IMGCOLOR_CHANNELS");
			red = i;
		}
		else if(SameName(spec.channel_names[i], "g")) {
			if(green >= 0)
				return Fail(ResultCode::Unsupported, diagnostics,
				            "multichannel input has duplicate G channels",
				            "channel_names", "IMGCOLOR_CHANNELS");
			green = i;
		}
		else if(SameName(spec.channel_names[i], "b")) {
			if(blue >= 0)
				return Fail(ResultCode::Unsupported, diagnostics,
				            "multichannel input has duplicate B channels",
				            "channel_names", "IMGCOLOR_CHANNELS");
			blue = i;
		}
	}
	if(red < 0 || green < 0 || blue < 0)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "multichannel input does not contain a complete named RGB triplet",
		            "channel_names", "IMGCOLOR_CHANNELS");
	if(spec.alpha_channel == red || spec.alpha_channel == green ||
	   spec.alpha_channel == blue)
		return Fail(ResultCode::Unsupported, diagnostics,
		            "multichannel alpha channel aliases an RGB channel",
		            "alpha_channel", "IMGCOLOR_CHANNELS");
	return Result::Success();
}

static float HalfToFloat(uint16_t bits)
{
	uint32_t sign = (uint32_t)(bits & 0x8000u) << 16;
	uint32_t exponent = (bits >> 10) & 0x1fu;
	uint32_t mantissa = bits & 0x03ffu;
	uint32_t result;

	if(exponent == 0) {
		if(mantissa == 0)
			result = sign;
		else {
			int e = -14;
			while((mantissa & 0x0400u) == 0) {
				mantissa <<= 1;
				--e;
			}
			mantissa &= 0x03ffu;
			result = sign | (uint32_t)(e + 127) << 23 | mantissa << 13;
		}
	}
	else if(exponent == 0x1fu)
		result = sign | 0x7f800000u | mantissa << 13;
	else
		result = sign | (exponent + (127 - 15)) << 23 | mantissa << 13;

	float value;
	memcpy(&value, &result, sizeof(value));
	return value;
}

static uint16_t FloatToHalf(float value)
{
	uint32_t bits;
	memcpy(&bits, &value, sizeof(bits));
	uint16_t sign = (uint16_t)((bits >> 16) & 0x8000u);
	uint32_t exponent_bits = (bits >> 23) & 0xffu;
	uint32_t mantissa = bits & 0x007fffffu;

	if(exponent_bits == 0xffu) {
		if(mantissa == 0)
			return (uint16_t)(sign | 0x7c00u);
		uint16_t payload = (uint16_t)(mantissa >> 13);
		if(payload == 0)
			payload = 1;
		return (uint16_t)(sign | 0x7c00u | payload);
	}

	int exponent = (int)exponent_bits - 127 + 15;
	if(exponent <= 0) {
		if(exponent < -10)
			return sign;
		mantissa |= 0x00800000u;
		int shift = 14 - exponent;
		uint32_t rounded = mantissa >> shift;
		uint32_t remainder = mantissa & ((1u << shift) - 1u);
		uint32_t halfway = 1u << (shift - 1);
		if(remainder > halfway || (remainder == halfway && (rounded & 1u)))
			++rounded;
		return (uint16_t)(sign | rounded);
	}
	if(exponent >= 31)
		return (uint16_t)(sign | 0x7c00u);

	uint16_t result = (uint16_t)(sign | ((uint16_t)exponent << 10) |
	                         (uint16_t)(mantissa >> 13));
	uint32_t remainder = mantissa & 0x1fffu;
	if(remainder > 0x1000u || (remainder == 0x1000u && (result & 1u)))
		++result;
	return result;
}

static float ReadSample(const byte* pixels, int64 sample, SampleType type)
{
	const byte* source = pixels + sample * BytesPerSample(type);
	switch(type) {
	case SampleType::UInt8:
		return source[0] / 255.0f;
	case SampleType::UInt16: {
		uint16_t value;
		memcpy(&value, source, sizeof(value));
		return value / 65535.0f;
	}
	case SampleType::Float16: {
		uint16_t value;
		memcpy(&value, source, sizeof(value));
		return HalfToFloat(value);
	}
	case SampleType::Float32: {
		float value;
		memcpy(&value, source, sizeof(value));
		return value;
	}
	default:
		return 0.0f;
	}
}

static void WriteSample(byte* pixels, int64 sample, SampleType type, float value)
{
	byte* target = pixels + sample * BytesPerSample(type);
	switch(type) {
	case SampleType::UInt8: {
		if(std::isnan(value) || value <= 0.0f)
			value = 0.0f;
		else if(!std::isfinite(value) || value >= 1.0f)
			value = 1.0f;
		byte quantized = (byte)std::floor(value * 255.0f + 0.5f);
		memcpy(target, &quantized, sizeof(quantized));
		break;
	}
	case SampleType::UInt16: {
		if(std::isnan(value) || value <= 0.0f)
			value = 0.0f;
		else if(!std::isfinite(value) || value >= 1.0f)
			value = 1.0f;
		uint16_t quantized = (uint16_t)std::floor(value * 65535.0f + 0.5f);
		memcpy(target, &quantized, sizeof(quantized));
		break;
	}
	case SampleType::Float16: {
		uint16_t half = FloatToHalf(value);
		memcpy(target, &half, sizeof(half));
		break;
	}
	case SampleType::Float32:
		memcpy(target, &value, sizeof(value));
		break;
	default:
		break;
	}
}

static Result ApplyProcessor(const ImageData& input, ImageData& output,
                             const OCIO::ConstCPUProcessorRcPtr& cpu,
                             Diagnostics* diagnostics)
{
	if(!input.IsValid())
		return Fail(ResultCode::InvalidSpecification, diagnostics,
		            "input image is invalid", "image", "IMGCOLOR_SAMPLE");
	if(!IsValid(input.spec.sample_type))
		return Fail(ResultCode::Unsupported, diagnostics,
		            "input sample type is unsupported", "sample_type",
		            "IMGCOLOR_SAMPLE");

	int red, green, blue;
	Result channels = ValidateColorChannels(input.spec, red, green, blue,
	                                      diagnostics);
	if(!channels)
		return channels;
	if(!cpu)
		return Fail(ResultCode::InternalFailure, diagnostics,
		            "OCIO CPU processor is unavailable", "processor",
		            "IMGCOLOR_PROCESSOR");

	int64 width;
	int64 height;
	int64 pixel_count;
	if(!input.spec.GetWidth(width) || !input.spec.GetHeight(height) ||
	   !input.spec.GetPixelCount(pixel_count) || width > INT_MAX ||
	   height > INT_MAX || pixel_count > INT_MAX)
		return Fail(ResultCode::Overflow, diagnostics,
		            "image geometry cannot be represented by the colour processor",
		            "image", "IMGCOLOR_PIXELS");

	ImageData candidate = input;
	Vector<float> row;
	if(width > INT_MAX / 3)
		return Fail(ResultCode::Overflow, diagnostics,
		            "image row is too wide for colour processing", "image",
		            "IMGCOLOR_PIXELS");
	row.SetCount((int)width * 3);

	try {
		for(int z = 0; z < input.spec.depth; ++z) {
			for(int64 y = 0; y < height; ++y) {
				for(int64 x = 0; x < width; ++x) {
					int64 pixel = ((int64)z * height + y) * width + x;
					int64 base = pixel * input.spec.channels;
					row[(int)x * 3 + 0] = ReadSample(input.buffer.Begin(), base + red,
					                                      input.spec.sample_type);
					row[(int)x * 3 + 1] = ReadSample(input.buffer.Begin(), base + green,
					                                      input.spec.sample_type);
					row[(int)x * 3 + 2] = ReadSample(input.buffer.Begin(), base + blue,
					                                      input.spec.sample_type);
				}

				OCIO::PackedImageDesc image(row.Begin(), (long)width, 1, 3);
				cpu->apply(image);

				for(int64 x = 0; x < width; ++x) {
					int64 pixel = ((int64)z * height + y) * width + x;
					int64 base = pixel * input.spec.channels;
					WriteSample(candidate.buffer.Begin(), base + red,
					            input.spec.sample_type, row[(int)x * 3 + 0]);
					WriteSample(candidate.buffer.Begin(), base + green,
					            input.spec.sample_type, row[(int)x * 3 + 1]);
					WriteSample(candidate.buffer.Begin(), base + blue,
					            input.spec.sample_type, row[(int)x * 3 + 2]);
				}
			}
		}
	}
	catch(const OCIO::Exception& e) {
		return Fail(ResultCode::InternalFailure, diagnostics, e.what(),
		            "pixels", "IMGCOLOR_PIXELS");
	}
	catch(const std::exception& e) {
		return Fail(ResultCode::InternalFailure, diagnostics, e.what(),
		            "pixels", "IMGCOLOR_PIXELS");
	}
	catch(...) {
		return Fail(ResultCode::InternalFailure, diagnostics,
		            "OCIO pixel processing failed", "pixels",
		            "IMGCOLOR_PIXELS");
	}

	if(!candidate.IsValid())
		return Fail(ResultCode::InternalFailure, diagnostics,
		            "processed image failed Core validation", "image",
		            "IMGCOLOR_PIXELS");
	output = pick(candidate);
	return Result::Success();
}

static Result BuildColorSpaceProcessor(const ColorSpaceTransform& transform,
                                       OCIO::ConstCPUProcessorRcPtr& cpu,
                                       Diagnostics* diagnostics)
{
	if(transform.source.IsEmpty() || transform.destination.IsEmpty())
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "source and destination colour spaces are required",
		            "transform", "IMGCOLOR_SELECTION");

	OCIO::ConstConfigRcPtr config;
	String identity;
	Result loaded = LoadConfig(transform.config, config, identity, diagnostics);
	if(!loaded)
		return loaded;

	try {
		if(!config->getColorSpace(transform.source.Begin()))
			return Fail(ResultCode::InvalidArgument, diagnostics,
			            "source colour space is not present in the config",
			            transform.source, "IMGCOLOR_SELECTION");
		if(!config->getColorSpace(transform.destination.Begin()))
			return Fail(ResultCode::InvalidArgument, diagnostics,
			            "destination colour space is not present in the config",
			            transform.destination, "IMGCOLOR_SELECTION");
		OCIO::ConstProcessorRcPtr processor =
			config->getProcessor(transform.source.Begin(),
			                     transform.destination.Begin());
		if(!processor)
			return Fail(ResultCode::InternalFailure, diagnostics,
			            "OCIO colour-space processor creation returned null",
			            identity, "IMGCOLOR_PROCESSOR");
		cpu = processor->getDefaultCPUProcessor();
		if(!cpu)
			return Fail(ResultCode::InternalFailure, diagnostics,
			            "OCIO CPU processor creation returned null", identity,
			            "IMGCOLOR_PROCESSOR");
		return Result::Success();
	}
	catch(const OCIO::Exception& e) {
		return Fail(ResultCode::InvalidArgument, diagnostics, e.what(),
		            identity, "IMGCOLOR_PROCESSOR");
	}
	catch(const std::exception& e) {
		return Fail(ResultCode::InternalFailure, diagnostics, e.what(),
		            identity, "IMGCOLOR_PROCESSOR");
	}
	catch(...) {
		return Fail(ResultCode::InternalFailure, diagnostics,
		            "OCIO colour-space processor creation failed", identity,
		            "IMGCOLOR_PROCESSOR");
	}
}

static Result BuildDisplayProcessor(const DisplayTransform& transform,
                                    OCIO::ConstCPUProcessorRcPtr& cpu,
                                    Diagnostics* diagnostics)
{
	if(transform.source.IsEmpty() || transform.display.IsEmpty() ||
	   transform.view.IsEmpty())
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "source colour space, display and view are required",
		            "transform", "IMGCOLOR_SELECTION");

	OCIO::ConstConfigRcPtr config;
	String identity;
	Result loaded = LoadConfig(transform.config, config, identity, diagnostics);
	if(!loaded)
		return loaded;

	try {
		if(!config->getColorSpace(transform.source.Begin()))
			return Fail(ResultCode::InvalidArgument, diagnostics,
			            "source colour space is not present in the config",
			            transform.source, "IMGCOLOR_SELECTION");

		bool display_found = false;
		for(int i = 0; i < config->getNumDisplays(); ++i) {
			const char* name = config->getDisplay(i);
			if(name && transform.display == name) {
				display_found = true;
				break;
			}
		}
		if(!display_found)
			return Fail(ResultCode::InvalidArgument, diagnostics,
			            "display is not present in the config", transform.display,
			            "IMGCOLOR_SELECTION");

		bool view_found = false;
		for(int i = 0; i < config->getNumViews(transform.display.Begin()); ++i) {
			const char* name = config->getView(transform.display.Begin(), i);
			if(name && transform.view == name) {
				view_found = true;
				break;
			}
		}
		if(!view_found)
			return Fail(ResultCode::InvalidArgument, diagnostics,
			            "view is not present for the selected display", transform.view,
			            "IMGCOLOR_SELECTION");

		OCIO::DisplayViewTransformRcPtr display = OCIO::DisplayViewTransform::Create();
		display->setSrc(transform.source.Begin());
		display->setDisplay(transform.display.Begin());
		display->setView(transform.view.Begin());
		OCIO::ConstProcessorRcPtr processor = config->getProcessor(display);
		if(!processor)
			return Fail(ResultCode::InternalFailure, diagnostics,
			            "OCIO display processor creation returned null", identity,
			            "IMGCOLOR_PROCESSOR");
		cpu = processor->getDefaultCPUProcessor();
		if(!cpu)
			return Fail(ResultCode::InternalFailure, diagnostics,
			            "OCIO display CPU processor creation returned null", identity,
			            "IMGCOLOR_PROCESSOR");
		return Result::Success();
	}
	catch(const OCIO::Exception& e) {
		return Fail(ResultCode::InvalidArgument, diagnostics, e.what(), identity,
		            "IMGCOLOR_PROCESSOR");
	}
	catch(const std::exception& e) {
		return Fail(ResultCode::InternalFailure, diagnostics, e.what(), identity,
		            "IMGCOLOR_PROCESSOR");
	}
	catch(...) {
		return Fail(ResultCode::InternalFailure, diagnostics,
		            "OCIO display processor creation failed", identity,
		            "IMGCOLOR_PROCESSOR");
	}
}

} // namespace

ColorConfigInfo::ColorConfigInfo(const ColorConfigInfo& other)
{
	*this = other;
}

ColorConfigInfo::ColorConfigInfo(ColorConfigInfo&& other)
	: identity(pick(other.identity)),
	  cache_id(pick(other.cache_id)),
	  color_spaces(pick(other.color_spaces)),
	  displays(pick(other.displays)),
	  looks(pick(other.looks)),
	  default_source(pick(other.default_source)),
	  default_display(pick(other.default_display)),
	  default_view(pick(other.default_view))
{
}

ColorConfigInfo& ColorConfigInfo::operator=(const ColorConfigInfo& other)
{
	if(this != &other) {
		identity = other.identity;
		cache_id = other.cache_id;
		color_spaces.Clear();
		for(const String& value : other.color_spaces)
			color_spaces.Add(value);
		displays.Clear();
		for(const String& value : other.displays)
			displays.Add(value);
		looks.Clear();
		for(const String& value : other.looks)
			looks.Add(value);
		default_source = other.default_source;
		default_display = other.default_display;
		default_view = other.default_view;
	}
	return *this;
}

ColorConfigInfo& ColorConfigInfo::operator=(ColorConfigInfo&& other)
{
	if(this != &other) {
		identity = pick(other.identity);
		cache_id = pick(other.cache_id);
		color_spaces = pick(other.color_spaces);
		displays = pick(other.displays);
		looks = pick(other.looks);
		default_source = pick(other.default_source);
		default_display = pick(other.default_display);
		default_view = pick(other.default_view);
	}
	return *this;
}

void ColorConfigInfo::Clear()
{
	identity.Clear();
	cache_id.Clear();
	color_spaces.Clear();
	displays.Clear();
	looks.Clear();
	default_source.Clear();
	default_display.Clear();
	default_view.Clear();
}

Vector<String> GetBuiltinColorConfigNames()
{
	Vector<String> names;
	try {
		const OCIO::BuiltinConfigRegistry& registry =
			OCIO::BuiltinConfigRegistry::Get();
		for(size_t i = 0; i < registry.getNumBuiltinConfigs(); ++i) {
			const char* name = registry.getBuiltinConfigName(i);
			if(name && *name)
				names.Add(name);
		}
	}
	catch(...) {
	}
	return names;
}

Result InspectColorConfig(const ColorConfigRef& reference,
                          ColorConfigInfo& info,
                          Diagnostics* diagnostics)
{
	Reset(diagnostics);
	OCIO::ConstConfigRcPtr config;
	String identity;
	Result loaded = LoadConfig(reference, config, identity, diagnostics);
	if(!loaded)
		return loaded;

	ColorConfigInfo candidate;
	candidate.identity = identity;
	try {
		const char* cache = config->getCacheID();
		if(cache)
			candidate.cache_id = cache;
		AddColorSpaces(config, candidate.color_spaces);
		AddDisplays(config, candidate.displays);
		AddLooks(config, candidate.looks);
		candidate.default_source = DefaultSource(config);
		const char* display = config->getDefaultDisplay();
		if(display && *display)
			candidate.default_display = display;
		else if(!candidate.displays.IsEmpty())
			candidate.default_display = candidate.displays[0];
		if(!candidate.default_display.IsEmpty()) {
			const char* view = config->getDefaultView(candidate.default_display.Begin());
			if(view && *view)
				candidate.default_view = view;
		}
	}
	catch(const OCIO::Exception& e) {
		return Fail(ResultCode::InternalFailure, diagnostics, e.what(), identity,
		            "IMGCOLOR_CONFIG");
	}
	catch(const std::exception& e) {
		return Fail(ResultCode::InternalFailure, diagnostics, e.what(), identity,
		            "IMGCOLOR_CONFIG");
	}

	info.Clear();
	info.identity = candidate.identity;
	info.cache_id = candidate.cache_id;
	info.color_spaces = pick(candidate.color_spaces);
	info.displays = pick(candidate.displays);
	info.looks = pick(candidate.looks);
	info.default_source = candidate.default_source;
	info.default_display = candidate.default_display;
	info.default_view = candidate.default_view;
	return Result::Success();
}

Result GetDisplayViews(const ColorConfigRef& reference,
                       const String& display,
                       Vector<String>& views,
                       String* default_view,
                       Diagnostics* diagnostics)
{
	Reset(diagnostics);
	if(display.IsEmpty())
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "display name is empty", "display", "IMGCOLOR_SELECTION");

	OCIO::ConstConfigRcPtr config;
	String identity;
	Result loaded = LoadConfig(reference, config, identity, diagnostics);
	if(!loaded)
		return loaded;

	Vector<String> candidate;
	String default_candidate;
	try {
		for(int i = 0; i < config->getNumViews(display.Begin()); ++i) {
			const char* name = config->getView(display.Begin(), i);
			if(name && *name)
				candidate.Add(name);
		}
		if(candidate.IsEmpty())
			return Fail(ResultCode::InvalidArgument, diagnostics,
			            "display is not present or has no views", display,
			            "IMGCOLOR_SELECTION");
		const char* value = config->getDefaultView(display.Begin());
		if(value && *value)
			default_candidate = value;
		else
			default_candidate = candidate[0];
	}
	catch(const OCIO::Exception& e) {
		return Fail(ResultCode::InvalidArgument, diagnostics, e.what(), display,
		            "IMGCOLOR_SELECTION");
	}

	views = pick(candidate);
	if(default_view)
		*default_view = default_candidate;
	return Result::Success();
}

Result ApplyColorSpaceTransform(const ImageData& input,
                                ImageData& output,
                                const ColorSpaceTransform& transform,
                                Diagnostics* diagnostics)
{
	Reset(diagnostics);
	OCIO::ConstCPUProcessorRcPtr cpu;
	Result processor = BuildColorSpaceProcessor(transform, cpu, diagnostics);
	if(!processor)
		return processor;
	return ApplyProcessor(input, output, cpu, diagnostics);
}

Result ApplyDisplayTransform(const ImageData& input,
                             ImageData& output,
                             const DisplayTransform& transform,
                             Diagnostics* diagnostics)
{
	Reset(diagnostics);
	OCIO::ConstCPUProcessorRcPtr cpu;
	Result processor = BuildDisplayProcessor(transform, cpu, diagnostics);
	if(!processor)
		return processor;
	return ApplyProcessor(input, output, cpu, diagnostics);
}

} // namespace Imaging
} // namespace Upp
