#ifndef UPP_IMAGING_IMAGING_ROUNDTRIP_TEST_SUPPORT_H
#define UPP_IMAGING_IMAGING_ROUNDTRIP_TEST_SUPPORT_H

#include <Core/Core.h>

struct TestRgbaF
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;
};

struct TestImageF
{
	int width = 0;
	int height = 0;
	Upp::Vector<TestRgbaF> pixels;
};

struct TestRgba8
{
	byte r = 0;
	byte g = 0;
	byte b = 0;
	byte a = 255;
};

static_assert(sizeof(TestRgba8) == 4, "TestRgba8 must stay packed");

struct TestRgb8
{
	byte r = 0;
	byte g = 0;
	byte b = 0;
};

static_assert(sizeof(TestRgb8) == 3, "TestRgb8 must stay packed");

struct TestRgba16
{
	Upp::uint16 r = 0;
	Upp::uint16 g = 0;
	Upp::uint16 b = 0;
	Upp::uint16 a = 65535;
};

static_assert(sizeof(TestRgba16) == 8, "TestRgba16 must stay packed");

struct TestImage8
{
	int width = 0;
	int height = 0;
	Upp::Vector<TestRgba8> pixels;

	void Clear();
	bool IsValid() const;
};

struct TestImage16
{
	int width = 0;
	int height = 0;
	Upp::Vector<TestRgba16> pixels;

	void Clear();
	bool IsValid() const;
};

struct RoundtripComparison
{
	bool dimensions_match = true;
	int different_components = 0;
	double max_error_r = 0.0;
	double max_error_g = 0.0;
	double max_error_b = 0.0;
	double max_error_a = 0.0;
	double mean_absolute_error = 0.0;
	double rmse = 0.0;
	Upp::String summary;

	RoundtripComparison();
};

struct RoundtripComparison8
{
	bool dimensions_match = true;
	int different_components = 0;
	int max_error_r = 0;
	int max_error_g = 0;
	int max_error_b = 0;
	int max_error_a = 0;
	Upp::String summary;
};

struct RoundtripComparison16
{
	bool dimensions_match = true;
	int different_components = 0;
	int max_error_r = 0;
	int max_error_g = 0;
	int max_error_b = 0;
	int max_error_a = 0;
	Upp::String summary;
};

struct TestRgbImage8
{
	int width = 0;
	int height = 0;
	Upp::Vector<TestRgb8> pixels;

	void Clear();
	bool IsValid() const;
};

struct LossyRgbComparison
{
	bool dimensions_match = true;

	int max_error_r = 0;
	int max_error_g = 0;
	int max_error_b = 0;

	double mean_absolute_error = 0.0;
	double rmse = 0.0;
	double psnr = 0.0;

	Upp::String summary;
};

TestImageF GenerateRoundtripTestPattern(int width, int height, bool include_hdr);
TestImage8 QuantizeToRgba8(const TestImageF& source);
TestImage16 QuantizeToRgba16(const TestImageF& source);
TestImageF NormalizeToFloat(const TestImage8& source);
TestImageF NormalizeToFloat(const TestImage16& source);
RoundtripComparison8 CompareExact(const TestImage8& expected, const TestImage8& actual);
RoundtripComparison16 CompareExact(const TestImage16& expected, const TestImage16& actual);
LossyRgbComparison CompareLossyRgb8(const TestRgbImage8& expected, const TestRgbImage8& actual);
RoundtripComparison CompareExact(const TestImageF& expected, const TestImageF& actual);
RoundtripComparison CompareTolerance(const TestImageF& expected, const TestImageF& actual, double tolerance);

#endif
