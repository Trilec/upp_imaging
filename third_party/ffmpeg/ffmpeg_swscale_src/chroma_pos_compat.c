/*
 * FFmpeg n9.0.1 places ff_sws_chroma_pos() inside CONFIG_UNSTABLE in
 * libswscale/format.c, while the stable legacy path in graph.c calls it
 * unconditionally.  Keep the repository's unstable backends disabled and
 * materialize the pinned implementation for that configuration only.
 */

#include "config.h"

#include "libavutil/pixdesc.h"
#include "libswscale/format.h"

#if !CONFIG_UNSTABLE

void ff_sws_chroma_pos(const SwsFormat *fmt, bool *incomplete,
                       int *out_x_pos, int *out_y_pos)
{
    enum AVChromaLocation chroma_loc = fmt->loc;
    const int sub_x = fmt->desc->log2_chroma_w;
    const int sub_y = fmt->desc->log2_chroma_h;
    int x_pos, y_pos;

    if (chroma_loc == AVCHROMA_LOC_UNSPECIFIED) {
        chroma_loc = AVCHROMA_LOC_CENTER;
        *incomplete |= sub_x || sub_y;
    }

    av_chroma_location_enum_to_pos(&x_pos, &y_pos, chroma_loc);
    x_pos *= (1 << sub_x) - 1;
    y_pos *= (1 << sub_y) - 1;

    if (sub_y && fmt->interlaced) {
        if (fmt->field == FIELD_BOTTOM)
            y_pos += (256 << sub_y) - 256;
        y_pos >>= 1;
    }

    *out_x_pos = x_pos;
    *out_y_pos = y_pos;
}

#endif
