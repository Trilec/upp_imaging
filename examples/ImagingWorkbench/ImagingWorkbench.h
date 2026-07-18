#ifndef _ImagingWorkbench_ImagingWorkbench_h_
#define _ImagingWorkbench_ImagingWorkbench_h_

#include "ImagingWorkbench.generated.h"
#include "ImagingCanvas.h"

#include <oiio/OIIO.h>

namespace Upp {

struct ImageChannelGroup : Moveable<ImageChannelGroup> {
	String name;
	String channels_text;
	int subimage = 0;
	int channel_count = 0;
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

private:
	void DoLoad();
	void UpdateDisplayState();
	void UpdateCanvasZoomLabel();
	void UpdateLayersPage();
	void UpdateSelectionSummary();
	void ScanSourceMetadata();
	void BuildPreviewImage();
	void SetStatus(const String& text);
	String FormatBytes(int64 bytes) const;
	String DescribePreviewChoice() const;
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
	String source_filename;
	String last_error;
	Vector<ImageChannelGroup> channel_groups;
	Vector<ImageSubimageInfo> subimages;
	int subimage_count = 0;
	String preview_choice;
	String resolution_text;
	String memory_text;
};

} // namespace Upp

#endif
