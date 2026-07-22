#ifndef _ImagingWorkbench_ImagingWorkbench_h_
#define _ImagingWorkbench_ImagingWorkbench_h_

#include "ImagingWorkbench.generated.h"
#include "ImagingCanvas.h"

#include <imaging_preview_coalescing/imaging_preview_coalescing.h>
#include <imaging_tone_conversion/imaging_tone_conversion.h>
#include <imaging_roundtrip_viewer_ocio/OcioPreview.h>

#include <oiio/OIIO.h>

namespace Upp {

namespace OCIO = OCIO_NAMESPACE;

struct PreviewGroup : Moveable<PreviewGroup> {
	String name;
	String channels_text;
	int subimage = 0;
	int channel_count = 0;
	int red = -1;
	int green = -1;
	int blue = -1;
	int alpha = -1;
	int single_channel = -1;

	bool HasRGB() const { return red >= 0 && green >= 0 && blue >= 0; }
	bool HasAlpha() const { return alpha >= 0; }
	bool HasSingle() const { return single_channel >= 0; }
};

enum class ChannelView {
	RGB,
	Red,
	Green,
	Blue,
	Alpha,
};

struct ImageSubimageInfo : Moveable<ImageSubimageInfo> {
	Size size;
	String pixel_type;
	String color_space;
	int channel_count = 0;
};

struct PreviewProxy : Moveable<PreviewProxy> {
	int group_index = -1;
	Size source_size;
	Size proxy_size;
	int channel_count = 0;
	int red = -1;
	int green = -1;
	int blue = -1;
	int alpha = -1;
	int single_channel = -1;
	bool has_alpha = false;
	Vector<float> pixels;

	bool IsValid() const { return group_index >= 0 && proxy_size.cx > 0 && proxy_size.cy > 0 && channel_count > 0 && pixels.GetCount() > 0; }
};

struct PreviewTimingBreakdown {
	double tone_ms = 0.0;
	double convert_ms = 0.0;
	double publish_ms = 0.0;
	double ui_ms = 0.0;
	double total_ms = 0.0;
};

class ImagingWorkbench : public ImagingWorkbenchLayout {
public:
	typedef ImagingWorkbench CLASSNAME;

	ImagingWorkbench();

protected:
	virtual void BindActions() override;
	virtual void PostBuild() override;
	virtual void Paint(Draw& w) override;
	virtual bool HotKey(dword key) override;

private:
	void DoLoad();
	bool LoadImageFile(const String& path, String& error, bool populate_ui = true);
	void UpdateDisplayState();
	void UpdateCanvasZoomLabel();
	void UpdateLayersPage();
	void UpdateSelectionSummary();
	void ScanSourceMetadata();
	void UpdatePreviewSelection();
	void UpdateViewerControls();
	void UpdateOcioControls();
	void ApplyChannelView(ChannelView view);
	void ApplyExposureStops(double value, bool immediate = false);
	void ApplyDisplayGamma(double value, bool immediate = false);
	void SchedulePreviewRender(bool immediate = false);
	void BuildSelectedGroupProxy();
	void RenderPreviewFromProxy();
	bool ApplyOcioPreview(TestImageF& image, String& error) const;
	String GetOcioSummary() const;
	String GetImageColorSpace() const;
	void UpdateProbe(Point image_point);
	void ClearProbe();
	void DoSave();
	void DoSaveFormat(const Value& data);
	bool SaveCurrentImage(String& path, const String& format, String& error);
	void SetStatus(const String& text);
	void RecordTiming(const String& phase, double milliseconds, const PreviewProxy* proxy = nullptr);
	String FormatBytes(int64 bytes) const;
	String DescribePreviewChoice() const;
	String DescribeChannelView() const;
	String DescribeSelectedGroup() const;
	String FormatProbeValue(float value) const;
	bool IsViewAvailable(ChannelView view) const;
	static bool IsRGBChannelName(const String& name);
	static bool IsAlphaChannelName(const String& name);
	static String GroupPrefix(const String& name);

	ImagingCanvas canvas;
	UiBoxLayout layers_layout;
	UiLabel layers_summary;
	UiTree layers_tree;
	UiLabel layers_detail;
	UiLabel ocio_body;
	UiLabel ocio_enable_label;
	UiLabel ocio_config_label;
	UiLabel ocio_source_label;
	UiLabel ocio_display_label;
	UiLabel ocio_view_label;
	UiLabel ocio_error;
	UiLabel analysis_body;
	UiDropdown ocio_enable_drop;
	UiDropdown ocio_config_drop;
	UiDropdown ocio_source_drop;
	UiDropdown ocio_display_drop;
	UiDropdown ocio_view_drop;

	OIIO::ImageBuf source_image;
	Image preview_image;
	Vector<PreviewProxy> proxy_cache;
	Vector<float> probe_source_pixel;
	String source_filename;
	String last_saved_filename;
	String last_save_format = "EXR";
	String last_error;
	Vector<PreviewGroup> preview_groups;
	Vector<ImageSubimageInfo> subimages;
	int subimage_count = 0;
	int selected_preview_group = -1;
	ChannelView channel_view = ChannelView::RGB;
	double exposure_stops = 0.0;
	double display_gamma = 1.0;
	bool syncing_view_controls = false;
	ToneConversionState preview_tone;
	double preview_tone_gamma = -1.0;
	PreviewRenderCoalescer preview_render_coalescer;
	PreviewTimingBreakdown preview_timing;
	String timing_summary;
	String resolution_text;
	String memory_text;
	OCIO::ConstConfigRcPtr ocio_config;
	bool ocio_controls_updating = false;
};

} // namespace Upp

#endif
