#include "RoundtripViewer.h"

#include <math.h>

#include <openexr_io/OpenExrIO.h>
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
		{"EXR HALF + ZIP", FORMAT_EXR, 256, 192, false, true, true},
		{"EXR FLOAT + NONE", FORMAT_EXR, 256, 192, true, false, false},
		{"PNG RGBA8", FORMAT_PNG, 256, 192, false, false, false},
	};
	return profiles[(int)kind < 0 ? 0 : (int)kind > 2 ? 2 : (int)kind];
}

bool RoundtripViewerWindow::IsExactPass(const RoundtripComparison& cmp)
{
	return cmp.dimensions_match && cmp.different_components == 0 && cmp.max_error_r == 0.0 && cmp.max_error_g == 0.0 && cmp.max_error_b == 0.0 && cmp.max_error_a == 0.0 && cmp.mean_absolute_error == 0.0 && cmp.rmse == 0.0;
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
	root_.Add(panes_row_).Expand(1).AlignSelf(UiBoxLayout::Align::Stretch);
	root_.Add(bottom_col_).Fit().AlignSelf(UiBoxLayout::Align::Stretch);

	top_row_.SetGap(DPI(8));
	top_row_.SetAlignItems(UiBoxLayout::Align::Center);
	panes_row_.SetGap(DPI(10));
	bottom_col_.SetGap(DPI(4));

	profile_label_.SetText("Profile").NoWantFocus().IgnoreMouse();
	display_label_.SetText("Display").NoWantFocus().IgnoreMouse();
	gain_label_.SetText("Gain").NoWantFocus().IgnoreMouse();
	run_button_.SetText("Run Round-trip");

	profile_drop_.Add("EXR HALF + ZIP", (int)PROFILE_EXR_HALF_ZIP);
	profile_drop_.Add("EXR FLOAT + NONE", (int)PROFILE_EXR_FLOAT_NONE);
	profile_drop_.Add("PNG RGBA8", (int)PROFILE_PNG_RGBA8);
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

	top_row_.Add(profile_label_).Fit();
	top_row_.Add(profile_drop_).Fixed(DPI(160));
	top_row_.Add(display_label_).Fit();
	top_row_.Add(display_drop_).Fixed(DPI(150));
	top_row_.Add(gain_label_).Fit();
	top_row_.Add(gain_drop_).Fixed(DPI(110));
	top_row_.Add(run_button_).Fixed(DPI(142));
	top_row_.Add(status_label_).Expand(1).AlignSelf(UiBoxLayout::Align::Center);

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
	run_button_.WhenAction = [this] { RunSelected(); };

	output_path_ = GetExeDirFile("roundtrip_viewer.exr");
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
	output_size_ = -1;
	output_path_ = spec.format == FORMAT_PNG ? GetExeDirFile("roundtrip_viewer.png") : GetExeDirFile("roundtrip_viewer.exr");

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
	if(IsExactPass(comparison_))
		SetRunState(RUN_PASS);
	else
		SetRunState(RUN_FAIL, comparison_.summary);
	UpdateStatus();
	UpdateDetails();
	RefreshViews();
}

void RoundtripViewerWindow::RefreshViews()
{
	DisplayKind kind = (DisplayKind)(int)display_drop_.GetSelectedData();
	generated_pane_.SetPaneImage(MakeDisplayImage(generated_, kind));
	reloaded_pane_.SetPaneImage(MakeDisplayImage(reloaded_, kind));
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
		status_label_.SetText("PASS — exact round-trip");
		return;
	case RUN_FAIL:
		if(!IsNull(run_message_))
			status_label_.SetText(Format("FAIL — %s", ~run_message_));
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
	details_label_.SetText(Format(
		"Profile: %s\nDimensions: %dx%d\nFile size: %s\nDifferent components: %d\nMax R error: %.9g\nMax G error: %.9g\nMax B error: %.9g\nMax A error: %.9g\nMean absolute error: %.9g\nRMSE: %.9g",
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
	path_label_.SetText(Format("Output path: %s", ~output_path_));
	error_label_.SetText(IsNull(io_error_) ? "Load/save error: none" : Format("Load/save error: %s", ~io_error_));
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
