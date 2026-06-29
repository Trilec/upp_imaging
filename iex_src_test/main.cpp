#include <stdio.h>

#include <iex_src/Iex.h>

int main()
{
	int passed = 0;
	int failed = 0;

	try {
		throw IEX_NAMESPACE::ArgExc("iex probe");
	}
	catch(const IEX_NAMESPACE::BaseExc& exc) {
		printf("Iex exception probe: OK\n");
		printf("caught: %s\n", exc.what());
		passed++;
	}
	catch(...) {
		printf("Iex exception probe: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
