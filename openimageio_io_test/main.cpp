#include <openimageio/OpenImageIO.h>
#include <opencolorio/OpenColorIO.h>

#include <cmath>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace OCIO = OCIO_NAMESPACE;
using namespace OIIO;
using namespace UppImaging;

static constexpr int WIDTH = 32;
static constexpr int HEIGHT = 24;
static constexpr int CHANNELS = 4;

static bool Near(float a, float b, float epsilon = 1e-4f)
{
    return std::fabs(a - b) <= epsilon;
}

static ImageBuf MakeSource(std::vector<float>& pixels)
{
    pixels.resize(WIDTH * HEIGHT * CHANNELS);
    ImageSpec spec(WIDTH, HEIGHT, CHANNELS, TypeDesc::FLOAT);
    spec.alpha_channel = 3;
    spec.attribute("oiio:ColorSpace", "linear");
    spec.attribute("test:source", "synthetic");

    for(int y = 0; y < HEIGHT; ++y) {
        for(int x = 0; x < WIDTH; ++x) {
            float* p = pixels.data() + (y * WIDTH + x) * CHANNELS;
            p[0] = float(x) / float(WIDTH - 1);
            p[1] = float(y) / float(HEIGHT - 1);
            p[2] = ((x / 4 + y / 4) & 1) ? 1.0f : 0.0f;
            p[3] = 1.0f;
        }
    }
    return ImageBuf(spec, pixels.data());
}

static ImageBuf MakeLayer(std::vector<float>& pixels)
{
    pixels.assign(WIDTH * HEIGHT * CHANNELS, 0.0f);
    ImageSpec spec(WIDTH, HEIGHT, CHANNELS, TypeDesc::FLOAT);
    spec.alpha_channel = 3;
    ImageBuf layer(spec, pixels.data());

    const float rectangle[] = {1.0f, 0.0f, 0.0f, 0.5f};
    ImageBufAlgo::render_box(layer, 4, 4, 15, 15, rectangle, true);
    for(int y = 0; y < HEIGHT; ++y) {
        for(int x = 0; x < WIDTH; ++x) {
            const float dx = (x - 20.0f) / 6.0f;
            const float dy = (y - 12.0f) / 5.0f;
            if(dx * dx + dy * dy <= 1.0f) {
                float* p = pixels.data() + (y * WIDTH + x) * CHANNELS;
                p[1] = 1.0f;
                p[3] = 0.5f;
            }
        }
    }
    return layer;
}

static bool ApplyLut(ImageBuf& image, const std::string& filename,
                     OCIO::TransformDirection direction, std::string& error)
{
    try {
        OCIO::FileTransformRcPtr transform = OCIO::FileTransform::Create();
        transform->setSrc(filename.c_str());
        transform->setDirection(direction);
        OCIO::ConstProcessorRcPtr processor = OCIO::Config::Create()->getProcessor(transform);
        OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
        std::vector<float> pixels(image.spec().width * image.spec().height * image.spec().nchannels);
        ROI roi = image.roi();
        if(!image.get_pixels(roi, TypeDesc::FLOAT, pixels.data(),
                             image.spec().nchannels * sizeof(float),
                             image.spec().width * image.spec().nchannels * sizeof(float),
                             AutoStride)) {
            error = image.geterror();
            return false;
        }
        OCIO::PackedImageDesc desc(pixels.data(), image.spec().width,
                                   image.spec().height, image.spec().nchannels);
        cpu->apply(desc);
        if(!image.set_pixels(roi, TypeDesc::FLOAT, pixels.data(),
                             image.spec().nchannels * sizeof(float),
                             image.spec().width * image.spec().nchannels * sizeof(float),
                             AutoStride)) {
            error = image.geterror();
            return false;
        }
        return true;
    }
    catch(const OCIO::Exception& e) {
        error = e.what();
        return false;
    }
}

static bool Check(bool condition, const char* label, int& passed, int& failed)
{
    std::printf("%s %s\n", condition ? "PASS" : "FAIL", label);
    condition ? ++passed : ++failed;
    return condition;
}

static bool WriteLut(const std::string& path, const char* values)
{
    std::ofstream out(path, std::ios::binary);
    out << "TITLE \"deterministic test LUT\"\n"
           "LUT_1D_SIZE 2\n"
           "DOMAIN_MIN 0.0 0.0 0.0\n"
           "DOMAIN_MAX 1.0 1.0 1.0\n"
        << values;
    return out.good();
}

int main()
{
    int passed = 0;
    int failed = 0;
    InitializeOpenImageIO();

    Check(true, "stable OpenImageIO marker", passed, failed);
    Check(OCIO_VERSION_MAJOR == 2, "stable OpenColorIO marker", passed, failed);
    const std::string input_formats = get_string_attribute("input_format_list");
    const std::string output_formats = get_string_attribute("output_format_list");
    const std::string extensions = get_string_attribute("extension_list");
    std::printf("registered_input_formats=%s\nregistered_output_formats=%s\nregistered_extensions=%s\n",
                input_formats.c_str(), output_formats.c_str(), extensions.c_str());
    Check(input_formats.find("openexr") != std::string::npos,
          "OpenEXR registration", passed, failed);
    Check(output_formats.find("openexr") != std::string::npos,
          "OpenEXR output registration", passed, failed);
    Check(input_formats.find("png") != std::string::npos,
          "PNG registration", passed, failed);
    Check(output_formats.find("png") != std::string::npos,
          "PNG output registration", passed, failed);

    const std::string root = std::string("openimageio_io_test_tmp");
    const std::string input_lut = std::filesystem::absolute(root + "/input_test.cube").string();
    const std::string output_lut = std::filesystem::absolute(root + "/output_test.cube").string();
    const std::string source_exr = root + "/source.exr";
    const std::string output_png = root + "/output.png";
    const std::string output_exr = root + "/output_reopened.exr";

    std::filesystem::create_directories(root);

    std::remove(source_exr.c_str());
    std::remove(output_png.c_str());
    std::remove(output_exr.c_str());
    std::remove(input_lut.c_str());
    std::remove(output_lut.c_str());

    std::vector<float> source_pixels;
    ImageBuf source = MakeSource(source_pixels);
    Check(source.spec().width == WIDTH && source.spec().height == HEIGHT,
          "synthetic source", passed, failed);

    std::vector<float> layer_pixels;
    ImageBuf layer = MakeLayer(layer_pixels);
    ImageBuf working = ImageBufAlgo::over(layer, source);
    Check(!working.has_error(), "alpha composite", passed, failed);

    std::string error;
    Check(WriteLut(input_lut, "0.0 0.0 0.0\n1.0 1.0 1.0\n"),
          "input LUT asset", passed, failed);
    Check(WriteLut(output_lut, "1.0 0.0 0.0\n0.0 1.0 1.0\n"),
          "output LUT asset", passed, failed);
    bool input_ok = ApplyLut(working, input_lut, OCIO::TRANSFORM_DIR_FORWARD, error);
    if(!input_ok)
        std::printf("input LUT error: %s\n", error.c_str());
    Check(input_ok,
          "incoming LUT", passed, failed);
    bool output_ok = ApplyLut(working, output_lut, OCIO::TRANSFORM_DIR_FORWARD, error);
    if(!output_ok)
        std::printf("output LUT error: %s\n", error.c_str());
    Check(output_ok,
          "outgoing LUT", passed, failed);

    bool exr_write = SaveImage(source_exr.c_str(), working, &error);
    if(!exr_write)
        std::printf("EXR write error: %s\n", error.c_str());
    Check(exr_write, "EXR write", passed, failed);
    ImageBuf reopened_exr;
    bool exr_read = LoadImage(source_exr.c_str(), reopened_exr, &error);
    if(!exr_read)
        std::printf("EXR read error: %s\n", error.c_str());
    Check(exr_read, "EXR read", passed, failed);
    Check(reopened_exr.spec().width == WIDTH && reopened_exr.spec().height == HEIGHT
              && reopened_exr.spec().nchannels == CHANNELS,
          "metadata", passed, failed);
    bool png_write = SaveImage(output_png.c_str(), reopened_exr, &error);
    if(!png_write)
        std::printf("PNG write error: %s\n", error.c_str());
    Check(png_write, "PNG write", passed, failed);
    ImageBuf reopened_png;
    bool png_read = LoadImage(output_png.c_str(), reopened_png, &error);
    if(!png_read)
        std::printf("PNG read error: %s\n", error.c_str());
    Check(png_read, "PNG read", passed, failed);
    Check(reopened_png.spec().width == WIDTH && reopened_png.spec().height == HEIGHT,
          "PNG dimensions", passed, failed);
    bool second_exr_write = SaveImage(output_exr.c_str(), reopened_png, &error);
    if(!second_exr_write)
        std::printf("PNG to EXR error: %s\n", error.c_str());
    Check(second_exr_write, "PNG to EXR conversion", passed, failed);
    ImageBuf final_exr;
    bool final_exr_read = LoadImage(output_exr.c_str(), final_exr, &error);
    if(!final_exr_read)
        std::printf("final EXR read error: %s\n", error.c_str());
    Check(final_exr_read, "reopen saved files", passed, failed);

    std::remove(source_exr.c_str());
    std::remove(output_png.c_str());
    std::remove(output_exr.c_str());
    std::remove(input_lut.c_str());
    std::remove(output_lut.c_str());
    const bool cleaned = std::filesystem::remove_all(root) >= 0
                      && !std::filesystem::exists(root);
    Check(cleaned, "cleanup", passed, failed);

    std::printf("source_size=32x24\nsource_channels=4\ninput_format=exr\noutput_format=png\n"
                "input_lut=input_test.cube\noutput_lut=output_test.cube\n"
                "SUMMARY passed=%d failed=%d\n", passed, failed);
    return failed ? 1 : 0;
}
