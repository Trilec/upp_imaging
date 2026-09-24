// libheif 1.23.1 registers built-in plugins during static initialization,
// even in applications that only link the library and never call heif_init.
// Compile the unmodified registry here so the fallback cleanup is ordered
// after registration (and therefore before registry destruction at exit).
#include "upstream/libheif/plugin_registry.cc"

namespace {
struct DefaultPluginLifetime {
    ~DefaultPluginLifetime()
    {
        // Normal paired heif_deinit already empties these registries. In the
        // unused-library case this releases the implicit libde265 init count.
        // Do not call heif_deinit without a corresponding heif_init.
        heif_unregister_decoder_plugins();
        heif_unregister_encoder_plugins();
    }
} default_plugin_lifetime;
}
