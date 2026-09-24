# openimageio_plugin_heif

Static OpenImageIO 3.1.15.0 HEIF-family input registration backed by `libheif_src` 1.23.5.

Supported registry extensions are `.heic`, `.heif`, `.heics`, `.hif` and `.avif`. The exact upstream OIIO `heifinput.cpp` is compiled from `openimageio_plugins_src`; no OIIO HEIF writer is compiled or registered.


## Lifetime

InitializeOpenImageIO calls the upstream oiio_heif_init once and registers one
process-exit shutdown callback inside its own call_once. The upstream helper
also uses call_once, shared with the HEIF reader factory: creating readers or
repeating InitializeOpenImageIO does not acquire additional libheif init counts.
The one repository-owned heif_deinit balances that one heif_init and releases
libde265 plugin caches before U++ HEAPDBG. This is production initialization;
no test prewarm or test-only shutdown is needed.

Repository readers and decoded HEIF objects are operation-scoped and destroyed
before exit; test workers are joined. Applications must likewise destroy their
native HEIF objects and join users before process teardown, and must not call
these internal init/shutdown wrappers independently or use HEIF from later
static destructors. The exit hook is not an arbitrary-global-object lifetime
manager. Recheck this pairing against upstream when either pin changes.

The lower libheif_src package additionally owns a same-translation-unit cleanup
for implicit built-in plugin registration when the library is linked but never
initialized. It does not acquire/release a heif_init count and becomes a no-op
when the normal heif_deinit has emptied the registries. This closes the
imaging_test link-only shutdown failure without adding a test prewarm.

Current validation targets and results are listed in `tests/acceptance.txt` and `docs/WINDOWS_ACCEPTANCE.md` at the repository root.
