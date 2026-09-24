#ifndef _imaging_preview_coalescing_imaging_preview_coalescing_h_
#define _imaging_preview_coalescing_imaging_preview_coalescing_h_

#include <Core/Core.h>

namespace Upp {

struct PreviewRenderCoalescer {
	bool pending = false;
	bool scheduled = false;
	int scheduled_id = 0;
	int next_id = 1;

	bool RequestDeferred();
	bool RequestImmediate();
	bool ShouldRunCallback(int callback_id, bool& render_now);
};

} // namespace Upp

#endif
