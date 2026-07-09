#include <stdio.h>

#include <imaging_roundtrip_viewer_ocio/OcioPreview.h>

using namespace Upp;

namespace OCIO = OCIO_NAMESPACE;

static bool SameImage(const TestImageF& a, const TestImageF& b)
{
	if(a.width != b.width || a.height != b.height || a.pixels.GetCount() != b.pixels.GetCount())
		return false;
	for(int i = 0; i < a.pixels.GetCount(); ++i) {
		if(a.pixels[i].r != b.pixels[i].r || a.pixels[i].g != b.pixels[i].g || a.pixels[i].b != b.pixels[i].b || a.pixels[i].a != b.pixels[i].a)
			return false;
	}
	return true;
}

static void CopyTestImage(const TestImageF& src, TestImageF& dst)
{
	dst.width = src.width;
	dst.height = src.height;
	dst.pixels.SetCount(src.pixels.GetCount());
	for(int i = 0; i < src.pixels.GetCount(); ++i)
		dst.pixels[i] = src.pixels[i];
}

static bool HasRgbDifference(const TestImageF& a, const TestImageF& b)
{
	for(int i = 0; i < a.pixels.GetCount(); ++i) {
		if(a.pixels[i].r != b.pixels[i].r || a.pixels[i].g != b.pixels[i].g || a.pixels[i].b != b.pixels[i].b)
			return true;
	}
	return false;
}

static bool TestPreviewRecovery(const OCIO::ConstConfigRcPtr& config, const String& builtin)
{
	String source = OcioPreview::GetDefaultSourceColorSpace(config);
	Vector<String> displays = OcioPreview::GetDisplayNames(config);
	if(source.IsEmpty() || displays.IsEmpty())
		return false;
	String display = displays[0];
	Vector<String> views = OcioPreview::GetViewNames(config, display);
	if(views.IsEmpty())
		return false;
	String view = views[0];

	TestImageF src;
	src.width = 2;
	src.height = 1;
	src.pixels.SetCount(2);
	src.pixels[0] = {0.2f, 0.4f, 0.6f, 0.7f};
	src.pixels[1] = {0.3f, 0.1f, 0.5f, 0.9f};
	TestImageF original;
	CopyTestImage(src, original);

	TestImageF invalid_preview;
	String preview_error;
	if(OcioPreview::ApplyPreview(config, source, "invalid-display", "invalid-view", src, invalid_preview, preview_error))
		return false;
	if(preview_error.IsEmpty())
		return false;
	if(!SameImage(src, original))
		return false;

	TestImageF valid_preview;
	preview_error.Clear();
	if(!OcioPreview::ApplyPreview(config, source, display, view, src, valid_preview, preview_error))
		return false;
	if(valid_preview.pixels.GetCount() == 0 || valid_preview.pixels.GetCount() != src.pixels.GetCount())
		return false;
	if(!HasRgbDifference(src, valid_preview))
		return false;
	if(!SameImage(src, original))
		return false;

	printf("OCIO preview recovery %s: OK (%s)\n", ~builtin, ~OcioPreview::DescribeSelection(builtin, source, display, view));
	return true;
}

int main()
{
	int passed = 0;
	int failed = 0;

	Vector<String> builtins = OcioPreview::GetBuiltinConfigNames();
	if(builtins.GetCount() == 2) {
		printf("OCIO builtin list: OK\n");
		passed++;
	} else {
		printf("OCIO builtin list: FAIL\n");
		failed++;
	}

	TestImageF src;
	src.width = 2;
	src.height = 1;
	src.pixels.SetCount(2);
	src.pixels[0] = {0.2f, 0.4f, 0.6f, 0.7f};
	src.pixels[1] = {0.3f, 0.1f, 0.5f, 0.9f};
	TestImageF original;
	CopyTestImage(src, original);

	bool preview_ok = false;
	String selected_note;
	for(const String& builtin : builtins) {
		OCIO::ConstConfigRcPtr config;
		String error;
		if(!OcioPreview::LoadBuiltinConfig(builtin, config, error)) {
			printf("OCIO builtin config %s: FAIL (%s)\n", ~builtin, ~error);
			failed++;
			continue;
		}
		printf("OCIO builtin config %s: OK\n", ~builtin);
		passed++;

		String source = OcioPreview::GetDefaultSourceColorSpace(config);
		Vector<String> displays = OcioPreview::GetDisplayNames(config);
		for(const String& display : displays) {
			Vector<String> views = OcioPreview::GetViewNames(config, display);
			for(const String& view : views) {
				TestImageF preview;
				String preview_error;
				if(!OcioPreview::ApplyPreview(config, source, display, view, src, preview, preview_error))
					continue;
				if(!SameImage(src, original))
					continue;
				if(!HasRgbDifference(src, preview))
					continue;
				if(preview.pixels[0].a != src.pixels[0].a || preview.pixels[1].a != src.pixels[1].a)
					continue;
				selected_note = OcioPreview::DescribeSelection(builtin, source, display, view);
				printf("OCIO preview transform: OK (%s)\n", ~selected_note);
				printf("OCIO preview alpha preserved: OK\n");
				preview_ok = true;
				break;
			}
			if(preview_ok)
				break;
		}
		if(preview_ok)
			break;
	}

	bool recovery_ok = false;
	for(const String& builtin : builtins) {
		OCIO::ConstConfigRcPtr config;
		String error;
		if(!OcioPreview::LoadBuiltinConfig(builtin, config, error))
			continue;
		recovery_ok = TestPreviewRecovery(config, builtin);
		if(recovery_ok)
			break;
	}
	if(recovery_ok)
		passed++;
	else {
		printf("OCIO preview recovery: FAIL\n");
		failed++;
	}

	if(preview_ok)
		passed += 2;
	else {
		printf("OCIO preview transform: FAIL\n");
		printf("OCIO preview alpha preserved: FAIL\n");
		failed += 2;
	}

	if(SameImage(src, original)) {
		printf("OCIO raw source unchanged: OK\n");
		passed++;
	} else {
		printf("OCIO raw source unchanged: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
