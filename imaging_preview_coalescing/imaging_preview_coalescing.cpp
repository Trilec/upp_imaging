#include "imaging_preview_coalescing.h"

namespace Upp {

bool PreviewRenderCoalescer::RequestDeferred()
{
	pending = true;
	if(scheduled)
		return false;
	scheduled = true;
	scheduled_id = next_id++;
	return true;
}

bool PreviewRenderCoalescer::RequestImmediate()
{
	pending = false;
	scheduled = false;
	scheduled_id = next_id++;
	return true;
}

bool PreviewRenderCoalescer::ShouldRunCallback(int callback_id, bool& render_now)
{
	if(!scheduled || callback_id != scheduled_id) {
		render_now = false;
		return false;
	}
	render_now = pending;
	pending = false;
	scheduled = false;
	return render_now;
}

} // namespace Upp
