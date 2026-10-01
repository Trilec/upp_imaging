#ifndef _ImagingVideo_ImagingVideo_h_
#define _ImagingVideo_ImagingVideo_h_

#include <Draw/Draw.h>
#include <memory>

namespace Upp { namespace Imaging {

struct VideoInputLimits {
	int64 file_bytes = 256 * 1024 * 1024;
	int64 pixels = 8 * 1024 * 1024;
	int dimension = 8192;
	int packet_bytes = 16 * 1024 * 1024;
	int packets_per_frame = 4096;
	int operation_ms = 3000;
};

struct VideoFrame {
	Image image;
	int64 time_ms = 0;
};

// One reader per decoding thread. Failed Open/ReadNext preserves caller output.
class VideoReader {
	struct Impl;
	std::unique_ptr<Impl> state;
	VideoInputLimits limits;
	String error;
public:
	VideoReader();
	~VideoReader();
	VideoReader(const VideoReader&) = delete;
	VideoReader& operator=(const VideoReader&) = delete;
	void SetLimits(const VideoInputLimits& value) { limits = value; }
	bool Open(const String& path);
	bool ReadNext(VideoFrame& frame);
	bool Seek(int64 time_ms);
	void Close();
	bool IsOpen() const;
	bool IsEof() const;
	Size GetSize() const;
	int64 GetDurationMs() const;
	const String& GetError() const { return error; }
};

}}
#endif
