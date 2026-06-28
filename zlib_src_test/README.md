# zlib_src_test

`zlib_src_test` is the strict linkage proof for `zlib_src`.

It intentionally avoids `Core` so the test can prove the imported upstream zlib 1.3.2 objects are the ones that get linked.
