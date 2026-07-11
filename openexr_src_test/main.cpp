#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <fstream>
#include <string>

#include <direct.h>

#include <openexr_src/Imf.h>

#ifndef UPP_IMAGING_LOCAL_OPENEXR_SOURCE_INCLUDE
#error local OpenEXR source package not selected
#endif

namespace
{
    constexpr int kWidth = 4;
    constexpr int kHeight = 3;

    struct Result
    {
        int passed = 0;
        int failed = 0;
    };

    bool Check(Result& result, bool condition, const char* passText, const char* failText = nullptr)
    {
        if (condition)
        {
            std::printf("%s\n", passText);
            ++result.passed;
            return true;
        }

        std::printf("FAIL %s\n", failText ? failText : passText);
        ++result.failed;
        return false;
    }

    void RemoveIfExists(const char* path)
    {
        std::remove(path);
    }

    bool EnsureWorkspace(const char* dir)
    {
        if (_mkdir(dir) == 0)
            return true;
        return errno == EEXIST;
    }

    Imf::Rgba Pixel(float r, float g, float b, float a)
    {
        return Imf::Rgba(r, g, b, a);
    }

    void FillSource(Imf::Array2D<Imf::Rgba>& pixels)
    {
        const Imf::Rgba pattern[kHeight][kWidth] = {
            {
                Pixel(0.0f, 0.25f, 0.5f, 1.0f),
                Pixel(0.25f, 0.5f, 0.75f, 0.75f),
                Pixel(0.5f, 0.75f, 1.0f, 0.5f),
                Pixel(1.0f, 0.5f, 0.25f, 0.25f),
            },
            {
                Pixel(2.0f, 1.0f, 0.5f, 1.0f),
                Pixel(1.0f, 2.0f, 0.25f, 0.75f),
                Pixel(0.5f, 0.25f, 2.0f, 0.5f),
                Pixel(0.25f, 1.0f, 2.0f, 0.25f),
            },
            {
                Pixel(1.0f, 0.0f, 0.0f, 1.0f),
                Pixel(0.0f, 1.0f, 0.0f, 0.75f),
                Pixel(0.0f, 0.0f, 1.0f, 0.5f),
                Pixel(0.0f, 0.0f, 0.0f, 0.25f),
            }
        };

        for (int y = 0; y < kHeight; ++y)
            for (int x = 0; x < kWidth; ++x)
                pixels[x][y] = pattern[y][x];
    }

    bool ComparePixels(const Imf::Array2D<Imf::Rgba>& expected,
                       const Imf::Array2D<Imf::Rgba>& actual,
                       int& differentPixels,
                       int& differentComponents,
                       float& maxErrorR,
                       float& maxErrorG,
                       float& maxErrorB,
                       float& maxErrorA)
    {
        bool ok = true;
        int mismatchReports = 0;

        for (int y = 0; y < kHeight; ++y)
        {
            for (int x = 0; x < kWidth; ++x)
            {
                const Imf::Rgba& e = expected[x][y];
                const Imf::Rgba& a = actual[x][y];
                bool pixelMismatch = false;

                auto compare = [&](const char* channel, float expectedValue, float actualValue, float& maxError)
                {
                    const float error = std::fabs(expectedValue - actualValue);
                    if (error > maxError)
                        maxError = error;
                    if (error != 0.0f)
                    {
                        ++differentComponents;
                        pixelMismatch = true;
                        ok = false;
                        if (mismatchReports < 8)
                        {
                            std::printf("Mismatch pixel (%d,%d) channel %s expected=%g actual=%g\n",
                                        x, y, channel, expectedValue, actualValue);
                            ++mismatchReports;
                        }
                    }
                };

                compare("R", static_cast<float>(e.r), static_cast<float>(a.r), maxErrorR);
                compare("G", static_cast<float>(e.g), static_cast<float>(a.g), maxErrorG);
                compare("B", static_cast<float>(e.b), static_cast<float>(a.b), maxErrorB);
                compare("A", static_cast<float>(e.a), static_cast<float>(a.a), maxErrorA);

                if (pixelMismatch)
                    ++differentPixels;
            }
        }

        return ok;
    }
}

int main()
{
    Result result;

    const char* const workspace = "out/openexr_src_test";
    const std::string exrPath = std::string(workspace) + "/strict_roundtrip.exr";
    const std::string malformedPath = std::string(workspace) + "/malformed_input.bin";

    if (!Check(result, OPENEXR_VERSION_MAJOR == 3 && OPENEXR_VERSION_MINOR == 4 && OPENEXR_VERSION_PATCH == 13,
               "PASS OpenEXR version 3.4.13", "OpenEXR version 3.4.13"))
    {
        std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
        return 1;
    }

    if (!Check(result, EnsureWorkspace(workspace), "PASS temporary workspace", "temporary workspace"))
    {
        std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
        return 1;
    }

    RemoveIfExists(exrPath.c_str());
    RemoveIfExists(malformedPath.c_str());

    Imf::Array2D<Imf::Rgba> source(kWidth, kHeight);
    Imf::Array2D<Imf::Rgba> destination(kWidth, kHeight);
    FillSource(source);

    try
    {
        {
            Imf::RgbaOutputFile writer(
                exrPath.c_str(),
                kWidth,
                kHeight,
                Imf::WRITE_RGBA,
                1.0f,
                Imath::V2f(0.0f, 0.0f),
                1.0f,
                Imf::INCREASING_Y,
                Imf::ZIP_COMPRESSION);

            writer.setFrameBuffer(&source[0][0], kHeight, 1);
            writer.writePixels(kHeight);

            Check(result, writer.compression() == Imf::ZIP_COMPRESSION, "PASS high-level EXR write", "high-level EXR write");
        }

        std::ifstream file(exrPath.c_str(), std::ios::binary | std::ios::ate);
        const unsigned long long fileSize = file ? static_cast<unsigned long long>(file.tellg()) : 0ULL;
        Check(result, fileSize > 0, "PASS file exists and is non-empty", "file exists and is non-empty");

        Imf::RgbaInputFile reader(exrPath.c_str());
        const Imath::Box2i dataWindow = reader.dataWindow();
        const int width = dataWindow.max.x - dataWindow.min.x + 1;
        const int height = dataWindow.max.y - dataWindow.min.y + 1;

        Check(result,
              dataWindow.min.x == 0 && dataWindow.min.y == 0 && dataWindow.max.x == 3 && dataWindow.max.y == 2,
              "PASS data window 4 x 3",
              "data window 4 x 3");
        Check(result, width == 4 && height == 3, "PASS width/height 4 x 3", "width/height 4 x 3");
        Check(result, reader.compression() == Imf::ZIP_COMPRESSION, "PASS ZIP compression", "ZIP compression");
        Check(result, reader.lineOrder() == Imf::INCREASING_Y, "PASS line order INCREASING_Y", "line order INCREASING_Y");

        reader.setFrameBuffer(&destination[0][0], kHeight, 1);
        reader.readPixels(0, kHeight - 1);
        Check(result, true, "PASS high-level EXR read");

        int differentPixels = 0;
        int differentComponents = 0;
        float maxErrorR = 0.0f;
        float maxErrorG = 0.0f;
        float maxErrorB = 0.0f;
        float maxErrorA = 0.0f;
        const bool exact = ComparePixels(source, destination, differentPixels, differentComponents,
                                         maxErrorR, maxErrorG, maxErrorB, maxErrorA);

        std::printf("width=%d\n", width);
        std::printf("height=%d\n", height);
        std::printf("different_pixels=%d\n", differentPixels);
        std::printf("different_components=%d\n", differentComponents);
        std::printf("max_error_r=%g\n", maxErrorR);
        std::printf("max_error_g=%g\n", maxErrorG);
        std::printf("max_error_b=%g\n", maxErrorB);
        std::printf("max_error_a=%g\n", maxErrorA);

        Check(result,
              exact && differentPixels == 0 && differentComponents == 0 &&
                  maxErrorR == 0.0f && maxErrorG == 0.0f && maxErrorB == 0.0f && maxErrorA == 0.0f,
              "PASS exact RGBA pixel comparison",
              "exact RGBA pixel comparison");

        {
            std::ofstream malformed(malformedPath.c_str(), std::ios::binary | std::ios::trunc);
            malformed.write("not an exr", 10);
        }

        bool rejected = false;
        std::string diagnostic;
        try
        {
            Imf::RgbaInputFile malformedReader(malformedPath.c_str());
            (void)malformedReader;
        }
        catch (const std::exception& e)
        {
            rejected = true;
            diagnostic = e.what();
        }

        if (rejected && !diagnostic.empty())
        {
            std::printf("PASS malformed file rejected\n");
            std::printf("diagnostic=%s\n", diagnostic.c_str());
            ++result.passed;
        }
        else
        {
            std::printf("FAIL malformed file rejected\n");
            ++result.failed;
        }
    }
    catch (const std::exception& e)
    {
        std::printf("EXCEPTION %s\n", e.what());
        ++result.failed;
    }

    RemoveIfExists(exrPath.c_str());
    RemoveIfExists(malformedPath.c_str());

    std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
    return result.failed ? 1 : 0;
}
