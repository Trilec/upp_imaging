#include "RoundtripViewer.h"

#include <math.h>

#include <openexr_io/OpenExrIO.h>
#include <jpeg_io/JpegIO.h>
#include <tiff_io/TiffIO.h>
#include <png_io/PngIO.h>

namespace Upp {

static ExrRgbaImageF ToExrRgbaImageF(const TestImageF& src)
{
	ExrRgbaImageF out;
	out.width = src.width;
	out.height = src.height;
	if(src.width <= 0 || src.height <= 0)
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
		out.pixels[i].a = src.pixels[i].a;
	}
	return out;
}

static TestImageF ToTestImageF(const ExrRgbaImageF& src)
{
	TestImageF out;
	out.width = src.width;
	out.height = src.height;
	if(src.width <= 0 || src.height <= 0)
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
		out.pixels[i].a = src.pixels[i].a;
	}
	return out;
}

static bool IsValidImage(const TestImageF& img);

static TiffRgbaImage8 ToTiffRgbaImage8(const TestImage8& src)
{
	TiffRgbaImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
	return out;
}

static TiffRgbaImage16 ToTiffRgbaImage16(const TestImage16& src)
{
	TiffRgbaImage16 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
	return out;
}

static TiffRgbaImageF ToTiffRgbaImageF(const TestImageF& src)
{
	TiffRgbaImageF out;
	out.width = src.width;
	out.height = src.height;
	if(!IsValidImage(src))
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
	return out;
}

static TestImage16 ToTestImage16(const TiffRgbaImage16& src)
{
	TestImage16 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b, src.pixels[i].a};
	return out;
}

static TestImage8 ToTestImage8(const TiffRgbaImage8& src)
{
	TestImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
		out.pixels[i].a = src.pixels[i].a;
	}
	return out;
}

static TestImageF ToTestImageF(const TiffRgbaImageF& src)
{
	TestImageF out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
		out.pixels[i].a = src.pixels[i].a;
	}
	return out;
}

static PngRgbaImage8 ToPngRgbaImage8(const TestImage8& src)
{
	PngRgbaImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
		out.pixels[i].a = src.pixels[i].a;
	}
	return out;
}

static byte ClampRgbByte(float v)
{
	v = v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
	return (byte)(v * 255.0f + 0.5f);
}

static TestRgbImage8 QuantizeToRgb8(const TestImageF& src)
{
	TestRgbImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(src.width <= 0 || src.height <= 0 || src.pixels.GetCount() != (int64)src.width * src.height)
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = ClampRgbByte(src.pixels[i].r);
		out.pixels[i].g = ClampRgbByte(src.pixels[i].g);
		out.pixels[i].b = ClampRgbByte(src.pixels[i].b);
	}
	return out;
}

static JpegRgbImage8 ToJpegRgbImage8(const TestRgbImage8& src)
{
	JpegRgbImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b};
	return out;
}

static TestRgbImage8 ToTestRgbImage8(const JpegRgbImage8& src)
{
	TestRgbImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		out.pixels[i] = {src.pixels[i].r, src.pixels[i].g, src.pixels[i].b};
	return out;
}

static TestImageF NormalizeToFloat(const TestRgbImage8& src)
{
	TestImageF out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		const TestRgb8& p = src.pixels[i];
		out.pixels[i].r = (float)p.r / 255.0f;
		out.pixels[i].g = (float)p.g / 255.0f;
		out.pixels[i].b = (float)p.b / 255.0f;
		out.pixels[i].a = 1.0f;
	}
	return out;
}

static TestImage8 ToTestImage8(const PngRgbaImage8& src)
{
	TestImage8 out;
	out.width = src.width;
	out.height = src.height;
	if(!src.IsValid())
		return out;
	out.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i) {
		out.pixels[i].r = src.pixels[i].r;
		out.pixels[i].g = src.pixels[i].g;
		out.pixels[i].b = src.pixels[i].b;
		out.pixels[i].a = src.pixels[i].a;
	}
	return out;
}

static bool IsValidImage(const TestImageF& img)
{
	return img.width > 0 && img.height > 0 && img.pixels.GetCount() >= (int64)img.width * img.height;
}

static float DiffMax(const TestRgbaF& a, const TestRgbaF& b)
{
	return max(max(fabsf(a.r - b.r), fabsf(a.g - b.g)), max(fabsf(a.b - b.b), fabsf(a.a - b.a)));
}

PreviewPane::PreviewPane()
{
	SetDirection(UiDirection::V);
	SetGap(DPI(6));
	SetInset(DPI(8));
	Add(title_).Fit().AlignSelf(UiBoxLayout::Align::Start);
	Add(canvas_).Expand(1).AlignSelf(UiBoxLayout::Align::Stretch);
	title_.SetText("Pane");
	canvas_.NoWantFocus().IgnoreMouse();
}

PreviewPane& PreviewPane::SetPaneTitle(const String& text)
{
	title_.SetText(text);
	return *this;
}

PreviewPane& PreviewPane::SetPaneImage(const Image& img)
{
	canvas_.SetImage(img);
	return *this;
}

void PreviewPane::PreviewCanvas::SetImage(const Image& img)
{
	image_ = img;
	Refresh();
}

Size PreviewPane::PreviewCanvas::GetMinSize() const
{
	return Size(DPI(180), DPI(140));
}

void PreviewPane::PreviewCanvas::Paint(Draw& w)
{
	Rect r = GetSize();
	w.DrawRect(r, Blend(SColorPaper(), SColorFace(), 240));
	w.DrawRect(r.Deflated(1), SColorPaper());
	if(IsNull(image_))
		return;
	Rect inner = r.Deflated(DPI(10));
	Size fit = GetFitSize(image_.GetSize(), inner.GetSize());
	if(fit.cx <= 0 || fit.cy <= 0)
		return;
	Rect dst = RectC(inner.CenterPoint().x - fit.cx / 2, inner.CenterPoint().y - fit.cy / 2, fit.cx, fit.cy);
	w.DrawImage(dst, image_);
}

const RoundtripViewerWindow::ProfileSpec& RoundtripViewerWindow::GetProfile(ProfileKind kind)
{
	static const ProfileSpec profiles[] = {
		{"EXR HALF + ZIP", FORMAT_EXR, 256, 192, false, true, true, {}, {}, TIFF_RGBA8_DEFLATE, false, 0.0, 0.0, 0.0},
		{"EXR FLOAT + NONE", FORMAT_EXR, 256, 192, true, false, false, {}, {}, TIFF_RGBA8_DEFLATE, false, 0.0, 0.0, 0.0},
		{"PNG RGBA8", FORMAT_PNG, 256, 192, false, false, false, {}, {}, TIFF_RGBA8_DEFLATE, false, 0.0, 0.0, 0.0},
		{"JPEG RGB8 95 4:4:4", FORMAT_JPEG, 256, 192, false, false, false, {95, JpegSubsampling::S444, false, true}, {}, TIFF_RGBA8_DEFLATE, true, 3.5, 6.5, 32.0},
		{"TIFF RGBA8 + Deflate", FORMAT_TIFF, 256, 192, false, false, false, {}, {TiffCompression::Deflate}, TIFF_RGBA8_DEFLATE, false, 0.0, 0.0, 0.0},
		{"TIFF RGBA16 + Deflate", FORMAT_TIFF, 257, 193, false, false, false, {}, {TiffCompression::Deflate}, TIFF_RGBA16_DEFLATE, false, 0.0, 0.0, 0.0},
		{"TIFF RGBA Float32 + NONE", FORMAT_TIFF, 255, 191, true, false, false, {}, {TiffCompression::None}, TIFF_RGBAF_NONE, false, 0.0, 0.0, 0.0},
	};
	return profiles[(int)kind < 0 ? 0 : (int)kind > 6 ? 6 : (int)kind];
}

bool RoundtripViewerWindow::IsLossyPass(const LossyRgbComparison& cmp, const ProfileSpec& spec)
{
	return cmp.dimensions_match
		&& cmp.mean_absolute_error <= spec.max_mae
		&& cmp.rmse <= spec.max_rmse
		&& cmp.psnr >= spec.min_psnr;
}

bool RoundtripViewerWindow::IsExactPass(const RoundtripComparison& cmp)
{
	return cmp.dimensions_match && cmp.different_components == 0 && cmp.max_error_r == 0.0 && cmp.max_error_g == 0.0 && cmp.max_error_b == 0.0 && cmp.max_error_a == 0.0 && cmp.mean_absolute_error == 0.0 && cmp.rmse == 0.0;
}

bool RoundtripViewerWindow::IsExactPass(const RoundtripComparison16& cmp)
{
	return cmp.dimensions_match && cmp.different_components == 0 && cmp.max_error_r == 0 && cmp.max_error_g == 0 && cmp.max_error_b == 0 && cmp.max_error_a == 0;
}

int RoundtripViewerWindow::GetGainValue(const Value& v)
{
	int gain = (int)v;
	return gain > 0 ? gain : 1;
}

void RoundtripViewerWindow::SetRunState(int state, const String& message)
{
	run_state_ = state;
	run_message_ = message;
	UpdateStatus();
}

static bool ContainsString(const Vector<String>& values, const String& value)
{
	for(const String& item : values) {
		if(item == value)
			return true;
	}
	return false;
}

static void CopyTestImage(const TestImageF& src, TestImageF& dst)
{
	dst.width = src.width;
	dst.height = src.height;
	dst.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		dst.pixels[i] = src.pixels[i];
}

bool RoundtripViewerWindow::IsOcioEnabled() const
{
	return (int)ocio_enable_drop_.GetSelectedData() != 0;
}

String RoundtripViewerWindow::GetDropdownValue(const UiDropdown& drop) const
{
	const int selection = drop.GetSelection();
	if(selection < 0)
		return String();
	return AsString(drop.GetSelectedData());
}

void RoundtripViewerWindow::SetDropdownValues(UiDropdown& drop, const Vector<String>& values, const String& selected_value)
{
	drop.Clear();
	if(values.IsEmpty()) {
		drop.Add("none", String());
		drop.Select(0);
		return;
	}
	int selected_index = 0;
	for(int i = 0; i < values.GetCount(); ++i) {
		drop.Add(values[i], values[i]);
		if(values[i] == selected_value)
			selected_index = i;
	}
	drop.Select(selected_index);
}

String RoundtripViewerWindow::GetOcioSummary() const
{
	if(!IsOcioEnabled())
		return "OCIO: off";
	const String config_name = GetDropdownValue(ocio_config_drop_);
	if(config_name.IsEmpty())
		return "OCIO: on / none";
	return OcioPreview::DescribeSelection(config_name, GetDropdownValue(ocio_source_drop_), GetDropdownValue(ocio_display_drop_), GetDropdownValue(ocio_view_drop_));
}

String RoundtripViewerWindow::GetOcioErrorText() const
{
	return ocio_error_;
}

void RoundtripViewerWindow::UpdateOcioSelections()
{
	if(!IsOcioEnabled() || !ocio_config_) {
		SetDropdownValues(ocio_source_drop_, Vector<String>(), String());
		SetDropdownValues(ocio_display_drop_, Vector<String>(), String());
		SetDropdownValues(ocio_view_drop_, Vector<String>(), String());
		return;
	}

	Vector<String> source_names = OcioPreview::GetColorSpaceNames(ocio_config_);
	String source = GetDropdownValue(ocio_source_drop_);
	if(!ContainsString(source_names, source))
		source = OcioPreview::GetDefaultSourceColorSpace(ocio_config_);
	SetDropdownValues(ocio_source_drop_, source_names, source);
	if(!ContainsString(source_names, GetDropdownValue(ocio_source_drop_)) && !source_names.IsEmpty())
		SetDropdownValues(ocio_source_drop_, source_names, source_names[0]);

	Vector<String> display_names = OcioPreview::GetDisplayNames(ocio_config_);
	String display = GetDropdownValue(ocio_display_drop_);
	if(!ContainsString(display_names, display))
		display = OcioPreview::GetDefaultDisplay(ocio_config_);
	SetDropdownValues(ocio_display_drop_, display_names, display);
	if(display.IsEmpty() && !display_names.IsEmpty())
		display = display_names[0];
	else
		display = GetDropdownValue(ocio_display_drop_);

	Vector<String> view_names = OcioPreview::GetViewNames(ocio_config_, display);
	String view = GetDropdownValue(ocio_view_drop_);
	if(!ContainsString(view_names, view))
		view = OcioPreview::GetDefaultView(ocio_config_, display);
	SetDropdownValues(ocio_view_drop_, view_names, view);

	if(GetDropdownValue(ocio_display_drop_).IsEmpty() && !display_names.IsEmpty())
		SetDropdownValues(ocio_display_drop_, display_names, display_names[0]);
	if(GetDropdownValue(ocio_view_drop_).IsEmpty() && !view_names.IsEmpty())
		SetDropdownValues(ocio_view_drop_, view_names, view_names[0]);
}

void RoundtripViewerWindow::UpdateOcioConfig()
{
	if(ocio_controls_updating_)
		return;
	ocio_controls_updating_ = true;
	ocio_error_.Clear();
	if(IsOcioEnabled()) {
		const String config_name = GetDropdownValue(ocio_config_drop_);
		if(config_name.IsEmpty())
			ocio_config_.reset();
		else if(!OcioPreview::LoadBuiltinConfig(config_name, ocio_config_, ocio_error_))
			ocio_config_.reset();
	}
	else {
		ocio_config_.reset();
	}
	UpdateOcioSelections();
	ocio_controls_updating_ = false;
	UpdateDetails();
	RefreshViews();
}

void RoundtripViewerWindow::SyncOcioDisplayViews()
{
	if(ocio_controls_updating_)
		return;
	ocio_controls_updating_ = true;
	UpdateOcioSelections();
	ocio_controls_updating_ = false;
	UpdateDetails();
	RefreshViews();
}

RoundtripViewerWindow::RoundtripViewerWindow()
{
	Title("Imaging round-trip viewer");
	Sizeable().Zoomable();
	SetRect(0, 0, DPI(1600), DPI(1020));
	BackPaint();

	Add(root_);
	root_.SizePos();
	root_.SetGap(DPI(10));
	root_.SetInset(DPI(10));

	root_.Add(top_row_).Fit().AlignSelf(UiBoxLayout::Align::Stretch);
	root_.Add(ocio_row_).Fit().AlignSelf(UiBoxLayout::Align::Stretch);
	root_.Add(panes_row_).Expand(1).AlignSelf(UiBoxLayout::Align::Stretch);
	root_.Add(bottom_col_).Fit().AlignSelf(UiBoxLayout::Align::Stretch);

	top_row_.SetGap(DPI(8));
	top_row_.SetAlignItems(UiBoxLayout::Align::Center);
	ocio_row_.SetGap(DPI(6));
	ocio_row_.SetAlignItems(UiBoxLayout::Align::Center);
	panes_row_.SetGap(DPI(10));
	bottom_col_.SetGap(DPI(4));

	profile_label_.SetText("Profile").NoWantFocus().IgnoreMouse();
	display_label_.SetText("Display").NoWantFocus().IgnoreMouse();
	gain_label_.SetText("Gain").NoWantFocus().IgnoreMouse();
	ocio_label_.SetText("OCIO").NoWantFocus().IgnoreMouse();
	ocio_config_label_.SetText("Config").NoWantFocus().IgnoreMouse();
	ocio_source_label_.SetText("Source").NoWantFocus().IgnoreMouse();
	ocio_display_label_.SetText("Display").NoWantFocus().IgnoreMouse();
	ocio_view_label_.SetText("View").NoWantFocus().IgnoreMouse();
	run_button_.SetText("Run Round-trip");

	profile_drop_.Add("EXR HALF + ZIP", (int)PROFILE_EXR_HALF_ZIP);
	profile_drop_.Add("EXR FLOAT + NONE", (int)PROFILE_EXR_FLOAT_NONE);
	profile_drop_.Add("PNG RGBA8", (int)PROFILE_PNG_RGBA8);
	profile_drop_.Add("JPEG RGB8 95 4:4:4", (int)PROFILE_JPEG_RGB95_444);
	profile_drop_.Add("TIFF RGBA8 + Deflate", (int)PROFILE_TIFF_RGBA8_DEFLATE);
	profile_drop_.Add("TIFF RGBA16 + Deflate", (int)PROFILE_TIFF_RGBA16_DEFLATE);
	profile_drop_.Add("TIFF RGBA Float32 + NONE", (int)PROFILE_TIFF_RGBAF_NONE);
	profile_drop_.Select(0);
	display_drop_.Add("RGB", DISPLAY_RGB);
	display_drop_.Add("Raw RGB", DISPLAY_RAW_RGB);
	display_drop_.Add("Alpha", DISPLAY_ALPHA);
	display_drop_.Select(0);
	gain_drop_.Add("1x", 1);
	gain_drop_.Add("16x", 16);
	gain_drop_.Add("64x", 64);
	gain_drop_.Add("256x", 256);
	gain_drop_.Select(0);
	ocio_enable_drop_.Add("Off", 0);
	ocio_enable_drop_.Add("On", 1);
	ocio_enable_drop_.Select(0);
	ocio_config_drop_.Add("Off / none", String());
	for(const String& name : OcioPreview::GetBuiltinConfigNames())
		ocio_config_drop_.Add(name, name);
	ocio_config_drop_.Select(0);
	ocio_source_drop_.Add("none", String());
	ocio_source_drop_.Select(0);
	ocio_display_drop_.Add("none", String());
	ocio_display_drop_.Select(0);
	ocio_view_drop_.Add("none", String());
	ocio_view_drop_.Select(0);

	top_row_.Add(profile_label_).Fit();
	top_row_.Add(profile_drop_).Fixed(DPI(160));
	top_row_.Add(display_label_).Fit();
	top_row_.Add(display_drop_).Fixed(DPI(150));
	top_row_.Add(gain_label_).Fit();
	top_row_.Add(gain_drop_).Fixed(DPI(110));
	top_row_.Add(run_button_).Fixed(DPI(142));
	top_row_.Add(status_label_).Expand(1).AlignSelf(UiBoxLayout::Align::Center);

	ocio_row_.Add(ocio_label_).Fit();
	ocio_row_.Add(ocio_enable_drop_).Fixed(DPI(74));
	ocio_row_.Add(ocio_config_label_).Fit();
	ocio_row_.Add(ocio_config_drop_).Fixed(DPI(280));
	ocio_row_.Add(ocio_source_label_).Fit();
	ocio_row_.Add(ocio_source_drop_).Fixed(DPI(160));
	ocio_row_.Add(ocio_display_label_).Fit();
	ocio_row_.Add(ocio_display_drop_).Fixed(DPI(150));
	ocio_row_.Add(ocio_view_label_).Fit();
	ocio_row_.Add(ocio_view_drop_).Fixed(DPI(150));

	generated_pane_.SetPaneTitle("Generated");
	reloaded_pane_.SetPaneTitle("Reloaded");
	difference_pane_.SetPaneTitle("Difference (RGB/A)");
	panes_row_.Add(generated_pane_).Expand(1).AlignSelf(UiBoxLayout::Align::Stretch);
	panes_row_.Add(reloaded_pane_).Expand(1).AlignSelf(UiBoxLayout::Align::Stretch);
	panes_row_.Add(difference_pane_).Expand(1).AlignSelf(UiBoxLayout::Align::Stretch);

	bottom_col_.Add(details_label_).Fit().AlignSelf(UiBoxLayout::Align::Stretch);
	bottom_col_.Add(path_row_).Fit().AlignSelf(UiBoxLayout::Align::Stretch);
	bottom_col_.Add(error_row_).Fit().AlignSelf(UiBoxLayout::Align::Stretch);
	path_row_.Add(path_label_).Fit().AlignSelf(UiBoxLayout::Align::Stretch);
	error_row_.Add(error_label_).Fit().AlignSelf(UiBoxLayout::Align::Stretch);

	profile_drop_.WhenSelectData = [this](const Value&) { RunSelected(); };
	display_drop_.WhenSelectData = [this](const Value&) { RefreshViews(); };
	gain_drop_.WhenSelectData = [this](const Value&) { RefreshViews(); };
	ocio_enable_drop_.WhenSelectData = [this](const Value&) { UpdateOcioConfig(); };
	ocio_config_drop_.WhenSelectData = [this](const Value&) { UpdateOcioConfig(); };
	ocio_source_drop_.WhenSelectData = [this](const Value&) { RefreshViews(); UpdateDetails(); };
	ocio_display_drop_.WhenSelectData = [this](const Value&) { SyncOcioDisplayViews(); };
	ocio_view_drop_.WhenSelectData = [this](const Value&) { RefreshViews(); UpdateDetails(); };
	run_button_.WhenAction = [this] { RunSelected(); };

	output_path_ = GetExeDirFile("roundtrip_viewer.exr");
	UpdateOcioConfig();
	UpdateStatus();
	UpdateDetails();
	RefreshViews();
	SetRunState(RUN_NOT_RUN);
	SetTimeCallback(150, [this] { RunSelected(); });
}

void RoundtripViewerWindow::RunSelected()
{
	RunProfile((ProfileKind)(int)profile_drop_.GetSelectedData());
}

void RoundtripViewerWindow::RunProfile(ProfileKind kind)
{
	SetRunState(RUN_RUNNING);
	io_error_.Clear();
	const ProfileSpec& spec = GetProfile(kind);
	generated_ = TestImageF();
	reloaded_ = TestImageF();
	comparison_ = RoundtripComparison();
	comparison16_ = RoundtripComparison16();
	output_size_ = -1;
	output_path_ = spec.format == FORMAT_PNG ? GetExeDirFile("roundtrip_viewer.png") : spec.format == FORMAT_JPEG ? GetExeDirFile("roundtrip_viewer.jpg") : spec.format == FORMAT_TIFF ? GetExeDirFile("roundtrip_viewer.tif") : GetExeDirFile("roundtrip_viewer.exr");
	rgb_comparison_ = LossyRgbComparison();

	if(spec.format == FORMAT_EXR) {
		generated_ = GenerateRoundtripTestPattern(spec.width, spec.height, spec.include_hdr);
		ExrRgbaImageF save_image = ToExrRgbaImageF(generated_);
		if(!SaveExrRgbaF(~output_path_, save_image, spec.output_half, spec.use_zip, &io_error_)) {
			SetRunState(RUN_FAIL, io_error_);
			UpdateStatus();
			UpdateDetails();
			RefreshViews();
			return;
		}
		FileIn in(~output_path_);
		if(in.IsOpen())
			output_size_ = (long long)in.GetSize();

		ExrRgbaImageF loaded_image;
		if(!LoadExrRgbaF(~output_path_, loaded_image, &io_error_)) {
			SetRunState(RUN_FAIL, io_error_);
			UpdateStatus();
			UpdateDetails();
			RefreshViews();
			return;
		}
		reloaded_ = ToTestImageF(loaded_image);
		comparison_ = CompareExact(generated_, reloaded_);
	}
	else if(spec.format == FORMAT_JPEG) {
		TestImageF source = GenerateRoundtripTestPattern(spec.width, spec.height, false);
		TestRgbImage8 expected8 = QuantizeToRgb8(source);
		generated_ = NormalizeToFloat(expected8);
		JpegRgbImage8 save_image = ToJpegRgbImage8(expected8);
		if(!SaveJpegRgb8(~output_path_, save_image, spec.jpeg_options, &io_error_)) {
			SetRunState(RUN_FAIL, io_error_);
			UpdateStatus();
			UpdateDetails();
			RefreshViews();
			return;
		}
		FileIn in(~output_path_);
		if(in.IsOpen())
			output_size_ = (long long)in.GetSize();

		JpegRgbImage8 loaded_image;
		if(!LoadJpegRgb8(~output_path_, loaded_image, &io_error_)) {
			SetRunState(RUN_FAIL, io_error_);
			UpdateStatus();
			UpdateDetails();
			RefreshViews();
			return;
		}
		TestRgbImage8 actual8 = ToTestRgbImage8(loaded_image);
		reloaded_ = NormalizeToFloat(actual8);
		rgb_comparison_ = CompareLossyRgb8(expected8, actual8);
	}
	else if(spec.format == FORMAT_TIFF) {
		RunTiffProfile(spec);
		return;
	}
	else {
		TestImageF source = GenerateRoundtripTestPattern(spec.width, spec.height, false);
		TestImage8 expected8 = QuantizeToRgba8(source);
		generated_ = NormalizeToFloat(expected8);
		PngRgbaImage8 save_image = ToPngRgbaImage8(expected8);
		if(!SavePngRgba8(~output_path_, save_image, &io_error_)) {
			SetRunState(RUN_FAIL, io_error_);
			UpdateStatus();
			UpdateDetails();
			RefreshViews();
			return;
		}
		FileIn in(~output_path_);
		if(in.IsOpen())
			output_size_ = (long long)in.GetSize();

		PngRgbaImage8 loaded_image;
		if(!LoadPngRgba8(~output_path_, loaded_image, &io_error_)) {
			SetRunState(RUN_FAIL, io_error_);
			UpdateStatus();
			UpdateDetails();
			RefreshViews();
			return;
		}
		TestImage8 actual8 = ToTestImage8(loaded_image);
		reloaded_ = NormalizeToFloat(actual8);
		comparison_ = CompareExact(generated_, reloaded_);
	}
	io_error_.Clear();
	if(spec.lossy) {
		if(IsLossyPass(rgb_comparison_, spec))
			SetRunState(RUN_PASS, "lossy round-trip within thresholds");
		else
			SetRunState(RUN_FAIL, rgb_comparison_.summary);
	}
	else if(IsExactPass(comparison_))
		SetRunState(RUN_PASS);
	else
		SetRunState(RUN_FAIL, comparison_.summary);
	UpdateStatus();
	UpdateDetails();
	RefreshViews();
}

void RoundtripViewerWindow::RunTiffProfile(const ProfileSpec& spec)
{
	auto fail = [this](const String& message) {
		SetRunState(RUN_FAIL, message);
		UpdateStatus();
		UpdateDetails();
		RefreshViews();
	};
	auto pass = [this](const String& message = String()) {
		SetRunState(RUN_PASS, message);
		UpdateStatus();
		UpdateDetails();
		RefreshViews();
	};

	if(spec.tiff_kind == TIFF_RGBA8_DEFLATE) {
		TestImageF source = GenerateRoundtripTestPattern(spec.width, spec.height, false);
		TestImage8 expected8 = QuantizeToRgba8(source);
		generated_ = NormalizeToFloat(expected8);
		TiffRgbaImage8 save_image = ToTiffRgbaImage8(expected8);
		if(!SaveTiffRgba8(~output_path_, save_image, spec.tiff_options, &io_error_)) {
			fail(io_error_);
			return;
		}
		FileIn in(~output_path_);
		if(in.IsOpen())
			output_size_ = (long long)in.GetSize();
		TiffRgbaImage8 loaded_image;
		if(!LoadTiffRgba8(~output_path_, loaded_image, &io_error_)) {
			fail(io_error_);
			return;
		}
		TestImage8 actual8 = ToTestImage8(loaded_image);
		reloaded_ = NormalizeToFloat(actual8);
		comparison_ = CompareExact(generated_, reloaded_);
		if(IsExactPass(comparison_))
			pass();
		else
			fail(comparison_.summary);
		return;
	}

	if(spec.tiff_kind == TIFF_RGBA16_DEFLATE) {
		TestImageF source = GenerateRoundtripTestPattern(spec.width, spec.height, false);
		TestImage16 expected16 = QuantizeToRgba16(source);
		generated_ = NormalizeToFloat(expected16);
		TiffRgbaImage16 save_image = ToTiffRgbaImage16(expected16);
		if(!SaveTiffRgba16(~output_path_, save_image, spec.tiff_options, &io_error_)) {
			fail(io_error_);
			return;
		}
		FileIn in(~output_path_);
		if(in.IsOpen())
			output_size_ = (long long)in.GetSize();
		TiffRgbaImage16 loaded_image;
		if(!LoadTiffRgba16(~output_path_, loaded_image, &io_error_)) {
			fail(io_error_);
			return;
		}
		TestImage16 actual16 = ToTestImage16(loaded_image);
		reloaded_ = NormalizeToFloat(actual16);
		comparison16_ = CompareExact(expected16, actual16);
		if(IsExactPass(comparison16_))
			pass();
		else
			fail(comparison16_.summary);
		return;
	}

	if(spec.tiff_kind == TIFF_RGBAF_NONE) {
		TestImageF source = GenerateRoundtripTestPattern(spec.width, spec.height, true);
		generated_.width = source.width;
		generated_.height = source.height;
		generated_.pixels.SetCount(source.pixels.GetCount());
		for(int i = 0; i < source.pixels.GetCount(); ++i)
			generated_.pixels[i] = source.pixels[i];
		TiffRgbaImageF save_image = ToTiffRgbaImageF(source);
		if(!SaveTiffRgbaF(~output_path_, save_image, spec.tiff_options, &io_error_)) {
			fail(io_error_);
			return;
		}
		FileIn in(~output_path_);
		if(in.IsOpen())
			output_size_ = (long long)in.GetSize();
		TiffRgbaImageF loaded_image;
		if(!LoadTiffRgbaF(~output_path_, loaded_image, &io_error_)) {
			fail(io_error_);
			return;
		}
		reloaded_ = ToTestImageF(loaded_image);
		comparison_ = CompareExact(generated_, reloaded_);
		if(IsExactPass(comparison_))
			pass();
		else
			fail(comparison_.summary);
		return;
	}

	fail("unsupported TIFF profile");
}

void RoundtripViewerWindow::RefreshViews()
{
	DisplayKind kind = (DisplayKind)(int)display_drop_.GetSelectedData();
	TestImageF preview_generated;
	TestImageF preview_reloaded;
	CopyTestImage(generated_, preview_generated);
	CopyTestImage(reloaded_, preview_reloaded);
	if(IsOcioEnabled() && ocio_config_) {
		const String source = GetDropdownValue(ocio_source_drop_);
		const String display = GetDropdownValue(ocio_display_drop_);
		const String view = GetDropdownValue(ocio_view_drop_);
		if(!source.IsEmpty() && !display.IsEmpty() && !view.IsEmpty()) {
			String preview_error;
			if(!OcioPreview::ApplyPreview(ocio_config_, source, display, view, generated_, preview_generated, preview_error) && ocio_error_.IsEmpty())
				ocio_error_ = preview_error;
			preview_error.Clear();
			if(!OcioPreview::ApplyPreview(ocio_config_, source, display, view, reloaded_, preview_reloaded, preview_error) && ocio_error_.IsEmpty())
				ocio_error_ = preview_error;
		}
	}
	generated_pane_.SetPaneImage(MakeDisplayImage(preview_generated, kind));
	reloaded_pane_.SetPaneImage(MakeDisplayImage(preview_reloaded, kind));
	if(IsValidImage(generated_) && IsValidImage(reloaded_) && generated_.width == reloaded_.width && generated_.height == reloaded_.height)
		difference_pane_.SetPaneImage(MakeDifferenceImage(generated_, reloaded_, GetGainValue(gain_drop_.GetSelectedData())));
	else
		difference_pane_.SetPaneImage(Image());
}

void RoundtripViewerWindow::UpdateStatus()
{
	switch(run_state_) {
	case RUN_NOT_RUN:
		status_label_.SetText("Not run");
		return;
	case RUN_RUNNING:
		status_label_.SetText("Running…");
		return;
	case RUN_PASS:
		status_label_.SetText(IsNull(run_message_) ? "PASS — exact round-trip" : Format("PASS — %s", ~run_message_));
		return;
	case RUN_FAIL:
		if(!IsNull(run_message_))
			status_label_.SetText(Format("FAIL — %s", ~run_message_));
		else if(GetProfile((ProfileKind)(int)profile_drop_.GetSelectedData()).format == FORMAT_TIFF && GetProfile((ProfileKind)(int)profile_drop_.GetSelectedData()).tiff_kind == TIFF_RGBA16_DEFLATE)
			status_label_.SetText(Format("FAIL — %s", ~comparison16_.summary));
		else
			status_label_.SetText(Format("FAIL — %s", ~comparison_.summary));
		return;
	default:
		status_label_.SetText("Not run");
		return;
	}
}

void RoundtripViewerWindow::UpdateDetails()
{
	String size_text = output_size_ >= 0 ? Format("%lld bytes", output_size_) : String("n/a");
	const ProfileSpec& spec = GetProfile((ProfileKind)(int)profile_drop_.GetSelectedData());
	const String ocio_summary = GetOcioSummary();
	if(spec.lossy) {
		details_label_.SetText(Format(
			"%s\nProfile: %s\nDimensions: %dx%d\nFile size: %s\nMax R error: %d\nMax G error: %d\nMax B error: %d\nMean absolute error: %.9g\nRMSE: %.9g\nPSNR: %.9g dB",
			ocio_summary,
			AsString(profile_drop_.GetItemText(profile_drop_.GetSelection())),
			generated_.width,
			generated_.height,
			size_text,
			rgb_comparison_.max_error_r,
			rgb_comparison_.max_error_g,
			rgb_comparison_.max_error_b,
			rgb_comparison_.mean_absolute_error,
			rgb_comparison_.rmse,
			rgb_comparison_.psnr));
	}
	else if(spec.format == FORMAT_TIFF && spec.tiff_kind == TIFF_RGBA16_DEFLATE) {
		details_label_.SetText(Format(
			"%s\nProfile: %s\nDimensions: %dx%d\nFile size: %s\nDifferent components: %d\nMax R error: %d\nMax G error: %d\nMax B error: %d\nMax A error: %d",
			ocio_summary,
			AsString(profile_drop_.GetItemText(profile_drop_.GetSelection())),
			generated_.width,
			generated_.height,
			size_text,
			comparison16_.different_components,
			comparison16_.max_error_r,
			comparison16_.max_error_g,
			comparison16_.max_error_b,
			comparison16_.max_error_a));
	}
	else {
		details_label_.SetText(Format(
			"%s\nProfile: %s\nDimensions: %dx%d\nFile size: %s\nDifferent components: %d\nMax R error: %.9g\nMax G error: %.9g\nMax B error: %.9g\nMax A error: %.9g\nMean absolute error: %.9g\nRMSE: %.9g",
			ocio_summary,
			AsString(profile_drop_.GetItemText(profile_drop_.GetSelection())),
			generated_.width,
			generated_.height,
			size_text,
			comparison_.different_components,
			comparison_.max_error_r,
			comparison_.max_error_g,
			comparison_.max_error_b,
			comparison_.max_error_a,
			comparison_.mean_absolute_error,
			comparison_.rmse));
	}
	path_label_.SetText(Format("Output path: %s", ~output_path_));
	String error_text = IsNull(io_error_) ? String("Load/save error: none") : Format("Load/save error: %s", ~io_error_);
	if(IsNull(ocio_error_))
		error_text << " | OCIO preview error: none";
	else
		error_text << " | OCIO preview error: " << ocio_error_;
	error_label_.SetText(error_text);
}

byte RoundtripViewerWindow::ClampByte(float v)
{
	v = Clamp01(v);
	return (byte)(v * 255.0f + 0.5f);
}

float RoundtripViewerWindow::Clamp01(float v)
{
	return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

byte RoundtripViewerWindow::BlendByte(byte fg, byte bg, float alpha)
{
	return (byte)(Clamp01(alpha) * fg + (1.0f - Clamp01(alpha)) * bg + 0.5f);
}

Image RoundtripViewerWindow::MakeDisplayImage(const TestImageF& img, DisplayKind kind) const
{
	if(!IsValidImage(img))
		return Image();
	ImageBuffer buffer(Size(img.width, img.height));
	RGBA* out = ~buffer;
	for(int y = 0; y < img.height; ++y) {
		for(int x = 0; x < img.width; ++x, ++out) {
			const TestRgbaF& p = img.pixels[y * img.width + x];
			if(kind == DISPLAY_ALPHA) {
				byte v = ClampByte(p.a);
				out->r = out->g = out->b = v;
				out->a = 255;
			}
			else if(kind == DISPLAY_RAW_RGB) {
				out->r = ClampByte(p.r);
				out->g = ClampByte(p.g);
				out->b = ClampByte(p.b);
				out->a = 255;
			}
			else {
				byte bg = ((x >> 4) ^ (y >> 4)) & 1 ? 214 : 238;
				out->r = BlendByte(ClampByte(p.r), bg, p.a);
				out->g = BlendByte(ClampByte(p.g), bg, p.a);
				out->b = BlendByte(ClampByte(p.b), bg, p.a);
				out->a = 255;
			}
		}
	}
	return buffer;
}

Image RoundtripViewerWindow::MakeDifferenceImage(const TestImageF& a, const TestImageF& b, int gain) const
{
	if(!IsValidImage(a) || !IsValidImage(b) || a.width != b.width || a.height != b.height)
		return Image();
	ImageBuffer buffer(Size(a.width, a.height));
	RGBA* out = ~buffer;
	for(int y = 0; y < a.height; ++y) {
		for(int x = 0; x < a.width; ++x, ++out) {
			const TestRgbaF& p = a.pixels[y * a.width + x];
			const TestRgbaF& q = b.pixels[y * b.width + x];
			float v = DiffMax(p, q) * (float)gain;
			byte c = ClampByte(v);
			out->r = out->g = out->b = c;
			out->a = 255;
		}
	}
	return buffer;
}

} // namespace Upp
