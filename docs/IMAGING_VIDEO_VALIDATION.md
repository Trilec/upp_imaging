# Windows Debug video integration — 1 October 2026

## Subsequent security repair — 2 October 2026

The reader no longer calls avformat_find_stream_info: supported H.264 dimensions must be present in the MP4/MOV header, before its single decoder is opened with max_pixels. Duration uses the selected stream without probing. Oversized header and continued valid-open checks pass in the current 24-check reader block. Post-repair Workbench regression passed 166 checks. See [current artifact identities and precise run scope](SECURITY_VALIDATION_20261002.md). The older identities and 22-check result below remain historical evidence.

Base HEAD: d944562640194668377f8832af0c762f34aac3dc. Builds include this
checkpoint's uncommitted source; HEAD alone does not identify the compiled tree.
No Release, Linux/macOS, sanitizer or fuzz run is claimed.

| Target | Checks | Exit | Klick run |
| --- | ---: | ---: | --- |
| imaging_video_test | 22 | 0 | exec-845273AD491FBF84A757886711197C86 |
| imaging_workbench_ocio_test | 166 | 0 | exec-4976C61A8ABB86FEDC841C7EBE0EA32B |

Reader artifact: build/imaging_video_debug.exe, SHA-256
0916bf2fa58c44bcfaec1f43d588b9c0d197380520d4b82ba74fbbb28b512a4c.
Combined Workbench test: build/imaging_workbench_ocio_debug.exe, SHA-256
e1534891c9d8cd1fc55912296c4d5fc71d5d371126e9aee9434bbe04a60a1001.
The actual app built in exec-CF7AC0EB678121AD36B28D9F53534E2B:
build/ImagingWorkbench_debug.exe, SHA-256
b926e5f36bc0f96135cc1ab36bad700a22b7c48f36bd72c3bcfe4e4214aaf9ed.
This hash identifies the new build, not Curt's previously accepted executable.
Workbench manually accepted by Curt. The new controls have deterministic
regression evidence; no new full manual GUI checklist was run.

The reader checks three timestamped frames, exact first-frame RGB, seeking,
clean EOF, failure preservation and input policies. Workbench checks opening,
pause preserving a prefetched frame, stepping, seek, failed-open preservation,
close cancelling playback and still-image replacement. Timing performance is
not measured. MOV coverage uses a QuickTime-branded variant of the MP4 fixture;
it does not prove every real-world MOV variant. Generated samples are under
build/windows-x64/samples. FFmpeg's scalar swscale informational messages are
retained in stderr; existing ImagingIO LNK4217 warnings remain in build output.

The complete Debug acceptance block is tracked in WINDOWS_ACCEPTANCE.md.
Security clearance and final Release publication remain separate gates.

The benchmark consumer initially failed to link IOFormatPolicy because its
manifest lacked ImagingIO. Adding the direct dependency fixed the Debug build
in exec-6B3F66B3ED0BF3B65BF92E15304E0F21, exit 0. No benchmark timing run
or performance claim was made. The full retained Debug set passed 51 targets,
1,482 checks and 51 exits at 0; see WINDOWS_ACCEPTANCE.md.
