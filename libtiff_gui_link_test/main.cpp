#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

#include <libtiff/tiffio.h>

using namespace Upp;

GUI_APP_MAIN
{
	(void)TIFFGetVersion();
}
