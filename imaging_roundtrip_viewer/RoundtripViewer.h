#ifndef UPP_IMAGING_ROUNDTRIP_VIEWER_H
#define UPP_IMAGING_ROUNDTRIP_VIEWER_H

#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

#include <imaging_roundtrip_test_support/ImagingRoundtripTest.h>

namespace Upp {

class PreviewPane : public UiBoxLayout {
public:
	typedef PreviewPane CLASSNAME;

	PreviewPane();

	PreviewPane& SetPaneTitle(const String& text);
	PreviewPane& SetPaneImage(const Image& img);

private:
	class PreviewCanvas : public Ctrl {
	public:
		typedef PreviewCanvas CLASSNAME;

		void SetImage(const Image& img);
		Size GetMinSize() const override;
		void Paint(Draw& w) override;

	private:
		Image image_;
	};

	UiLabel title_;
	PreviewCanvas canvas_;
};

class RoundtripViewerWindow : public TopWindow {
public:
	typedef RoundtripViewerWindow CLASSNAME;

	RoundtripViewerWindow();

private:
	enum ProfileKind { PROFILE_HALF_ZIP = 0, PROFILE_FLOAT_NONE = 1 };
	enum DisplayKind { DISPLAY_RGB = 0, DISPLAY_RAW_RGB = 1, DISPLAY_ALPHA = 2 };

	struct ProfileSpec {
		const char* name;
		int width;
		int height;
		bool include_hdr;
		bool output_half;
		bool use_zip;
	};

	static const ProfileSpec& GetProfile(ProfileKind kind);
	static bool IsExactPass(const RoundtripComparison& cmp);
	static int GetGainValue(const Value& v);

	void RunSelected();
	void RunProfile(ProfileKind kind);
	void RefreshViews();
	void UpdateStatus();
	void UpdateDetails();
	Image MakeDisplayImage(const TestImageF& img, DisplayKind kind) const;
	Image MakeDifferenceImage(const TestImageF& a, const TestImageF& b, int gain) const;

	static byte ClampByte(float v);
	static float Clamp01(float v);
	static byte BlendByte(byte fg, byte bg, float alpha);

	UiBoxLayout root_ { UiBoxLayout::Direction::V };
	UiBoxLayout top_row_ { UiBoxLayout::Direction::H };
	UiBoxLayout panes_row_ { UiBoxLayout::Direction::H };
	UiBoxLayout bottom_col_ { UiBoxLayout::Direction::V };
	UiBoxLayout path_row_ { UiBoxLayout::Direction::H };
	UiBoxLayout error_row_ { UiBoxLayout::Direction::H };

	UiLabel profile_label_;
	UiLabel display_label_;
	UiLabel gain_label_;
	UiLabel status_label_;
	UiLabel details_label_;
	UiLabel path_label_;
	UiLabel error_label_;

	UiDropdown profile_drop_;
	UiDropdown display_drop_;
	UiDropdown gain_drop_;
	UiButton run_button_;

	PreviewPane generated_pane_;
	PreviewPane reloaded_pane_;
	PreviewPane difference_pane_;

	TestImageF generated_;
	TestImageF reloaded_;
	RoundtripComparison comparison_;
	String io_error_;
	String output_path_;
	long long output_size_ = -1;
};

} // namespace Upp

#endif
