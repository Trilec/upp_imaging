#include <OpenImageIO/imageio.h>

#include <png_io/PngIO.h>

#include <algorithm>
#include <cmath>
#include <cstring>

using namespace OIIO;
using namespace Upp;

namespace {

class PNGInput final : public ImageInput {
public:
    bool open(const std::string& name, ImageSpec& newspec) override
    {
        close();
        if(!LoadPngRgba8(name.c_str(), m_image, &m_error))
            return false;

        m_spec = ImageSpec(m_image.width, m_image.height, 4, TypeDesc::UINT8);
        m_spec.alpha_channel = 3;
        newspec = m_spec;
        return true;
    }

    bool close() override
    {
        m_image.Clear();
        m_error.Clear();
        return true;
    }

    bool read_native_scanline(int, int, int y, int, void* data) override
    {
        if(y < 0 || y >= m_image.height)
            return false;
        std::memcpy(data, m_image.pixels.Begin() + (size_t)y * (size_t)m_image.width,
                    (size_t)m_image.width * sizeof(PngRgba8));
        return true;
    }

    const char* format_name(void) const override { return "png"; }

    bool valid_file(Filesystem::IOProxy*) const override { return true; }

private:
    PngRgbaImage8 m_image;
    String m_error;
};

class PNGOutput final : public ImageOutput {
public:
    bool open(const std::string& name, const ImageSpec& newspec, OpenMode mode) override
    {
        if(!check_open(mode, newspec))
            return false;
        m_name = name;
        m_image.width = newspec.width;
        m_image.height = newspec.height;
        m_image.pixels.SetCount((size_t)m_image.width * (size_t)m_image.height);
        m_spec = newspec;
        return true;
    }

    bool write_scanline(int y, TypeDesc format, const image_span<const std::byte>& data) override
    {
        if(y < 0 || y >= m_image.height)
            return false;

        PngRgba8* row = m_image.pixels.Begin() + (size_t)y * (size_t)m_image.width;
        if(format == TypeDesc::UINT8) {
            std::memcpy(row, data.data(), (size_t)m_image.width * sizeof(PngRgba8));
            return true;
        }

        if(format != TypeDesc::FLOAT)
            return false;

        const float* src = reinterpret_cast<const float*>(data.data());
        for(int x = 0; x < m_image.width; ++x) {
            PngRgba8 c;
            c.r = (byte)std::clamp((int)std::lround(src[(size_t)x * 4 + 0] * 255.0f), 0, 255);
            c.g = (byte)std::clamp((int)std::lround(src[(size_t)x * 4 + 1] * 255.0f), 0, 255);
            c.b = (byte)std::clamp((int)std::lround(src[(size_t)x * 4 + 2] * 255.0f), 0, 255);
            c.a = (byte)std::clamp((int)std::lround(src[(size_t)x * 4 + 3] * 255.0f), 0, 255);
            row[x] = c;
        }
        return true;
    }

    bool write_scanline(int y, int, TypeDesc format, const void* data,
                        stride_t xstride) override
    {
        if(xstride == AutoStride)
            xstride = (stride_t)(format.size() * 4);
        std::vector<unsigned char> row((size_t)m_image.width * 4);
        for(int x = 0; x < m_image.width; ++x) {
            const unsigned char* p = (const unsigned char*)data + x * xstride;
            if(format == TypeDesc::UINT8) {
                std::memcpy(row.data() + (size_t)x * 4, p, 4);
            }
            else if(format == TypeDesc::FLOAT) {
                const float* f = (const float*)p;
                for(int c = 0; c < 4; ++c)
                    row[(size_t)x * 4 + c] = (unsigned char)std::clamp((int)std::lround(f[c] * 255.0f), 0, 255);
            }
            else
                return false;
        }
        std::memcpy(m_image.pixels.Begin() + (size_t)y * (size_t)m_image.width,
                    row.data(), row.size());
        return true;
    }

    bool close() override
    {
        if(m_name.IsEmpty())
            return true;
        String error;
        if(!SavePngRgba8(~m_name, m_image, &error)) {
            m_error = error;
            return false;
        }
        m_name.Clear();
        return true;
    }

    const char* format_name(void) const override { return "png"; }

private:
    String m_name;
    PngRgbaImage8 m_image;
    String m_error;
};

}

ImageInput* png_input_imageio_create()
{
    return new PNGInput();
}

ImageOutput* png_output_imageio_create()
{
    return new PNGOutput();
}

const char* png_input_extensions[] = {"png", nullptr};
const char* png_output_extensions[] = {"png", nullptr};

const char* png_imageio_library_version()
{
    return "png_io";
}

void RegisterOpenImageIOPNGPlugin()
{
    declare_imageio_format("png", png_input_imageio_create, png_input_extensions,
                           png_output_imageio_create, png_output_extensions,
                           png_imageio_library_version());
}
