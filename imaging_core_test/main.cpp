#include <Core/Core.h>
#include <ImagingCore/ImagingCore.h>

using namespace Upp;
using namespace Upp::Imaging;

struct TestState { int passed = 0; int failed = 0; };

static void Check(TestState& state, bool condition, const char* name)
{
	Cout() << (condition ? "PASS " : "FAIL ") << name << '\n';
	(condition ? state.passed : state.failed)++;
}

static ImageSpec MakeSpec(DataWindow window, int channels, ChannelLayout layout, SampleType type)
{
	ImageSpec spec;
	spec.data_window = window;
	spec.channels = channels;
	spec.channel_layout = layout;
	spec.sample_type = type;
	if(layout == ChannelLayout::GrayAlpha) spec.alpha_channel = 1;
	if(layout == ChannelLayout::RGBA) spec.alpha_channel = 3;
	return spec;
}

CONSOLE_APP_MAIN
{
	TestState state;
	int64 value = 99;
	DataWindow empty;
	DataWindow window{-4, 7, 5, 18};
	Check(state, empty.IsEmpty() && !empty.IsValid(), "default empty data window");
	Check(state, window.GetWidth(value) && value == 10 && window.GetHeight(value) && value == 12, "checked dimensions and origins");
	Check(state, window.GetPixelCount(value) && value == 120, "checked pixel count");
	Check(state, window == DataWindow{-4, 7, 5, 18} && window != empty, "data window equality");
	DataWindow malformed{0, 0, -2, 3};
	Check(state, !malformed.GetWidth(value) && value == 0 && !malformed.GetPixelCount(value) && value == 0, "malformed bounds refusal");
	DataWindow extreme{INT_MIN, 0, INT_MAX, 1};
	Check(state, extreme.GetWidth(value) && value == (int64)INT_MAX - INT_MIN + 1, "extreme coordinate arithmetic");
	Check(state, !CheckedMultiply(INT64_MAX, 2, value) && value == 0, "multiplication overflow resets output");
	Check(state, !CheckedMultiply(-1, 2, value) && value == 0, "negative multiplication refusal");

	Check(state, BytesPerSample(SampleType::UInt8) == 1 && BytesPerSample(SampleType::UInt16) == 2 && BytesPerSample(SampleType::Float16) == 2 && BytesPerSample(SampleType::Float32) == 4, "sample byte sizes");
	Check(state, !IsValid(SampleType::Invalid) && IsFloating(SampleType::Float16) && !IsFloating(SampleType::UInt16), "sample classifications");
	Check(state, CanonicalChannels(ChannelLayout::Gray) == 1 && CanonicalChannels(ChannelLayout::GrayAlpha) == 2 && CanonicalChannels(ChannelLayout::RGB) == 3 && CanonicalChannels(ChannelLayout::RGBA) == 4, "canonical layout counts");
	Check(state, CanonicalChannels((ChannelLayout)99) == 0, "invalid layout enum");

	ImageSpec gray = MakeSpec(window, 1, ChannelLayout::Gray, SampleType::UInt8);
	ImageSpec gray_alpha = MakeSpec(window, 2, ChannelLayout::GrayAlpha, SampleType::UInt16);
	ImageSpec rgb = MakeSpec(window, 3, ChannelLayout::RGB, SampleType::Float16);
	ImageSpec rgba = MakeSpec(window, 4, ChannelLayout::RGBA, SampleType::Float32);
	Check(state, gray.IsValid() && gray_alpha.IsValid() && rgb.IsValid() && rgba.IsValid(), "canonical image specifications");
	Check(state, rgba.GetPixelCount(value) && value == 120 && rgba.GetSampleCount(value) && value == 480 && rgba.GetByteCount(value) && value == 1920, "checked specification sizes");
	ImageSpec bad_alpha = rgba; bad_alpha.alpha_channel = 2;
	ImageSpec bad_layout = rgba; bad_layout.channel_layout = (ChannelLayout)99;
	ImageSpec bad_count = rgba; bad_count.channels = 3;
	Check(state, !bad_alpha.IsValid() && !bad_layout.IsValid() && !bad_count.IsValid(), "layout and alpha validation");
	ImageSpec multi; multi.data_window = window; multi.channels = 2; multi.channel_layout = ChannelLayout::MultiChannel; multi.sample_type = SampleType::Float32; multi.channel_names.Add("temperature"); multi.channel_names.Add("confidence");
	Check(state, multi.IsValid(), "named multichannel specification");
	ImageSpec unnamed = multi; unnamed.channel_names.Clear();
	ImageSpec empty_name = multi; empty_name.channel_names[0].Clear();
	Check(state, !unnamed.IsValid() && !empty_name.IsValid(), "multichannel name validation");
	Check(state, rgba.IsValid() == rgba.IsValid(), "repeatable specification validation");

	ImageSpec copied(rgba); Check(state, copied.IsValid() && copied.alpha_channel == 3, "specification copy construction");
	ImageSpec assigned; assigned = multi; Check(state, assigned.IsValid() && assigned.channel_names.GetCount() == 2, "specification copy assignment");
	ImageSpec moved(pick(copied)); Check(state, moved.IsValid() && copied.IsValid() == false, "specification move construction");
	ImageSpec moved_assigned; moved_assigned = pick(assigned); Check(state, moved_assigned.IsValid() && assigned.IsValid() == false, "specification move assignment");
	moved_assigned = moved_assigned; Check(state, moved_assigned.IsValid(), "specification self assignment");

	ImageBuffer buffer;
	Check(state, buffer.IsEmpty() && buffer.Allocate(rgba) && buffer.IsValid() && buffer.GetSampleType() == SampleType::Float32 && buffer.GetByteCount() == 1920, "buffer allocation invariant");
	buffer.Begin()[0] = 42;
	ImageBuffer buffer_copy(buffer); buffer_copy.Begin()[0] = 7;
	Check(state, buffer.Begin()[0] == 42 && buffer_copy.Begin()[0] == 7, "buffer copy independence");
	ImageBuffer buffer_assigned; buffer_assigned = buffer; buffer_assigned = buffer_assigned;
	Check(state, buffer_assigned.Begin()[0] == 42 && buffer_assigned.IsValid(), "buffer assignment and self assignment");
	ImageBuffer buffer_moved(pick(buffer_copy));
	Check(state, buffer_moved.IsValid() && buffer_copy.IsEmpty(), "buffer move construction");
	ImageBuffer invalid_buffer; Check(state, !invalid_buffer.Allocate(ImageSpec()) && invalid_buffer.IsEmpty(), "invalid allocation clears buffer");
	buffer.Clear(); Check(state, buffer.IsEmpty(), "buffer clear");

	Metadata metadata;
	ValueArray numbers; numbers << 1 << 2 << 3;
	metadata.Set("name", Value("fixture")); metadata.Set("enabled", Value(true)); metadata.Set("count", Value(7)); metadata.Set("large", Value((int64)9000000000LL)); metadata.Set("ratio", Value(0.5)); metadata.Set("values", Value(numbers));
	Value looked_up;
	Check(state, metadata.GetCount() == 6 && metadata.TryGet("large", looked_up) && looked_up == Value((int64)9000000000LL), "typed metadata values");
	metadata.Set("name", Value("replacement"));
	Check(state, metadata.GetCount() == 6 && metadata.Items().GetKey(0) == "name" && metadata.Get("name") == Value("replacement"), "metadata replacement preserves order");
	int before_missing = metadata.GetCount(); metadata.Get("missing");
	Check(state, metadata.GetCount() == before_missing && metadata.Remove("enabled") && metadata.GetCount() == 5, "metadata missing lookup and removal");
	Metadata metadata_copy = metadata; Check(state, metadata_copy == metadata, "metadata copy equality"); metadata.Clear(); Check(state, metadata.IsEmpty(), "metadata clear");

	ImageData image; image.spec = rgba; image.buffer = buffer_moved; image.metadata = metadata_copy;
	Check(state, image.IsValid() && image.metadata.Get("name") == Value("replacement"), "coherent image data");
	ImageData image_copy = image; Check(state, image_copy.IsValid(), "image data copy"); image.Clear(); Check(state, !image.IsValid() && image.metadata.IsEmpty(), "image data clear");

	Result success = Result::Success();
	Result failure = Result::Failure(ResultCode::Overflow, "too large", "ImageSpec.byte_count");
	Result normalized = Result::Failure(ResultCode::Ok, "invalid failure");
	Check(state, success.IsOk() && success.message.IsEmpty() && success.context.IsEmpty(), "result success invariant");
	Check(state, !failure.IsOk() && failure.code == ResultCode::Overflow && failure.message == "too large" && failure.context == "ImageSpec.byte_count", "result failure context");
	Check(state, normalized.code == ResultCode::InternalFailure && !normalized.IsOk(), "result rejects contradictory success failure");
	Check(state, Result::Failure(ResultCode::InvalidArgument, "x").code == ResultCode::InvalidArgument && Result::Failure(ResultCode::InvalidSpecification, "x").code == ResultCode::InvalidSpecification && Result::Failure(ResultCode::Unsupported, "x").code == ResultCode::Unsupported && Result::Failure(ResultCode::AllocationFailure, "x").code == ResultCode::AllocationFailure && Result::Failure(ResultCode::IOError, "x").code == ResultCode::IOError && Result::Failure(ResultCode::InternalFailure, "x").code == ResultCode::InternalFailure, "result categories");

	Diagnostics diagnostics;
	diagnostics.Add(DiagnosticSeverity::Information, "read", "IMG001", "source");
	diagnostics.Add(DiagnosticSeverity::Warning, "missing alpha", "IMG002"); diagnostics.Error("invalid size", "IMG003", "bytes");
	Check(state, diagnostics.GetCount() == 3 && diagnostics.Count(DiagnosticSeverity::Warning) == 1 && diagnostics.Count(DiagnosticSeverity::Error) == 1 && diagnostics.Entries()[0].code == "IMG001", "structured diagnostics");
	Diagnostics diagnostics_copy = diagnostics; Check(state, diagnostics_copy.Entries()[2].context == "bytes", "diagnostics copy and ordering"); diagnostics.Clear(); Check(state, diagnostics.IsEmpty() && diagnostics_copy.HasErrors(), "diagnostics clear and independent copy");

	Cout() << "SUMMARY passed=" << state.passed << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
