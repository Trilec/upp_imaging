# libheif_prereq_test

Focused decode-only container/backend contract for repository-pinned libheif 1.23.1.

The test verifies version identity, clean library initialization, built-in AV1 and HEVC decoder availability, deliberate absence of both encoder classes, HEIF context allocation and clean deinitialization.

Expected summary: `passed=8 failed=0`.
