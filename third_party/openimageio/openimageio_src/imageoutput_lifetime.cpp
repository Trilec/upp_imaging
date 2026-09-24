// See imageinput_lifetime.cpp. Worker TLS is released when workers join.
#include "upstream/libOpenImageIO/imageoutput.cpp"

OIIO_NAMESPACE_3_1_BEGIN
namespace pvt {
void ReleaseCurrentThreadOutputErrors()
{
    output_error_messages.clear();
    output_error_messages.rehash(0);
}
}
OIIO_NAMESPACE_3_1_END
