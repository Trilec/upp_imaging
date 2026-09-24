# Status and roadmap

The current repository separates framework, third-party sources, U++ integration, applications, tests, examples, tools and documentation. [Acceptance](WINDOWS_ACCEPTANCE.md) is authoritative for the current deterministic suite; [migration audit](STRUCTURE_MIGRATION.md) explains test and package removals.

The implemented still-image scope covers EXR, PNG, JPEG XL, HDR/RGBE, DPX/Cineon, RAW, WebP, HEIF/AVIF and TIFF. Direct native APIs and the optional EXR display plugin remain available. The bounded FFmpeg slice remains pinned to n9.0.1 with native H.264, MOV/MP4, local-file input and scalar RGBA conversion.

Future work remains separate: broader codecs/containers, audio, seeking, SIMD, hardware acceleration, a backend-neutral media wrapper, waveform/vectorscope analysis and provenance-reviewed real-camera fixtures. The known upstream OIIO TLS error-buffer limitation is documented in the source package README and HARDENING_REVIEW.md.
