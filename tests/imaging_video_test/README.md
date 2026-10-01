# U++ video reader contract

Three independent IDR samples are derived from the retained single-frame H.264
MP4 by updating fixed-size sample tables/durations and repeating its packet. The
QuickTime-brand MOV variant changes the major container brand. This is a narrow,
reproducible container test, not broad camera-MOV coverage. Decode, exact RGB,
frame timestamps, seek, EOF/failure preservation and limits are checked together.
Samples remain in build/windows-x64/samples.
