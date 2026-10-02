# Standard JPEG
Static OpenImageIO 3.1.17.0 JPEG reader/writer using the repository's
libjpeg-turbo 3.2.0 scalar provider. No installed U++ JPEG codec, TurboJPEG,
external DLL, tools or UltraHDR dependency is enabled.
ImagingIO supports ordinary UInt8 Gray/RGB .jpg/.jpeg input/output.
JPEG is lossy and has no alpha channel. It is distinct from JPEG XL and JPEG XR.
High dynamic range is supplied by EXR and Radiance HDR, not this JPEG slice.
