#include <stdio.h>

#include <robinmap_src/robin_map.h>

int main()
{
	int passed = 0;
	int failed = 0;

	tsl::robin_map<int, int> values;
	values[4] = 16;
	values.insert(std::make_pair(7, 49));

	if(values.size() == 2 && values.at(4) == 16 && values.at(7) == 49) {
		printf("robin-map source probe: OK\n");
		passed++;
	}
	else {
		printf("robin-map source probe: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
