#include "ImagingCanvas.h"

namespace Upp {

ImagingCanvas::ImagingCanvas()
{
	SetMinSize(Size(DPI(640), DPI(480)));
}

void ImagingCanvas::SetPlaceholderText(const String& text)
{
	placeholder = text;
	Refresh();
}

void ImagingCanvas::Paint(Draw& w)
{
	Size sz = GetSize();
	w.DrawRect(sz, Color(34, 36, 40));
	const int tile = 16;
	for(int y = 0; y < sz.cy; y += tile)
		for(int x = 0; x < sz.cx; x += tile)
			if(((x / tile) + (y / tile)) & 1)
				w.DrawRect(x, y, tile, tile, Color(46, 48, 52));
	w.DrawText(20, 20, placeholder, StdFont(), White());
}

} // namespace Upp
