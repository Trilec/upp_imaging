# jpeg_io

Reusable RGB8 JPEG helper built on `libjpeg_turbo`.

- RGB8 load/save only
- quality range 1 to 100
- 4:4:4, 4:2:2, and 4:2:0 subsampling options
- progressive and baseline save modes
- straight RGB input, no alpha compositing
- no ICC or metadata preservation
- CMYK and YCCK are rejected
- malformed and truncated JPEG files fail strictly
- validated under `CLANGx64`
