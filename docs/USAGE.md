# Usage

## EXR save

```cpp
#include <openexr_io/OpenExrIO.h>

ExrRgbaImageF image;
image.width = 2;
image.height = 2;
image.pixels.SetCount(4);

String error;
if(!SaveExrRgbaF(path, image, true, true, &error)) {
    // output_half = true
    // use_zip = true
}
```

`output_half = true` stores HALF; `use_zip = true` uses ZIP compression.

## EXR load

```cpp
ExrRgbaImageF image;
String error;

if(!LoadExrRgbaF(path, image, &error)) {
    ...
}
```

RGB is required; missing alpha becomes `1.0f`.

## PNG save

```cpp
#include <png_io/PngIO.h>

PngRgbaImage8 image;
image.width = 2;
image.height = 2;
image.pixels.SetCount(4);

String error;
if(!SavePngRgba8(path, image, &error)) {
    ...
}
```

## PNG load

```cpp
PngRgbaImage8 image;
String error;

if(!LoadPngRgba8(path, image, &error)) {
    ...
}
```

Pixels are straight, non-premultiplied RGBA8.

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
