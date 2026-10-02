#ifndef _ImagingCore_TrustedInput_h_
#define _ImagingCore_TrustedInput_h_
#include <Core/Core.h>
#include <filesystem>

namespace Upp { namespace Imaging {
// Supported contract: trusted, stable local regular files. This preflight
// bounds file input; it is not content validation, a sandbox or a TOCTOU guard.
inline bool CheckTrustedLocalInput(const String& path, int64 max_bytes, String& error)
{
    error.Clear();
    auto remote = [](const String& value) {
        return value.StartsWith("\\\\") || value.StartsWith("//") || value.Find("://") >= 0;
    };
    if(path.IsEmpty() || max_bytes <= 0 || remote(path)) {
        error = "input must be a trusted local file"; return false;
    }
    std::error_code ec;
    auto resolved = std::filesystem::canonical(std::filesystem::u8path(path.Begin()), ec);
    if(ec || remote(String(resolved.u8string().c_str())) ||
       !std::filesystem::is_regular_file(resolved, ec) || ec) {
        error = "input must be an existing local regular file"; return false;
    }
    auto bytes = std::filesystem::file_size(resolved, ec);
    if(ec || bytes == 0 || bytes > (uint64_t)max_bytes) {
        error = "input exceeds the file-size policy or is empty"; return false;
    }
    return true;
}
} }
#endif
