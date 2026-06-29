# U++ plugin/png Comparison

U++ already includes `plugin/png`.

Current local observations:

- `plugin/png` is built around U++ `Draw`, `Image`, `Raster`, `PNGRaster`, and `PNGEncoder`
- it is suitable for simple automated PNG round-trip tests in normal U++ application code
- on Windows in this local CLANGx64 build, `plugin/png` uses U++'s bundled `plugin/png/lib/png.h`
- on Windows in this local CLANGx64 build, `Core` and `plugin/png` also rely on U++ `plugin/z`
- in the tested `Image` round-trip path, opaque RGBA pixels round-trip as expected
- in the tested `Image` round-trip path, the partial-alpha pixel preserves alpha, but its low-level RGB payload under alpha was not preserved as an exact channel value

Why keep direct libpng packages anyway:

- future imaging or VFX-oriented work may need stricter control over the exact libpng version
- future work may need low-level control outside `Draw`/`Image` abstractions
- future work may need exact channel-layout handling without U++ image abstraction steps
- future work may need fuller control over metadata and profile handling
- future work may need high-bit-depth preservation checks

Current package roles:

- `libpng_src` remains the strict imported-source validation path
- `libpng` remains the user-facing package for non-`plugin/png` application use
- `plugin/png` is useful as a comparison path and a simple U++ UI/image pipeline reference

This comparison does not prove that `plugin/png` is sufficient for future LumaPix or VFX requirements.
It only establishes that the local U++ PNG path can encode/decode a small RGBA image successfully.
