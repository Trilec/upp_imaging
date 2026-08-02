#ifndef _ImagingCore_ImagingCore_h_
#define _ImagingCore_ImagingCore_h_

#include <Core/Core.h>

namespace Upp {
namespace Imaging {

enum class SampleType { Invalid, UInt8, UInt16, Float16, Float32 };

inline bool IsValid(SampleType type) { return type != SampleType::Invalid; }
inline bool IsFloating(SampleType type) { return type == SampleType::Float16 || type == SampleType::Float32; }
inline int BytesPerSample(SampleType type)
{
	switch(type) {
	case SampleType::UInt8: return 1;
	case SampleType::UInt16: case SampleType::Float16: return 2;
	case SampleType::Float32: return 4;
	default: return 0;
	}
}

enum class ChannelLayout { Gray, GrayAlpha, RGB, RGBA, MultiChannel };

inline int CanonicalChannels(ChannelLayout layout)
{
	switch(layout) {
	case ChannelLayout::Gray: return 1;
	case ChannelLayout::GrayAlpha: return 2;
	case ChannelLayout::RGB: return 3;
	case ChannelLayout::RGBA: return 4;
	default: return 0;
	}
}

struct DataWindow {
	int left = 0, top = 0, right = -1, bottom = -1;
	bool IsEmpty() const { return right < left || bottom < top; }
	bool IsValid() const { return !IsEmpty(); }
	int Width() const { return IsValid() ? right - left + 1 : 0; }
	int Height() const { return IsValid() ? bottom - top + 1 : 0; }
	bool operator==(const DataWindow& b) const { return left == b.left && top == b.top && right == b.right && bottom == b.bottom; }
	bool operator!=(const DataWindow& b) const { return !(*this == b); }
	bool GetPixelCount(int64& count) const
	{
		if(!IsValid() || Width() <= 0 || Height() <= 0) return false;
		count = (int64)Width() * Height();
		return count > 0;
	}
};

inline bool CheckedMultiply(int64 a, int64 b, int64& out)
{
	if(a < 0 || b < 0 || (b && a > INT64_MAX / b)) return false;
	out = a * b;
	return true;
}

struct ImageSpec {
	DataWindow data_window;
	int depth = 1;
	int channels = 0;
	ChannelLayout channel_layout = ChannelLayout::MultiChannel;
	SampleType sample_type = SampleType::Invalid;
	Vector<String> channel_names;
	int alpha_channel = -1;
	ImageSpec() = default;
	ImageSpec(const ImageSpec& b) : data_window(b.data_window), depth(b.depth), channels(b.channels), channel_layout(b.channel_layout), sample_type(b.sample_type), alpha_channel(b.alpha_channel) { for(const String& name : b.channel_names) channel_names.Add(name); }
	ImageSpec& operator=(const ImageSpec& b) { if(this != &b) { ImageSpec copy(b); *this = pick(copy); } return *this; }

	int Width() const { return data_window.Width(); }
	int Height() const { return data_window.Height(); }
	bool GetPixelCount(int64& count) const { return data_window.GetPixelCount(count) && CheckedMultiply(count, depth, count); }
	bool GetSampleCount(int64& count) const { return GetPixelCount(count) && CheckedMultiply(count, channels, count); }
	bool GetByteCount(int64& count) const { return GetSampleCount(count) && CheckedMultiply(count, BytesPerSample(sample_type), count); }
	bool IsValid() const
	{
		int64 ignored;
		if(!data_window.IsValid() || depth <= 0 || channels <= 0 || Imaging::IsValid(sample_type) == false) return false;
		int canonical = CanonicalChannels(channel_layout);
		if(canonical ? channels != canonical : channel_names.GetCount() != channels) return false;
		if(alpha_channel < -1 || alpha_channel >= channels) return false;
		if(!channel_names.IsEmpty() && channel_names.GetCount() != channels) return false;
		return GetByteCount(ignored);
	}
};

struct ImageBuffer {
	Vector<byte> bytes;
	int64 samples = 0;
	SampleType sample_type = SampleType::Invalid;
	ImageBuffer() = default;
	ImageBuffer(const ImageBuffer& b) : samples(b.samples), sample_type(b.sample_type) { bytes.SetCount(b.bytes.GetCount()); if(!bytes.IsEmpty()) memcpy(bytes.Begin(), b.bytes.Begin(), bytes.GetCount()); }
	ImageBuffer& operator=(const ImageBuffer& b) { if(this != &b) { ImageBuffer copy(b); *this = pick(copy); } return *this; }

	bool Allocate(const ImageSpec& spec)
	{
		int64 size;
		if(!spec.IsValid() || !spec.GetSampleCount(samples) || !spec.GetByteCount(size) || size > INT_MAX) { Clear(); return false; }
		Vector<byte> replacement;
		replacement.SetCount((int)size);
		bytes = pick(replacement);
		sample_type = spec.sample_type;
		return true;
	}
	void Clear() { bytes.Clear(); samples = 0; sample_type = SampleType::Invalid; }
	bool IsEmpty() const { return bytes.IsEmpty() && samples == 0 && sample_type == SampleType::Invalid; }
	bool IsValid() const { return Imaging::IsValid(sample_type) && samples >= 0 && bytes.GetCount() == samples * BytesPerSample(sample_type); }
	int64 GetSampleCount() const { return samples; }
	int GetByteCount() const { return bytes.GetCount(); }
	byte* Begin() { return bytes.Begin(); }
	const byte* Begin() const { return bytes.Begin(); }
};

struct Metadata {
	VectorMap<String, String> values;
	void Set(String key, String value) { values.GetAdd(key) = value; }
	bool Has(const String& key) const { return values.Find(key) >= 0; }
	bool TryGet(const String& key, String& value) const { int i = values.Find(key); if(i < 0) return false; value = values[i]; return true; }
	String Get(const String& key, String fallback = Null) const { String value; return TryGet(key, value) ? value : fallback; }
	bool Remove(const String& key) { int i = values.Find(key); if(i < 0) return false; values.Remove(i); return true; }
	void Clear() { values.Clear(); }
	int GetCount() const { return values.GetCount(); }
	const VectorMap<String, String>& Items() const { return values; }
	bool operator==(const Metadata& b) const { return values == b.values; }
};

struct ImageData {
	ImageSpec spec;
	ImageBuffer buffer;
	Metadata metadata;
	bool IsValid() const { int64 samples; return spec.IsValid() && buffer.IsValid() && spec.GetSampleCount(samples) && buffer.GetSampleCount() == samples; }
	void Clear() { spec = ImageSpec(); buffer.Clear(); metadata.Clear(); }
};

enum class ResultCode { Ok, InvalidArgument, InvalidSpecification, Unsupported, Overflow, AllocationFailure, IOError, InternalFailure };
struct Result {
	ResultCode code = ResultCode::Ok;
	String message;
	String context;
	bool IsOk() const { return code == ResultCode::Ok; }
	operator bool() const { return IsOk(); }
	static Result Success() { return {}; }
	static Result Failure(ResultCode code, String message, String context = Null) { Result r; r.code = code; r.message = message; r.context = context; return r; }
};

enum class DiagnosticSeverity { Information, Warning, Error };
struct DiagnosticEntry : Moveable<DiagnosticEntry> { DiagnosticSeverity severity; String code; String message; String context; };
struct Diagnostics {
	Vector<DiagnosticEntry> entries;
	void Add(DiagnosticSeverity severity, String message, String code = Null, String context = Null) { DiagnosticEntry entry; entry.severity = severity; entry.code = code; entry.message = message; entry.context = context; entries.Add(pick(entry)); }
	void Info(String message, String context = Null) { Add(DiagnosticSeverity::Information, message, Null, context); }
	void Warning(String message, String context = Null) { Add(DiagnosticSeverity::Warning, message, Null, context); }
	void Error(String message, String context = Null) { Add(DiagnosticSeverity::Error, message, Null, context); }
	void Clear() { entries.Clear(); }
	bool IsEmpty() const { return entries.IsEmpty(); }
	int GetCount() const { return entries.GetCount(); }
	int Count(DiagnosticSeverity severity) const { int n = 0; for(const auto& e : entries) if(e.severity == severity) ++n; return n; }
	bool HasWarnings() const { return Count(DiagnosticSeverity::Warning) != 0; }
	bool HasErrors() const { return Count(DiagnosticSeverity::Error) != 0; }
};

} // namespace Imaging
} // namespace Upp

#endif
