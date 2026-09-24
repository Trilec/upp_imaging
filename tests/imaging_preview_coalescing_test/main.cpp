#include <Core/Core.h>

#include <cstdio>

#include <imaging_preview_coalescing/imaging_preview_coalescing.h>

using namespace Upp;

struct Result {
	int passed = 0;
	int failed = 0;
};

static bool Check(Result& result, bool condition, const char* label)
{
	std::printf("%s: %s\n", label, condition ? "PASS" : "FAIL");
	if(condition)
		++result.passed;
	else
		++result.failed;
	return condition;
}

int main()
{
	Result result;
	PreviewRenderCoalescer state;
	bool render_now = false;

	Check(result, state.RequestDeferred(), "PASS one deferred request schedules");
	int first_id = state.scheduled_id;
	Check(result, state.scheduled && state.pending, "PASS deferred state set");
	Check(result, !state.RequestDeferred(), "PASS repeated deferred request does not reschedule");
	Check(result, state.scheduled_id == first_id, "PASS scheduled id stays stable while queued");
	Check(result, state.ShouldRunCallback(first_id, render_now) && render_now, "PASS callback renders latest state");
	Check(result, !state.scheduled && !state.pending, "PASS scheduled state clears after callback");

	Check(result, state.RequestDeferred(), "PASS later request can schedule again");
	int second_id = state.scheduled_id;
	Check(result, state.RequestImmediate(), "PASS immediate final request accepted");
	Check(result, !state.scheduled && !state.pending, "PASS immediate clears scheduled lock");
	render_now = true;
	Check(result, !state.ShouldRunCallback(second_id, render_now) && !render_now, "PASS stale callback cannot render or lock future work");
	Check(result, state.RequestDeferred(), "PASS later request after stale callback can schedule");
	int third_id = state.scheduled_id;
	render_now = false;
	Check(result, state.ShouldRunCallback(third_id, render_now) && render_now, "PASS fresh callback renders after stale one");
	Check(result, !state.scheduled && !state.pending, "PASS final callback clears state");

	std::printf("SUMMARY passed=%d failed=%d\n", result.passed, result.failed);
	return result.failed ? 1 : 0;
}
