#include "ImagingWorkbench.h"

GUI_APP_MAIN
{
	Upp::ImagingWorkbench window;
	// Bounded release startup check, using the normal GUI loop and Close path.
	if(Upp::CommandLine().GetCount() == 1 && Upp::CommandLine()[0] == "--smoke")
		window.SetTimeCallback(1000, [&] { window.Close(); });
	window.Run();
}
