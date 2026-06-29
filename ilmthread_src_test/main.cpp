#include <stdio.h>

#include <ilmthread_src/IlmThread.h>
#include <ilmthread_src/IlmThreadSemaphore.h>

int main()
{
	int passed = 0;
	int failed = 0;

	int hw = ILMTHREAD_NAMESPACE::supportsThreads();
	printf("IlmThread supportsThreads: %d\n", hw);
	passed++;

	ILMTHREAD_NAMESPACE::Semaphore sem(0);
	sem.post();
	sem.wait();
	printf("IlmThread semaphore probe: OK\n");
	passed++;

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
