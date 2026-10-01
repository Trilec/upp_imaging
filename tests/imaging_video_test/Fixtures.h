#ifndef _imaging_video_test_Fixtures_h_
#define _imaging_video_test_Fixtures_h_

#include <Core/Core.h>

// Derive a three-frame clip from the original single-frame AVC fixture.
// All samples are independently decodable IDR frames. Fixed-size sample tables
// keep the original first-chunk offset; only mdat grows.
static Upp::uint32 VideoBE32(const Upp::String& data, int offset)
{
	return (Upp::uint32(Upp::byte(data[offset])) << 24) |
	       (Upp::uint32(Upp::byte(data[offset + 1])) << 16) |
	       (Upp::uint32(Upp::byte(data[offset + 2])) << 8) |
	        Upp::uint32(Upp::byte(data[offset + 3]));
}
static void SetVideoBE32(Upp::String& data, int offset, Upp::uint32 value)
{
	for(int i = 0; i < 4; ++i) data.Set(offset + i, char(value >> (24 - i * 8)));
}
static bool ExtendVideoTables(Upp::String& data, int begin, int end, int count, int& mdat)
{
	for(int pos = begin; pos < end;) {
		if(end - pos < 8) return false;
		Upp::uint32 size = VideoBE32(data, pos);
		if(size < 8 || size > Upp::uint32(end - pos)) return false;
		Upp::String type = data.Mid(pos + 4, 4);
		if(type == "moov" || type == "trak" || type == "mdia" || type == "minf" ||
		   type == "stbl" || type == "edts") {
			if(!ExtendVideoTables(data, pos + 8, pos + size, count, mdat)) return false;
		}
		else if(type == "mvhd" || type == "mdhd") {
			if(size < 28 || data[pos + 8] != 0) return false;
			SetVideoBE32(data, pos + 24, VideoBE32(data, pos + 24) * count);
		}
		else if(type == "tkhd") {
			if(size < 32 || data[pos + 8] != 0) return false;
			SetVideoBE32(data, pos + 28, VideoBE32(data, pos + 28) * count);
		}
		else if(type == "elst") {
			if(size < 28 || data[pos + 8] != 0 || VideoBE32(data, pos + 12) != 1) return false;
			SetVideoBE32(data, pos + 16, VideoBE32(data, pos + 16) * count);
		}
		else if(type == "stts") {
			if(size < 24 || VideoBE32(data, pos + 12) != 1 || VideoBE32(data, pos + 16) != 1) return false;
			SetVideoBE32(data, pos + 16, count);
		}
		else if(type == "stsc") {
			if(size < 28 || VideoBE32(data, pos + 12) != 1 || VideoBE32(data, pos + 20) != 1) return false;
			SetVideoBE32(data, pos + 20, count);
		}
		else if(type == "stsz") {
			if(size < 20 || !VideoBE32(data, pos + 12) || VideoBE32(data, pos + 16) != 1) return false;
			SetVideoBE32(data, pos + 16, count);
		}
		else if(type == "mdat") mdat = pos;
		pos += size;
	}
	return true;
}
static Upp::String ThreeFrameVideo(const Upp::String& original)
{
	Upp::String data = original;
	int mdat = -1;
	if(!ExtendVideoTables(data, 0, data.GetLength(), 3, mdat) || mdat < 0) return Upp::String();
	int size = VideoBE32(data, mdat);
	Upp::String payload = data.Mid(mdat + 8, size - 8);
	SetVideoBE32(data, mdat, 8 + payload.GetLength() * 3);
	return data.Left(mdat + 8) + payload + payload + payload + data.Mid(mdat + size);
}
#endif
