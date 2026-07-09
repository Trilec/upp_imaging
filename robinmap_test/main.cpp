#include <stdio.h>

#include <string>

#include <robinmap/robin_map.h>

int main()
{
	int passed = 0;
	int failed = 0;

	tsl::robin_map<std::string, int> values;
	values["fmt"] = 12;
	values.insert(std::make_pair(std::string("robin"), 1));

	if(values.size() == 2 && values["fmt"] == 12 && values.at("robin") == 1) {
		printf("robin-map user probe: OK\n");
		passed++;
	}
	else {
		printf("robin-map user probe: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
