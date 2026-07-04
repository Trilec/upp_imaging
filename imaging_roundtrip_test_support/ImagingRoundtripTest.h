#ifndef UPP_IMAGING_IMAGING_ROUNDTRIP_TEST_SUPPORT_H
#define UPP_IMAGING_IMAGING_ROUNDTRIP_TEST_SUPPORT_H

#include <Core/Core.h>

struct TestRgbaF
{
	float r;
	float g;
	float b;
	float a;
};

struct TestImageF
{
	int width;
	int height;
	Upp::Vector<TestRgbaF> pixels;
};

struct RoundtripComparison
{
	bool dimensions_match;
	int different_components;
	double max_error_r;
	double max_error_g;
	double max_error_b;
	double max_error_a;
	double mean_absolute_error;
	double rmse;
	Upp::String summary;

	RoundtripComparison();
};

TestImageF GenerateRoundtripTestPattern(int width, int height, bool include_hdr);
RoundtripComparison CompareExact(const TestImageF& expected, const TestImageF& actual);
RoundtripComparison CompareTolerance(const TestImageF& expected, const TestImageF& actual, double tolerance);

#endif
