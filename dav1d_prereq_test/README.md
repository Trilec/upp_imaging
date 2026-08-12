# dav1d_prereq_test

Focused scalar AV1 decoder contract for repository-pinned dav1d 1.5.4.

The test verifies runtime version/API identity, deterministic default settings, one-thread scalar context construction, malformed sequence-header rejection and clean context shutdown. It requires no AVIF fixture.

Expected summary: `passed=6 failed=0`.
