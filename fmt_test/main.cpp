#include <stdio.h>

#include <fmt/format.h>

int main()
{
	int passed = 0;
	int failed = 0;

	const std::string text = fmt::format("{} {}", "fmt", 12.2);
	if(text == "fmt 12.2") {
		printf("fmt user probe: OK\n");
		passed++;
	}
	else {
		printf("fmt user probe: FAIL (%s)\n", text.c_str());
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
