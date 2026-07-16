#include <OpenImageIO/imageio.h>

#include <openexr_io/OpenExrIO.h>

#include <cmath>
#include <cstring>

using namespace OIIO;
using namespace Upp;

namespace {

class OpenEXRInput final : public ImageInput {
public:
    bool open(const std::string& name, ImageSpec& newspec) override
    {
        close();
        if(!LoadExrRgbaF(name.c_str(), m_image, &m_error))
            return false;

        m_spec = ImageSpec(m_image.width, m_image.height, 4, TypeDesc::FLOAT);
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
        float* dst = reinterpret_cast<float*>(data);
        const ExrRgbaF* src = m_image.pixels.Begin() + (size_t)y * (size_t)m_image.width;
        for(int x = 0; x < m_image.width; ++x) {
            dst[(size_t)x * 4 + 0] = src[x].r;
            dst[(size_t)x * 4 + 1] = src[x].g;
            dst[(size_t)x * 4 + 2] = src[x].b;
            dst[(size_t)x * 4 + 3] = src[x].a;
        }
        return true;
    }

    const char* format_name(void) const override { return "openexr"; }

    bool valid_file(Filesystem::IOProxy*) const override { return true; }

private:
    ExrRgbaImageF m_image;
    String m_error;
};

class OpenEXROutput final : public ImageOutput {
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

        ExrRgbaF* row = m_image.pixels.Begin() + (size_t)y * (size_t)m_image.width;
        if(format == TypeDesc::FLOAT) {
            const float* src = reinterpret_cast<const float*>(data.data());
            for(int x = 0; x < m_image.width; ++x) {
                row[x].r = src[(size_t)x * 4 + 0];
                row[x].g = src[(size_t)x * 4 + 1];
                row[x].b = src[(size_t)x * 4 + 2];
                row[x].a = src[(size_t)x * 4 + 3];
            }
            return true;
        }

        if(format != TypeDesc::UINT8)
            return false;

        const unsigned char* src = reinterpret_cast<const unsigned char*>(data.data());
        for(int x = 0; x < m_image.width; ++x) {
            row[x].r = src[(size_t)x * 4 + 0] / 255.0f;
            row[x].g = src[(size_t)x * 4 + 1] / 255.0f;
            row[x].b = src[(size_t)x * 4 + 2] / 255.0f;
            row[x].a = src[(size_t)x * 4 + 3] / 255.0f;
        }
        return true;
    }

    bool write_scanline(int y, int, TypeDesc format, const void* data,
                        stride_t xstride) override
    {
        if(xstride == AutoStride)
            xstride = (stride_t)(format.size() * 4);
        std::vector<float> row((size_t)m_image.width * 4);
        for(int x = 0; x < m_image.width; ++x) {
            const unsigned char* p = (const unsigned char*)data + x * xstride;
            if(format == TypeDesc::FLOAT) {
                const float* f = (const float*)p;
                for(int c = 0; c < 4; ++c)
                    row[(size_t)x * 4 + c] = f[c];
            }
            else if(format == TypeDesc::UINT8) {
                for(int c = 0; c < 4; ++c)
                    row[(size_t)x * 4 + c] = p[c] / 255.0f;
            }
            else
                return false;
        }
        return write_scanline(y, TypeDesc::FLOAT,
                              image_span<const std::byte>((const std::byte*)row.data(), 4,
                                                           m_image.width, 1, 1));
    }

    bool close() override
    {
        if(m_name.IsEmpty())
            return true;
        String error;
        if(!SaveExrRgbaF(~m_name, m_image, false, true, &error)) {
            m_error = error;
            return false;
        }
        m_name.Clear();
        return true;
    }

    const char* format_name(void) const override { return "openexr"; }

private:
    String m_name;
    ExrRgbaImageF m_image;
    String m_error;
};

}

ImageInput* openexr_input_imageio_create()
{
    return new OpenEXRInput();
}

ImageOutput* openexr_output_imageio_create()
{
    return new OpenEXROutput();
}

const char* openexr_input_extensions[] = {"exr", "sxr", "mxr", nullptr};
const char* openexr_output_extensions[] = {"exr", nullptr};

const char* openexr_imageio_library_version()
{
    return "openexr_io";
}

void RegisterOpenImageIOOpenEXRPlugin()
{
    declare_imageio_format("openexr", openexr_input_imageio_create, openexr_input_extensions,
                           openexr_output_imageio_create, openexr_output_extensions,
                           openexr_imageio_library_version());
}
