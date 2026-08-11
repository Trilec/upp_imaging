# openimageio_plugin_hdr

Static OpenImageIO 3.1.15.0 Radiance HDR/RGBE plugin registration for the U++
imaging nest. The format is self-contained and introduces no new external codec
dependency.

The backend's native contract is three-channel Float32 RGB. RGBE encoding is
high-dynamic-range but quantized, so framework tests use representable HDR
values and comparison appropriate to the format rather than claiming arbitrary
Float32 bit preservation.
