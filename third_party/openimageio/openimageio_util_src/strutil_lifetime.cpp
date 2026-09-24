// Retain the pinned implementation and its pending-error reporting policy.
#include "upstream/libutil/strutil.cpp"

OIIO_NAMESPACE_3_1_BEGIN
namespace pvt {
void ReleaseCurrentThreadGlobalError()
{
    // Moving to a local holder reports an unconsumed error exactly as the
    // upstream TLS destructor does, then releases the string's capacity.
    ErrorHolder pending;
    pending.error_msg.swap(error_msg_holder.error_msg);
}
}
OIIO_NAMESPACE_3_1_END
