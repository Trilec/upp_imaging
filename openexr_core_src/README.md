# openexr_core_src

`openexr_core_src` is the first honest package attempt for OpenEXRCore.

Current scope:

- packages the public C-layer header surface needed for compile/link probing
- links a small real implementation subset around `base.c`
- does not claim full OpenEXRCore coverage yet
- does not claim EXR read/write support

Local imported-source adjustment:

- `upstream/openexr_config.h` was minimally adjusted to use quoted local includes for `IlmThreadConfig.h` and `ImathConfig.h` so the imported header surface works under the U++ package include model

Why this shape:

- it proves the generated-config and public C API surface can live in a U++ package
- it keeps the current claim modest while the larger compression/context/chunk machinery is still being sorted out
- OpenEXRCore 2x2 uncompressed Y/HALF pixel write/read round-trip passes under CLANGx64
