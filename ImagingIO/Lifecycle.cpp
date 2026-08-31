#include "ImagingIO.h"

#include <OpenImageIO/OIIO.h>

namespace Upp {
namespace Imaging {

ImageIOApplicationScope::ImageIOApplicationScope()
{
	UppImaging::InitializeOpenImageIO();
}

ImageIOApplicationScope::~ImageIOApplicationScope() noexcept
{
	UppImaging::ShutdownOpenImageIO();
}

} // namespace Imaging
} // namespace Upp
