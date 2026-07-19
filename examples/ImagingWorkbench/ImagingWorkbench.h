#ifndef _ImagingWorkbench_ImagingWorkbench_h_
#define _ImagingWorkbench_ImagingWorkbench_h_

#include "ImagingWorkbench.generated.h"
#include "ImagingCanvas.h"

#include <oiio/OIIO.h>

namespace Upp {

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
	void UpdateDisplayState();
	void UpdateCanvasZoomLabel();
	void UpdateLayersPage();
	void UpdateSelectionSummary();
	void ScanSourceMetadata();
	void UpdatePreviewSelection();
	void UpdateViewerControls();
	void ApplyChannelView(ChannelView view);
	void ApplyExposureStops(double value);
	void ApplyDisplayGamma(double value);
	void BuildPreviewImage();
	void UpdateProbe(Point image_point);
	void ClearProbe();
	void DoSave();
	void DoSaveFormat(const Value& data);
	bool SaveCurrentImage(String& path, const String& format, String& error);
	void SetStatus(const String& text);
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
	UiLabel analysis_body;

	OIIO::ImageBuf source_image;
	Image preview_image;
	Size preview_size;
	Vector<float> probe_pixels;
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
	String resolution_text;
	String memory_text;
};

} // namespace Upp

#endif
