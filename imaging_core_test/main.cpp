#include <Core/Core.h>
#include <ImagingCore/ImagingCore.h>
#include <cstdio>
using namespace Upp;
using namespace Upp::Imaging;
struct T { int passed = 0, failed = 0; };
static void Check(T& t, bool ok, const char* name) { std::printf("%s: %s\n", name, ok ? "PASS" : "FAIL"); (ok ? t.passed : t.failed)++; }

int main()
{
	T t; int64 count;
	DataWindow empty, w{-4, 7, 5, 18};
	Check(t, empty.IsEmpty() && !empty.IsValid(), "default window is empty");
	Check(t, w.IsValid() && w.Width() == 10 && w.Height() == 12 && w.GetPixelCount(count) && count == 120, "window geometry and count");
	Check(t, w == DataWindow{-4, 7, 5, 18} && w != empty, "window equality");
	DataWindow bad{0, 0, -2, 3}; Check(t, bad.IsEmpty() && !bad.GetPixelCount(count), "invalid window rejected");
	DataWindow huge{INT_MIN, 0, INT_MAX, 1}; Check(t, !huge.GetPixelCount(count), "window overflow rejected");
	Check(t, BytesPerSample(SampleType::UInt8) == 1 && BytesPerSample(SampleType::UInt16) == 2 && BytesPerSample(SampleType::Float16) == 2 && BytesPerSample(SampleType::Float32) == 4, "sample sizes");
	Check(t, !IsValid(SampleType::Invalid) && IsFloating(SampleType::Float32), "sample classification");
	Check(t, CanonicalChannels(ChannelLayout::Gray) == 1 && CanonicalChannels(ChannelLayout::GrayAlpha) == 2 && CanonicalChannels(ChannelLayout::RGB) == 3 && CanonicalChannels(ChannelLayout::RGBA) == 4, "canonical layouts");
	ImageSpec spec; spec.data_window = w; spec.channels = 4; spec.channel_layout = ChannelLayout::RGBA; spec.sample_type = SampleType::Float32; spec.alpha_channel = 3;
	Check(t, spec.IsValid() && spec.GetPixelCount(count) && count == 120 && spec.GetSampleCount(count) && count == 480 && spec.GetByteCount(count) && count == 1920, "valid RGBA specification");
	ImageSpec mismatch; mismatch.data_window = w; mismatch.channels = 3; mismatch.channel_layout = ChannelLayout::RGBA; mismatch.sample_type = SampleType::Float32; Check(t, !mismatch.IsValid(), "canonical mismatch rejected");
	ImageSpec multi; multi.data_window = w; multi.channel_layout = ChannelLayout::MultiChannel; multi.channels = 2; multi.sample_type = SampleType::Float32; multi.channel_names.Add("X"); multi.channel_names.Add("Y"); Check(t, multi.IsValid(), "named multichannel specification");
	ImageSpec unnamed; unnamed.data_window = w; unnamed.channel_layout = ChannelLayout::MultiChannel; unnamed.channels = 2; unnamed.sample_type = SampleType::Float32; Check(t, !unnamed.IsValid(), "unnamed multichannel rejected");
	ImageSpec invalid; invalid.data_window = w; invalid.channels = 4; invalid.channel_layout = ChannelLayout::RGBA; invalid.sample_type = SampleType::Invalid; Check(t, !invalid.IsValid(), "invalid sample rejected");
	ImageBuffer buffer; Check(t, buffer.IsEmpty() && buffer.Allocate(spec) && buffer.IsValid() && buffer.GetByteCount() == 1920, "owned buffer allocation");
	buffer.Begin()[0] = 42; Check(t, buffer.Begin()[0] == 42, "buffer read write");
	ImageBuffer copy = buffer; Check(t, copy.GetByteCount() == buffer.GetByteCount() && copy.Begin()[0] == 42, "buffer copy");
	ImageBuffer moved = pick(copy); Check(t, moved.IsValid() && copy.IsEmpty(), "buffer move");
	ImageSpec badspec = spec; badspec.data_window = DataWindow(); ImageBuffer refused; Check(t, !refused.Allocate(badspec) && refused.IsEmpty(), "invalid allocation leaves empty");
	ImageData image; image.spec = spec; image.buffer = buffer; image.metadata.Set("source", "synthetic"); Check(t, image.IsValid() && image.metadata.Get("source") == "synthetic", "coherent image data");
	Metadata metadata; String value; metadata.Set("a", "1"); metadata.Set("b", "2"); metadata.Set("a", "3"); Check(t, metadata.GetCount() == 2 && metadata.Get("a") == "3" && metadata.Get("missing", "fallback") == "fallback", "metadata insert replace lookup");
	Check(t, !metadata.TryGet("missing", value) && metadata.Remove("a") && metadata.GetCount() == 1, "metadata remove without insertion"); metadata.Clear(); Check(t, metadata.GetCount() == 0, "metadata clear");
	Result ok = Result::Success(), failure = Result::Failure(ResultCode::Overflow, "too large", "ImageSpec.byte_count"); Check(t, ok && ok.code == ResultCode::Ok && !failure && failure.code == ResultCode::Overflow && failure.context == "ImageSpec.byte_count", "result semantics");
	Diagnostics diagnostics; diagnostics.Info("read", "source"); diagnostics.Warning("missing alpha"); diagnostics.Error("invalid size", "bytes"); Check(t, diagnostics.GetCount() == 3 && diagnostics.HasWarnings() && diagnostics.HasErrors() && diagnostics.entries[0].context == "source", "structured diagnostics"); diagnostics.Clear(); Check(t, diagnostics.IsEmpty(), "diagnostics clear");
	std::printf("SUMMARY passed=%d failed=%d\n", t.passed, t.failed); return t.failed ? 1 : 0;
}
