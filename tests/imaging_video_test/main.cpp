#include <ImagingVideo/ImagingVideo.h>
#include "../ffmpeg_first_frame_test/Fixture.h"
#include <cstring>
#include "Fixtures.h"

using namespace Upp;
using namespace Upp::Imaging;

struct State { int passed = 0, failed = 0; };
static void Check(State& s, bool ok, const char* label) {
	Cout() << (ok ? "PASS " : "FAIL ") << label << '\n'; (ok ? s.passed : s.failed)++;
}
static bool SameImage(const Image& a, const Image& b) {
	return a.GetSize() == b.GetSize() && !a.IsEmpty() &&
		memcmp(a.Begin(), b.Begin(), a.GetLength() * sizeof(RGBA)) == 0;
}
CONSOLE_APP_MAIN
{
	State s;
	String root = AppendFileName(GetCurrentDirectory(), "build/windows-x64/samples");
	RealizeDirectory(root);
	String mp4 = AppendFileName(root, "h264.mp4"), mov = AppendFileName(root, "h264-quicktime.mov");
	String encoded = ThreeFrameVideo(String(kFfmpegFirstFrameFixture, kFfmpegFirstFrameFixtureSize));
	Check(s, !encoded.IsEmpty() && SaveFile(mp4, encoded), "embedded H.264 MP4 sample retained");
	// QuickTime-brand MOV variant; encoded AVC and box layout remain unchanged.
	String quicktime = encoded;
	for(int i = 0; i < 4; ++i) quicktime.Set(8 + i, "qt  "[i]);
	Check(s, SaveFile(mov, quicktime), "QuickTime-brand MOV sample retained");
	VideoReader reader;
	Check(s, reader.Open(mp4), "reusable reader opens MP4");
	Check(s, reader.GetSize() == Size(16, 16) && reader.GetDurationMs() == 3000, "video dimensions and duration");
	VideoFrame frame;
	Check(s, reader.ReadNext(frame) && frame.image.GetSize() == Size(16, 16) && frame.time_ms == 0,
	      "timestamped U++ video frame decodes");
	Image first = frame.image;
	Check(s, !first.IsEmpty() && first[0][0].a == 255, "video image is opaque");
	Check(s, !first.IsEmpty() && abs(first[0][0].r - 49) <= 2 &&
	         abs(first[0][0].g - 100) <= 2 && abs(first[0][0].b - 201) <= 2,
	      "decoded video RGB matches the retained pixel contract");
	Check(s, reader.ReadNext(frame) && frame.time_ms == 1000 && SameImage(first, frame.image), "second frame timestamp");
	Check(s, reader.ReadNext(frame) && frame.time_ms == 2000 && SameImage(first, frame.image), "third frame timestamp");
	Check(s, !reader.ReadNext(frame) && reader.IsEof() && reader.GetError().IsEmpty() &&
	         SameImage(first, frame.image), "EOF preserves caller frame without an error");
	Check(s, reader.Seek(0) && !reader.IsEof() && reader.ReadNext(frame) &&
	         SameImage(first, frame.image), "seek resets decoder and reproduces first frame");
	Check(s, reader.Seek(1500) && reader.ReadNext(frame) && frame.time_ms == 2000,
	      "seek discards frames preceding requested time");
	Check(s, !reader.Seek(-1) && !reader.Seek(3000), "seek outside clip rejected");
	Check(s, !reader.Open("missing-video.mp4") && reader.IsOpen(), "failed open preserves previous reader");
	Check(s, reader.Seek(0) && reader.ReadNext(frame) && SameImage(first, frame.image),
	      "previous video remains usable after failed open");
	Check(s, reader.Open(mov) && reader.ReadNext(frame) && SameImage(first, frame.image),
	      "QuickTime-brand MOV decoding matches MP4");
	reader.Close();
	Check(s, !reader.IsOpen() && reader.GetSize() == Size(0, 0) && !reader.ReadNext(frame) &&
	         SameImage(first, frame.image), "closed reader preserves output and reports unavailable");

	VideoInputLimits limits;
	limits.file_bytes = encoded.GetLength() - 1;
	reader.SetLimits(limits);
	Check(s, !reader.Open(mp4), "encoded video file size cap rejects");
	limits = VideoInputLimits(); limits.pixels = 255; reader.SetLimits(limits);
	Check(s, !reader.Open(mp4), "video pixel limit rejects container dimensions");
	limits = VideoInputLimits(); limits.dimension = 15; reader.SetLimits(limits);
	Check(s, !reader.Open(mp4), "video dimension limit rejects");
	limits = VideoInputLimits(); limits.packet_bytes = 1; reader.SetLimits(limits);
	Check(s, reader.Open(mp4) && !reader.ReadNext(frame) && !reader.GetError().IsEmpty() &&
	         SameImage(first, frame.image), "packet limit rejects without replacing output");
	reader.Close();
	limits = VideoInputLimits(); reader.SetLimits(limits);
	String oversized = encoded;
	int stsd = oversized.Find("stsd");
	int avc = stsd >= 0 ? oversized.Find("avc1", stsd + 4) : -1;
	bool changed = avc >= 4 && avc + 31 < oversized.GetLength();
	if(changed) {
		oversized.Set(avc + 28, char(0x40)); oversized.Set(avc + 29, char(0));
	}
	String hostile = AppendFileName(root, "oversized-header.mp4");
	Check(s, changed && SaveFile(hostile, oversized) && !reader.Open(hostile) &&
	         reader.GetError().Find("dimension/pixel limits") >= 0,
	      "oversized AVC header rejected before stream-info decoding");
	DeleteFile(hostile);
	Check(s, reader.Open(mp4) && reader.ReadNext(frame) && SameImage(first, frame.image),
	      "normal reader remains usable after crafted header rejection");
	reader.Close();
	Check(s, !reader.Open("https://example.invalid/video.mp4"), "network URL rejected");
	Cout() << "SUMMARY passed=" << s.passed << " failed=" << s.failed << '\n';
	SetExitCode(s.failed ? 1 : 0);
}
