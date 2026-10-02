#pragma once

#include "expat/expat.h"

#include <OpenColorIO/OpenColorIO.h>
// Repository policy: disable XML input until the unresolved Expat CPU issue
// has a fix. Cover direct CDL APIs as well as FileTransform dispatch.
inline XML_Parser UppImagingDisabledOcioXmlParser(const XML_Char*)
{
    throw OCIO_NAMESPACE::Exception("OCIO XML input is disabled by the Windows imaging policy");
}
#define XML_ParserCreate UppImagingDisabledOcioXmlParser
