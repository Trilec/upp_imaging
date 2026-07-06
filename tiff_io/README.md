# tiff_io

Reusable TIFF IO helper built on `libtiff`.

- RGBA8, RGBA16 and RGBA float save/load
- exact scanline storage with top-left orientation
- classic TIFF only
- compression: none, LZW, Deflate
- straight RGBA input, no premultiplication
- no metadata or ICC preservation
- CMYK, grayscale, palette, tiled, multi-directory and planar-separate inputs are rejected
- malformed and truncated TIFF files fail strictly
- validated under `CLANGx64`
