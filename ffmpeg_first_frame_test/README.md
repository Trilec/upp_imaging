# ffmpeg_first_frame_test

End-to-end deterministic acceptance contract for the first FFmpeg video slice.

The test embeds a 16x16 one-frame MP4 containing Constrained Baseline H.264, writes it to a temporary local file, opens it through the `file` protocol and MOV demuxer, decodes the native H.264 frame, converts YUV420P to RGBA8 through scalar libswscale with explicit limited-range ITU-601 policy, and verifies exact fixture/decoded-frame evidence before complete cleanup.

Expected result: `SUMMARY passed=27 failed=0`.
