# U++ plugin/z compatibility provider

The `GitHubOut` assembly places `third_party/codecs` before the installed U++
`uppsrc` nest. This package therefore resolves U++ Core's `plugin/z` dependency
to the repository's pinned `zlib_src` implementation on Windows. Its public
header forwards to the same 1.3.2 source header used by that implementation.

Keep the package name and header path: U++ Core includes
`<plugin/z/lib/zlib.h>` directly. Do not compile a second zlib source copy in
this package. Verify provider resolution with `zlib_test` and its build log
after changing the assembly or U++ installation.
