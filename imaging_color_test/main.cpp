#include <ImagingColor/ImagingColor.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

using namespace Upp;
using namespace Upp::Imaging;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool ok, const char* name)
{
	Cout() << (ok ? "PASS " : "FAIL ") << name << '\n';
	(ok ? state.passed : state.failed)++;
}

static bool Near(float a, float b, float epsilon = 2e-5f)
{
	return std::fabs(a - b) <= epsilon;
}

static bool HasDiagnostic(const Diagnostics& diagnostics, const char* code)
{
	for(const DiagnosticEntry& entry : diagnostics.Entries())
		if(entry.code == code)
			return true;
	return false;
}

static bool SameBytes(const ImageBuffer& a, const ImageBuffer& b)
{
	return a.GetByteCount() == b.GetByteCount() &&
	       (a.GetByteCount() == 0 ||
	        memcmp(a.Begin(), b.Begin(), a.GetByteCount()) == 0);
}

static bool SameSpec(const ImageSpec& a, const ImageSpec& b)
{
	return a.data_window == b.data_window && a.depth == b.depth &&
	       a.channels == b.channels && a.channel_layout == b.channel_layout &&
	       a.sample_type == b.sample_type && a.channel_names == b.channel_names &&
	       a.alpha_channel == b.alpha_channel;
}

static void SetSample(ImageData& image, int64 sample, float value)
{
	byte* target = image.buffer.Begin() + sample * BytesPerSample(image.spec.sample_type);
	switch(image.spec.sample_type) {
	case SampleType::UInt8: {
		byte v = (byte)std::floor(std::max(0.0f, std::min(1.0f, value)) * 255.0f + 0.5f);
		memcpy(target, &v, 1);
		break;
	}
	case SampleType::UInt16: {
		uint16_t v = (uint16_t)std::floor(std::max(0.0f, std::min(1.0f, value)) * 65535.0f + 0.5f);
		memcpy(target, &v, 2);
		break;
	}
	case SampleType::Float32:
		memcpy(target, &value, 4);
		break;
	default:
		break;
	}
}

static float GetSample(const ImageData& image, int64 sample)
{
	const byte* source = image.buffer.Begin() + sample * BytesPerSample(image.spec.sample_type);
	switch(image.spec.sample_type) {
	case SampleType::UInt8:
		return source[0] / 255.0f;
	case SampleType::UInt16: {
		uint16_t v;
		memcpy(&v, source, 2);
		return v / 65535.0f;
	}
	case SampleType::Float32: {
		float v;
		memcpy(&v, source, 4);
		return v;
	}
	default:
		return 0.0f;
	}
}

static ImageData MakeImage(SampleType type, ChannelLayout layout, int width = 1,
                           int height = 1)
{
	ImageData image;
	image.spec.data_window = {-2, 3, -2 + width - 1, 3 + height - 1};
	image.spec.depth = 1;
	image.spec.sample_type = type;
	image.spec.channel_layout = layout;
	switch(layout) {
	case ChannelLayout::RGB:
		image.spec.channels = 3;
		image.spec.alpha_channel = -1;
		image.spec.channel_names.Add("R");
		image.spec.channel_names.Add("G");
		image.spec.channel_names.Add("B");
		break;
	case ChannelLayout::RGBA:
		image.spec.channels = 4;
		image.spec.alpha_channel = 3;
		image.spec.channel_names.Add("R");
		image.spec.channel_names.Add("G");
		image.spec.channel_names.Add("B");
		image.spec.channel_names.Add("A");
		break;
	case ChannelLayout::Gray:
		image.spec.channels = 1;
		image.spec.alpha_channel = -1;
		image.spec.channel_names.Add("Y");
		break;
	default:
		break;
	}
	image.buffer.Allocate(image.spec);
	image.metadata.Set("test:label", "preserve-me");
	image.metadata.Set("test:number", 17);
	return image;
}

static String ConfigText()
{
	return
		"ocio_profile_version: 2\n"
		"environment: {}\n"
		"search_path:\n"
		"  - .\n"
		"roles:\n"
		"  default: linear\n"
		"  scene_linear: linear\n"
		"colorspaces:\n"
		"  - !<ColorSpace>\n"
		"    name: linear\n"
		"    isdata: false\n"
		"  - !<ColorSpace>\n"
		"    name: log\n"
		"    isdata: false\n"
		"    to_scene_reference: !<GroupTransform>\n"
		"      children:\n"
		"        - !<MatrixTransform> {matrix: [2., 0., 0., 0., 0., 0.5, 0., 0., 0., 0., 1.5, 0., 0., 0., 0., 1.], offset: [0.1, 0.2, -0.1, 0.]}\n"
		"        - !<ExponentTransform> {value: [2., 2., 2., 1.]}\n"
		"        - !<RangeTransform> {min_in_value: 0., min_out_value: 0., max_in_value: 1., max_out_value: 1.}\n"
		"displays:\n"
		"  sRGB:\n"
		"    - !<View> {name: Raw, colorspace: linear}\n"
		"active_displays: [sRGB]\n"
		"active_views: [Raw]\n";
}

static ColorConfigRef FileConfig(const String& path)
{
	ColorConfigRef ref;
	ref.source = ColorConfigSource::File;
	ref.value = path;
	return ref;
}

static ColorSpaceTransform LogToLinear(const String& path)
{
	ColorSpaceTransform transform;
	transform.config = FileConfig(path);
	transform.source = "log";
	transform.destination = "linear";
	return transform;
}

CONSOLE_APP_MAIN
{
	State state;
	String root = AppendFileName(GetTempPath(), "imaging_color_contract");
	RealizeDirectory(root);
	String config_path = AppendFileName(root, "contract.ocio");
	Check(state, SaveFile(config_path, ConfigText()), "config fixture creation");

	Diagnostics diagnostics;
	ColorConfigInfo info;
	Result inspect = InspectColorConfig(FileConfig(config_path), info, &diagnostics);
	Check(state, inspect.IsOk(), "file config inspection");
	Check(state, info.identity == config_path, "config identity");
	Check(state, !info.cache_id.IsEmpty(), "config cache id");
	Check(state, info.color_spaces.GetCount() == 2, "config color-space count");
	Check(state, info.default_source == "linear", "config default source");
	Check(state, info.default_display == "sRGB", "config default display");
	Check(state, info.default_view == "Raw", "config default view");

	Vector<String> views;
	String default_view;
	Check(state, GetDisplayViews(FileConfig(config_path), "sRGB", views,
	                             &default_view, &diagnostics).IsOk(),
	      "display view inspection");
	Check(state, views.GetCount() == 1 && views[0] == "Raw" &&
	             default_view == "Raw", "display view contents");
	Vector<String> preserved_views;
	preserved_views.Add("sentinel-view");
	String preserved_default = "sentinel-default";
	Result missing_views = GetDisplayViews(FileConfig(config_path), "Missing",
	                                       preserved_views, &preserved_default,
	                                       &diagnostics);
	Check(state, missing_views.code == ResultCode::InvalidArgument,
	      "missing display view inspection rejected");
	Check(state, preserved_views.GetCount() == 1 &&
	             preserved_views[0] == "sentinel-view" &&
	             preserved_default == "sentinel-default",
	      "failed display view inspection preserves output");

	Vector<String> builtins = GetBuiltinColorConfigNames();
	bool has_cg = false;
	for(const String& name : builtins)
		if(name == "cg-config-v4.0.0_aces-v2.0_ocio-v2.5")
			has_cg = true;
	Check(state, !builtins.IsEmpty(), "builtin config registry available");
	Check(state, has_cg, "expected builtin config available");
	ColorConfigRef builtin_ref;
	builtin_ref.source = ColorConfigSource::Builtin;
	builtin_ref.value = "cg-config-v4.0.0_aces-v2.0_ocio-v2.5";
	ColorConfigInfo builtin_info;
	Check(state, InspectColorConfig(builtin_ref, builtin_info, &diagnostics).IsOk(),
	      "builtin config inspection");
	Check(state, !builtin_info.color_spaces.IsEmpty() &&
	             !builtin_info.default_display.IsEmpty() &&
	             !builtin_info.default_view.IsEmpty(),
	      "builtin config usable defaults");

	ColorConfigInfo copied_info = info;
	Check(state, copied_info.identity == info.identity &&
	             copied_info.color_spaces == info.color_spaces &&
	             copied_info.displays == info.displays,
	      "config info copy semantics");
	ColorConfigInfo moved_info = pick(copied_info);
	Check(state, moved_info.identity == info.identity &&
	             moved_info.color_spaces == info.color_spaces,
	      "config info move semantics");

	ImageData rgba = MakeImage(SampleType::Float32, ChannelLayout::RGBA);
	SetSample(rgba, 0, 0.2f);
	SetSample(rgba, 1, 0.4f);
	SetSample(rgba, 2, 0.6f);
	SetSample(rgba, 3, 0.37f);
	ImageData rgba_before = rgba;
	ImageData rgba_out;
	bool rgba_ok = ApplyColorSpaceTransform(rgba, rgba_out, LogToLinear(config_path),
	                                        &diagnostics).IsOk();
	Check(state, rgba_ok, "Float32 RGBA color transform");
	Check(state, rgba_ok && Near(GetSample(rgba_out, 0), 0.25f) &&
	             Near(GetSample(rgba_out, 1), 0.16f) &&
	             Near(GetSample(rgba_out, 2), 0.64f),
	      "Float32 transformed RGB values");
	Check(state, rgba_ok && GetSample(rgba_out, 3) == GetSample(rgba, 3),
	      "Float32 alpha preserved");
	Check(state, SameSpec(rgba_out.spec, rgba.spec), "Float32 spec preserved");
	Check(state, rgba_out.metadata == rgba.metadata, "Float32 metadata preserved");
	Check(state, SameBytes(rgba.buffer, rgba_before.buffer), "source image immutable");

	ImageData half = MakeImage(SampleType::Float16, ChannelLayout::RGBA);
	const uint16_t half_bits[4] = {0x0000, 0x3800, 0x3c00, 0x3400};
	memcpy(half.buffer.Begin(), half_bits, sizeof(half_bits));
	ImageData half_before = half;
	ColorSpaceTransform identity;
	identity.config = FileConfig(config_path);
	identity.source = "linear";
	identity.destination = "linear";
	ImageData half_out;
	Check(state, ApplyColorSpaceTransform(half, half_out, identity, &diagnostics).IsOk(),
	      "Float16 identity transform");
	Check(state, SameBytes(half.buffer, half_out.buffer),
	      "Float16 exact identity bits");
	Check(state, SameBytes(half.buffer, half_before.buffer),
	      "Float16 source immutable");

	ImageData u8 = MakeImage(SampleType::UInt8, ChannelLayout::RGBA);
	SetSample(u8, 0, 0.2f);
	SetSample(u8, 1, 0.4f);
	SetSample(u8, 2, 0.6f);
	u8.buffer.Begin()[3] = 77;
	ImageData u8_out;
	bool u8_ok = ApplyColorSpaceTransform(u8, u8_out, LogToLinear(config_path),
	                                      &diagnostics).IsOk();
	Check(state, u8_ok, "UInt8 RGBA color transform");
	Check(state, u8_ok && u8_out.buffer.Begin()[0] == 64 && u8_out.buffer.Begin()[1] == 41 &&
	             u8_out.buffer.Begin()[2] == 163,
	      "UInt8 normalized transform values");
	Check(state, u8_ok && u8_out.buffer.Begin()[3] == 77, "UInt8 alpha exact preservation");

	ImageData u16 = MakeImage(SampleType::UInt16, ChannelLayout::RGB);
	SetSample(u16, 0, 0.2f);
	SetSample(u16, 1, 0.4f);
	SetSample(u16, 2, 0.6f);
	ImageData u16_out;
	bool u16_ok = ApplyColorSpaceTransform(u16, u16_out, LogToLinear(config_path),
	                                       &diagnostics).IsOk();
	Check(state, u16_ok, "UInt16 RGB color transform");
	uint16_t u16_values[3] = {};
	if(u16_ok)
		memcpy(u16_values, u16_out.buffer.Begin(), sizeof(u16_values));
	Check(state, u16_ok && u16_values[0] == 16384 && u16_values[1] == 10486 &&
	             u16_values[2] == 41942,
	      "UInt16 normalized transform values");

	ImageData multi;
	multi.spec.data_window = {7, -3, 7, -3};
	multi.spec.depth = 1;
	multi.spec.channels = 5;
	multi.spec.channel_layout = ChannelLayout::MultiChannel;
	multi.spec.sample_type = SampleType::Float32;
	multi.spec.channel_names.Add("Z");
	multi.spec.channel_names.Add("R");
	multi.spec.channel_names.Add("G");
	multi.spec.channel_names.Add("B");
	multi.spec.channel_names.Add("A");
	multi.spec.alpha_channel = 4;
	multi.buffer.Allocate(multi.spec);
	SetSample(multi, 0, 42.0f);
	SetSample(multi, 1, 0.2f);
	SetSample(multi, 2, 0.4f);
	SetSample(multi, 3, 0.6f);
	SetSample(multi, 4, 0.7f);
	multi.metadata.Set("layer", "beauty");
	ImageData multi_out;
	bool multi_ok = ApplyColorSpaceTransform(multi, multi_out, LogToLinear(config_path),
	                                         &diagnostics).IsOk();
	Check(state, multi_ok, "named MultiChannel color transform");
	Check(state, multi_ok && GetSample(multi_out, 0) == 42.0f && GetSample(multi_out, 4) == 0.7f,
	      "non-RGB MultiChannel samples preserved");
	Check(state, multi_ok && Near(GetSample(multi_out, 1), 0.25f) &&
	             Near(GetSample(multi_out, 2), 0.16f) &&
	             Near(GetSample(multi_out, 3), 0.64f),
	      "named MultiChannel RGB transformed");
	Check(state, SameSpec(multi_out.spec, multi.spec) && multi_out.metadata == multi.metadata,
	      "MultiChannel structure and metadata preserved");

	ImageData depth = MakeImage(SampleType::Float32, ChannelLayout::RGB, 1, 1);
	depth.spec.depth = 2;
	depth.buffer.Allocate(depth.spec);
	SetSample(depth, 0, 0.2f);
	SetSample(depth, 1, 0.4f);
	SetSample(depth, 2, 0.6f);
	SetSample(depth, 3, 0.3f);
	SetSample(depth, 4, 0.1f);
	SetSample(depth, 5, 0.5f);
	ImageData depth_out;
	bool depth_ok = ApplyColorSpaceTransform(depth, depth_out, LogToLinear(config_path),
	                                         &diagnostics).IsOk();
	Check(state, depth_ok, "depth-slice color transform");
	Check(state, depth_ok && Near(GetSample(depth_out, 0), 0.25f) &&
	             Near(GetSample(depth_out, 1), 0.16f) &&
	             Near(GetSample(depth_out, 2), 0.64f) &&
	             Near(GetSample(depth_out, 3), 0.49f) &&
	             Near(GetSample(depth_out, 4), 0.0625f) &&
	             Near(GetSample(depth_out, 5), 0.4225f),
	      "all depth slices transformed");
	Check(state, depth_out.spec.depth == 2 && depth_out.metadata == depth.metadata,
	      "depth structure and metadata preserved");

	ImageData alpha_alias = multi;
	alpha_alias.spec.alpha_channel = 1;
	ImageData alpha_alias_output = rgba;
	Result alpha_alias_result = ApplyColorSpaceTransform(alpha_alias, alpha_alias_output,
	                                                    LogToLinear(config_path),
	                                                    &diagnostics);
	Check(state, alpha_alias_result.code == ResultCode::Unsupported,
	      "MultiChannel alpha alias rejected");
	Check(state, HasDiagnostic(diagnostics, "IMGCOLOR_CHANNELS"),
	      "alpha alias diagnostic code");
	Check(state, SameBytes(alpha_alias_output.buffer, rgba.buffer) &&
	             SameSpec(alpha_alias_output.spec, rgba.spec),
	      "alpha alias rejection preserves output");

	DisplayTransform display;
	display.config = FileConfig(config_path);
	display.source = "linear";
	display.display = "sRGB";
	display.view = "Raw";
	ImageData display_out;
	Check(state, ApplyDisplayTransform(rgba, display_out, display, &diagnostics).IsOk(),
	      "display/view transform");
	Check(state, SameBytes(rgba.buffer, display_out.buffer),
	      "Raw display identity pixels");
	Check(state, display_out.metadata == rgba.metadata && SameSpec(display_out.spec, rgba.spec),
	      "display transform structure preserved");

	ImageData alias = rgba;
	Check(state, ApplyColorSpaceTransform(alias, alias, LogToLinear(config_path),
	                                     &diagnostics).IsOk(),
	      "in-place alias transform");
	Check(state, Near(GetSample(alias, 0), 0.25f) && Near(GetSample(alias, 1), 0.16f) &&
	             Near(GetSample(alias, 2), 0.64f) && GetSample(alias, 3) == 0.37f,
	      "in-place alias result");

	ImageData sentinel = rgba;
	ImageData preserved = sentinel;
	ImageData gray = MakeImage(SampleType::Float32, ChannelLayout::Gray);
	SetSample(gray, 0, 0.5f);
	Result gray_result = ApplyColorSpaceTransform(gray, preserved,
	                                             LogToLinear(config_path),
	                                             &diagnostics);
	Check(state, gray_result.code == ResultCode::Unsupported,
	      "Gray transform rejected");
	Check(state, HasDiagnostic(diagnostics, "IMGCOLOR_CHANNELS"),
	      "Gray rejection diagnostic code");
	Check(state, SameBytes(preserved.buffer, sentinel.buffer) &&
	             SameSpec(preserved.spec, sentinel.spec),
	      "failed channel transform preserves output");

	ImageData incomplete = multi;
	incomplete.spec.channel_names[3] = "depth";
	ImageData incomplete_output = sentinel;
	Result incomplete_result = ApplyColorSpaceTransform(incomplete, incomplete_output,
	                                                   LogToLinear(config_path),
	                                                   &diagnostics);
	Check(state, incomplete_result.code == ResultCode::Unsupported,
	      "incomplete MultiChannel RGB rejected");
	Check(state, SameBytes(incomplete_output.buffer, sentinel.buffer),
	      "incomplete RGB rejection preserves output");

	ColorSpaceTransform bad_config = LogToLinear(AppendFileName(root, "missing.ocio"));
	ImageData bad_output = sentinel;
	Result bad_config_result = ApplyColorSpaceTransform(rgba, bad_output, bad_config,
	                                                   &diagnostics);
	Check(state, !bad_config_result.IsOk(), "missing config rejected");
	Check(state, HasDiagnostic(diagnostics, "IMGCOLOR_CONFIG"),
	      "missing config diagnostic code");
	Check(state, SameBytes(bad_output.buffer, sentinel.buffer),
	      "missing config preserves output");

	ColorSpaceTransform missing_selection = LogToLinear(config_path);
	missing_selection.destination.Clear();
	ImageData selection_output = sentinel;
	Result selection_result = ApplyColorSpaceTransform(rgba, selection_output,
	                                                  missing_selection, &diagnostics);
	Check(state, selection_result.code == ResultCode::InvalidArgument,
	      "incomplete color selection rejected");
	Check(state, HasDiagnostic(diagnostics, "IMGCOLOR_SELECTION"),
	      "selection diagnostic code");
	Check(state, SameBytes(selection_output.buffer, sentinel.buffer),
	      "selection failure preserves output");

	DisplayTransform bad_display = display;
	bad_display.view = "Missing";
	ImageData display_failure = sentinel;
	Result display_failure_result = ApplyDisplayTransform(rgba, display_failure,
	                                                     bad_display, &diagnostics);
	Check(state, display_failure_result.code == ResultCode::InvalidArgument,
	      "invalid display view rejected");
	Check(state, HasDiagnostic(diagnostics, "IMGCOLOR_SELECTION"),
	      "display rejection diagnostic code");
	Check(state, SameBytes(display_failure.buffer, sentinel.buffer),
	      "display rejection preserves output");

	ImageData success_after_failure;
	Check(state, ApplyColorSpaceTransform(rgba, success_after_failure,
	                                     LogToLinear(config_path),
	                                     &diagnostics).IsOk(),
	      "success after failure");
	Check(state, diagnostics.IsEmpty(), "diagnostics reset on success");

	ColorConfigInfo untouched;
	untouched.identity = "sentinel";
	untouched.cache_id = "sentinel-cache";
	untouched.color_spaces.Add("sentinel-space");
	ColorConfigRef invalid_ref;
	invalid_ref.source = ColorConfigSource::File;
	invalid_ref.value = AppendFileName(root, "absent.ocio");
	Result inspect_failure = InspectColorConfig(invalid_ref, untouched, &diagnostics);
	Check(state, !inspect_failure.IsOk(), "failed config inspection");
	Check(state, untouched.identity == "sentinel" &&
	             untouched.cache_id == "sentinel-cache" &&
	             untouched.color_spaces.GetCount() == 1 &&
	             untouched.color_spaces[0] == "sentinel-space",
	      "failed config inspection preserves output");

	FileDelete(config_path);
	Check(state, !FileExists(config_path), "fixture cleanup");

	Cout() << "SUMMARY passed=" << state.passed << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
