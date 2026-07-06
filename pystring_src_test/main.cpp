#include <Core/Core.h>

#include <stdio.h>

#include <pystring.h>

using namespace Upp;

static bool Check(const Vector<String>& got, const Vector<String>& expected)
{
	return got == expected;
}

int main()
{
	int passed = 0;
	int failed = 0;

	const std::vector<std::string> split = pystring::split("red,green,blue", ",");
	Vector<String> split_expected;
	split_expected.Add("red");
	split_expected.Add("green");
	split_expected.Add("blue");
	Vector<String> split_got;
	for(const auto& s : split)
		split_got.Add(s.c_str());
	if(Check(split_got, split_expected)) {
		printf("split: OK\n");
		passed++;
	} else {
		printf("split: FAIL\n");
		failed++;
	}

	const std::string joined = pystring::join("-", split);
	if(joined == "red-green-blue") {
		printf("join: OK\n");
		passed++;
	} else {
		printf("join: FAIL\n");
		failed++;
	}

	if(pystring::strip("  padded  ") == "padded") {
		printf("strip: OK\n");
		passed++;
	} else {
		printf("strip: FAIL\n");
		failed++;
	}

	if(pystring::lower("HeLLo") == "hello") {
		printf("lower: OK\n");
		passed++;
	} else {
		printf("lower: FAIL\n");
		failed++;
	}

	if(pystring::upper("HeLLo") == "HELLO") {
		printf("upper: OK\n");
		passed++;
	} else {
		printf("upper: FAIL\n");
		failed++;
	}

	if(pystring::startswith("OpenColorIO", "Open") && pystring::endswith("OpenColorIO", "ColorIO")) {
		printf("starts/ends: OK\n");
		passed++;
	} else {
		printf("starts/ends: FAIL\n");
		failed++;
	}

	if(pystring::replace("one two two", "two", "3") == "one 3 3") {
		printf("replace: OK\n");
		passed++;
	} else {
		printf("replace: FAIL\n");
		failed++;
	}

	if(pystring::find("abracadabra", "cada") == 4) {
		printf("find: OK\n");
		passed++;
	} else {
		printf("find: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
