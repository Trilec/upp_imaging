#include <stdio.h>

#include <fmt_src/format.h>

int main()
{
	int passed = 0;
	int failed = 0;

	const std::string text = fmt::format("{} + {} = {}", 2, 3, 5);
	if(text == "2 + 3 = 5") {
		printf("fmt source probe: OK\n");
		passed++;
	}
	else {
		printf("fmt source probe: FAIL (%s)\n", text.c_str());
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
