# Usage

## EXR save

```cpp
#include <openexr_io/OpenExrIO.h>

const char* path = "example.exr";

ExrRgbaImageF image;
image.width = 2;
image.height = 2;
image.pixels.SetCount(4);

Upp::String error;
if(!SaveExrRgbaF(path, image, true, true, &error)) {
    // output_half = true
    // use_zip = true
}
```

`output_half = true` stores HALF; `use_zip = true` uses ZIP compression.

## EXR load

```cpp
const char* path = "example.exr";

ExrRgbaImageF image;
Upp::String error;

if(!LoadExrRgbaF(path, image, &error)) {
    ...
}
```

RGB is required; missing alpha becomes `1.0f`.

## PNG save

```cpp
#include <png_io/PngIO.h>

const char* path = "example.png";

PngRgbaImage8 image;
image.width = 2;
image.height = 2;
image.pixels.SetCount(4);

Upp::String error;
if(!SavePngRgba8(path, image, &error)) {
    ...
}
```

## PNG load

```cpp
const char* path = "example.png";

PngRgbaImage8 image;
Upp::String error;

if(!LoadPngRgba8(path, image, &error)) {
    ...
}
```

Pixels are straight, non-premultiplied RGBA8.

## JPEG save

```cpp
#include <jpeg_io/JpegIO.h>

const char* path = "example.jpg";

JpegRgbImage8 image;
image.width = 2;
image.height = 2;
image.pixels.SetCount(4);

Upp::String error;
JpegSaveOptions options;
options.quality = 95;
options.subsampling = JpegSubsampling::S444;
options.progressive = false;
options.optimize_coding = true;

if(!SaveJpegRgb8(path, image, options, &error)) {
    ...
}
```

## JPEG load

```cpp
const char* path = "example.jpg";

JpegRgbImage8 image;
Upp::String error;

if(!LoadJpegRgb8(path, image, &error)) {
    ...
}
```

JPEG is RGB-only. Alpha is not stored or composited by `jpeg_io`.
The output is lossy. ICC profiles and metadata preservation are not implemented.
CMYK and YCCK are rejected. Malformed and truncated files fail strictly.
`jpeg_io` is validated under `CLANGx64`.

## TIFF save

```cpp
#include <tiff_io/TiffIO.h>

const char* path = "example.tif";

TiffRgbaImage8 image;
image.width = 2;
image.height = 2;
image.pixels.SetCount(4);

TiffSaveOptions options;
options.compression = TiffCompression::Deflate;

Upp::String error;
if(!SaveTiffRgba8(path, image, options, &error)) {
    ...
}
```

## TIFF load

```cpp
const char* path = "example.tif";

TiffRgbaImage8 image;
Upp::String error;

if(!LoadTiffRgba8(path, image, &error)) {
    ...
}
```

Use the typed API that matches the stored TIFF sample type.
`SaveTiffRgba16()` / `LoadTiffRgba16()` are for 16-bit RGBA.
`SaveTiffRgbaF()` / `LoadTiffRgbaF()` are for 32-bit float RGBA.
TIFF support is classic, single-directory, scanline RGBA only.

## Build

Console test:

```text
E:\upp-18468\umk.exe "E:/apps/github/upp_imaging,E:/upp-18468/uppsrc" png_io_test CLANGx64 -abr "E:/apps/github/upp_imaging/out/png_io_test.exe"
```

Viewer:

```text
E:\upp-18468\umk.exe "E:/apps/github/upp_imaging,E:/apps/github/upp_Ui,E:/apps/github/upp_AnimationEasing,E:/upp-18468/uppsrc" imaging_roundtrip_viewer CLANGx64 -abr "E:/apps/github/upp_imaging/out/imaging_roundtrip_viewer.exe"
```

`GitHubOut.var` is local-only. `GitHubOut.var.example` documents the symbolic nest requirement.
