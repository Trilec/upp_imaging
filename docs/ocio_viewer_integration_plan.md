# OCIO Viewer Integration Plan

Add OCIO into `imaging_roundtrip_viewer` as a CPU-side preview transform only.
The decoded image stays raw for round-trip validation; OCIO affects preview panes, not file comparison.

Planned controls:
- OCIO enabled/off
- builtin config selector
- source color space
- display
- view
- look, optional
- exposure/gamma, optional

Placement:
- decode -> raw test image
- raw image feeds round-trip metrics unchanged
- preview path applies OCIO before the current display conversion
- GPU/bgfx rendering stays a later task

Validation:
- original image remains unchanged in memory
- preview can toggle OCIO on/off
- exact EXR/PNG/TIFF metrics still use decoded pixels
- OCIO does not affect export unless we add that later
