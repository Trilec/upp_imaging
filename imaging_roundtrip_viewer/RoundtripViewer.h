#ifndef UPP_IMAGING_ROUNDTRIP_VIEWER_H
#define UPP_IMAGING_ROUNDTRIP_VIEWER_H

#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

#include <imaging_roundtrip_test_support/ImagingRoundtripTest.h>
#include <jpeg_io/JpegIO.h>
#include <tiff_io/TiffIO.h>

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
	enum FormatKind { FORMAT_EXR = 0, FORMAT_PNG = 1, FORMAT_JPEG = 2, FORMAT_TIFF = 3 };
	enum ProfileKind { PROFILE_EXR_HALF_ZIP = 0, PROFILE_EXR_FLOAT_NONE = 1, PROFILE_PNG_RGBA8 = 2, PROFILE_JPEG_RGB95_444 = 3, PROFILE_TIFF_RGBA8_DEFLATE = 4, PROFILE_TIFF_RGBA16_DEFLATE = 5, PROFILE_TIFF_RGBAF_NONE = 6 };
	enum TiffProfileKind { TIFF_RGBA8_DEFLATE = 0, TIFF_RGBA16_DEFLATE = 1, TIFF_RGBAF_NONE = 2 };
	enum DisplayKind { DISPLAY_RGB = 0, DISPLAY_RAW_RGB = 1, DISPLAY_ALPHA = 2 };

	struct ProfileSpec {
		const char* name;
		FormatKind format;
		int width;
		int height;
		bool include_hdr;
		bool output_half;
		bool use_zip;
		JpegSaveOptions jpeg_options;
		TiffSaveOptions tiff_options;
		TiffProfileKind tiff_kind;
		bool lossy;
		double max_mae;
		double max_rmse;
		double min_psnr;
	};

	static const ProfileSpec& GetProfile(ProfileKind kind);
	static bool IsExactPass(const RoundtripComparison& cmp);
	static bool IsExactPass(const RoundtripComparison16& cmp);
	static bool IsLossyPass(const LossyRgbComparison& cmp, const ProfileSpec& spec);
	static int GetGainValue(const Value& v);

	void SetRunState(int state, const String& message = String());

	void RunSelected();
	void RunProfile(ProfileKind kind);
	void RunTiffProfile(const ProfileSpec& spec);
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

	enum RunState { RUN_NOT_RUN = 0, RUN_RUNNING = 1, RUN_PASS = 2, RUN_FAIL = 3 };
	int run_state_ = RUN_NOT_RUN;
	String run_message_;

	PreviewPane generated_pane_;
	PreviewPane reloaded_pane_;
	PreviewPane difference_pane_;

	TestImageF generated_;
	TestImageF reloaded_;
	RoundtripComparison comparison_;
	RoundtripComparison16 comparison16_;
	LossyRgbComparison rgb_comparison_;
	String io_error_;
	String output_path_;
	long long output_size_ = -1;
};

} // namespace Upp

#endif
