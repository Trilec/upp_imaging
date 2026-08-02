#ifndef _ImagingCore_ImagingCore_h_
#define _ImagingCore_ImagingCore_h_

#include <Core/Core.h>

namespace Upp {
namespace Imaging {

enum class SampleType {
	UInt8,
	UInt16,
	Float16,
	Float32,
};

enum class ChannelLayout {
	Gray,
	GrayAlpha,
	RGB,
	RGBA,
	MultiChannel,
};

struct DataWindow {
	int left = 0;
	int top = 0;
	int right = -1;
	int bottom = -1;

	int Width() const { return IsValid() ? right - left + 1 : 0; }
	int Height() const { return IsValid() ? bottom - top + 1 : 0; }
	bool IsValid() const { return right >= left && bottom >= top; }
};

struct ImageSpec {
	int width = 0;
	int height = 0;
	int depth = 1;
	int channels = 0;
	ChannelLayout channel_layout = ChannelLayout::MultiChannel;
	SampleType sample_type = SampleType::UInt8;
	DataWindow data_window;

	bool IsValid() const
	{
		return width > 0 && height > 0 && depth > 0 && channels > 0 &&
			(data_window.IsValid() ? data_window.Width() == width && data_window.Height() == height : true);
	}
};

struct ImageBuffer {
	Vector<byte> bytes;
	SampleType sample_type = SampleType::UInt8;
	int samples = 0;

	static int BytesPerSample(SampleType type)
	{
		switch(type) {
		case SampleType::UInt8:   return 1;
		case SampleType::UInt16:  return 2;
		case SampleType::Float16: return 2;
		case SampleType::Float32: return 4;
		}
		return 0;
	}

	void SetSampleCount(int count, SampleType type)
	{
		samples = max(count, 0);
		sample_type = type;
		bytes.SetCount(samples * BytesPerSample(type));
	}

	int GetSampleCount() const { return samples; }
	int GetByteCount() const { return bytes.GetCount(); }
	bool IsValid() const { return samples >= 0 && GetByteCount() == samples * BytesPerSample(sample_type); }
};

struct Metadata {
	VectorMap<String, String> values;

	void Set(String key, String value) { values.GetAdd(key) = value; }
	bool Has(String key) const { return values.Find(key) >= 0; }
	String Get(String key, String fallback = Null) const
	{
		int index = values.Find(key);
		return index >= 0 ? values[index] : fallback;
	}
};

struct ImageData {
	ImageSpec spec;
	ImageBuffer buffer;
	Metadata metadata;

	bool IsValid() const
	{
		return spec.IsValid() && buffer.IsValid() &&
			buffer.GetSampleCount() == spec.width * spec.height * spec.depth * spec.channels;
	}
};

enum class ResultCode {
	Ok,
	InvalidArgument,
	Unsupported,
	NotFound,
	IOError,
	ProcessingError,
};

struct Result {
	ResultCode code = ResultCode::Ok;
	String message;

	bool IsOk() const { return code == ResultCode::Ok; }
	static Result Success() { return {}; }
	static Result Failure(ResultCode code, String message)
	{
		Result result;
		result.code = code;
		result.message = message.IsEmpty() ? "Imaging operation failed" : message;
		return result;
	}
};

struct Diagnostics {
	Vector<String> messages;

	void Add(String message) { messages.Add(message); }
	bool IsEmpty() const { return messages.IsEmpty(); }
};

} // namespace Imaging
} // namespace Upp

#endif
