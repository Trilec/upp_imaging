# ImagingVideo

Add ImagingVideo to `uses` and include `<ImagingVideo/ImagingVideo.h>`.
`Upp::Imaging::VideoReader` opens local H.264 MP4/MOV files, reads timestamped
opaque U++ images, seeks to a keyframe and discards frames preceding the requested
time. `ReadNext` returns false with an empty error at EOF. Failed Open preserves
the previous reader; failed ReadNext preserves the caller's frame.

Defaults: 256 MiB input file, 8,388,608 decoded pixels, 8192 per dimension,
16 MiB packet, 4096 packet/frame steps per call, 3-second operation budget,
16 streams and 16 MiB demux index. Network protocols and UNC paths are rejected;
audio is ignored. Track codec and dimensions must be present in the container
header. Stream-info decoding is disabled; incomplete headers are rejected before
opening the sole H.264 decoder with its pixel budget. Decoder is the scalar native H.264 slice, one thread.

The interruption/deadline check is cooperative, not a hard CPU kill of a codec
call. Packet limits are checked after demux allocation. Native metadata, decoder
reference frames and scratch require separate security review; these policies
are not a blanket security clearance or validation of other platforms.
