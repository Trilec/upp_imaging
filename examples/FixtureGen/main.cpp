#include <Core/Core.h>
#include <oiio/OIIO.h>

#include <filesystem>
#include <string>
#include <vector>

using namespace Upp;

CONSOLE_APP_MAIN
{
	std::filesystem::path root = std::filesystem::temp_directory_path() / "opencode" / "openimageio_io_test_fixtures";
	std::filesystem::create_directories(root);
	std::filesystem::path out = root / "prefixed_channels.exr";

	OIIO::ImageSpec spec(32, 24, 4, OIIO::TypeDesc::FLOAT);
	spec.channelnames = { "left.R", "left.G", "left.B", "left.A" };
	spec.alpha_channel = 3;
	spec.attribute("oiio:ColorSpace", "linear");

	std::vector<float> pixels((size_t)spec.width * spec.height * spec.nchannels);
	for(int y = 0; y < spec.height; ++y) {
		for(int x = 0; x < spec.width; ++x) {
			float* p = pixels.data() + ((size_t)y * spec.width + x) * spec.nchannels;
			p[0] = float(x) / float(spec.width - 1);
			p[1] = float(y) / float(spec.height - 1);
			p[2] = ((x / 4 + y / 4) & 1) ? 1.0f : 0.0f;
			p[3] = 1.0f;
		}
	}

	OIIO::ImageBuf image(spec);
	if(!image.set_pixels(image.roi(), OIIO::TypeDesc::FLOAT, pixels.data(),
				   OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride)) {
		std::printf("set_pixels failed: %s\n", image.geterror().c_str());
		SetExitCode(1);
		return;
	}

	std::string error;
	if(!UppImaging::SaveImage(out.string().c_str(), image, &error)) {
		std::printf("save failed: %s\n", error.c_str());
		SetExitCode(1);
		return;
	}

	std::printf("%s\n", out.string().c_str());
}
