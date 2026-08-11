# ImagingDiagnostics

`ImagingDiagnostics` is the GUI-independent reporting and validation layer for the backend-neutral `Upp::Imaging` framework.

It depends only on `ImagingCore` and provides:

- deterministic absolute/relative numerical comparison helpers;
- array comparison summaries with first mismatch and maximum errors;
- stable names for ImagingCore enums;
- structured image-specification, metadata, result and diagnostics reports;
- operation snapshots combining result, diagnostics and elapsed time;
- deterministic text rendering for tests, logs and future ImagingWorkbench presentation.

`ImagingCore::Diagnostics` remains the authoritative diagnostics container. This package formats and compares existing Core contracts; it does not duplicate backend or GUI state.
