// Keep the pinned source unchanged; expose bounded shutdown of MinGW's
// main-thread TLS error storage before U++ performs its heap audit.
#include "upstream/libOpenImageIO/imageinput.cpp"

OIIO_NAMESPACE_3_1_BEGIN
namespace pvt {
void ReleaseCurrentThreadInputErrors()
{
    input_error_messages.clear();
    input_error_messages.rehash(0);
}
}
OIIO_NAMESPACE_3_1_END
