# ImagingAnalysis

`ImagingAnalysis` is the Core-only reusable analysis layer for `Upp::Imaging`.

The initial slice provides:

- per-channel finite/non-finite statistics;
- normalized `[0,1]` histograms with explicit below/above-range counts;
- source-coordinate pixel probes across data-window origins and depth slices;
- all `ImagingCore` sample types and all channels, including named MultiChannel data;
- transactional outputs and stable `IMGANALYSIS_*` diagnostics.

UInt8 and UInt16 samples are normalized to `[0,1]`. Float16 and Float32 values retain their source numeric values. Histograms include only finite in-range values in bins while statistics retain finite out-of-range evidence.

Waveforms, vectorscopes, GUI controls and Workbench policy are deliberately outside this first framework slice.
