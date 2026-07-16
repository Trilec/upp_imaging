#include "OpenImageIO.h"

#include <mutex>

void RegisterOpenImageIOOpenEXRPlugin();
void RegisterOpenImageIOPNGPlugin();

namespace UppImaging {

void InitializeOpenImageIO()
{
    static std::once_flag once;
    std::call_once(once, [] {
        RegisterOpenImageIOOpenEXRPlugin();
        RegisterOpenImageIOPNGPlugin();
    });
}

bool LoadImage(const char* filename, OIIO::ImageBuf& destination,
               std::string* error)
{
    InitializeOpenImageIO();
    if(!filename || !*filename) {
        if(error)
            *error = "empty image filename";
        return false;
    }

    OIIO::ImageBuf loaded(filename);
    if(!loaded.read()) {
        if(error)
            *error = loaded.geterror();
        return false;
    }

    destination = std::move(loaded);
    return true;
}

bool SaveImage(const char* filename, const OIIO::ImageBuf& source,
               std::string* error)
{
    InitializeOpenImageIO();
    if(!filename || !*filename) {
        if(error)
            *error = "empty image filename";
        return false;
    }

    if(!source.write(filename)) {
        if(error)
            *error = source.geterror();
        return false;
    }
    return true;
}

}
