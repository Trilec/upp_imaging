# webp_imagingio_test

Focused backend-neutral WebP contract for `ImagingIO`.

The test proves static zero-origin UInt8 RGB/RGBA exact-lossless roundtrips, source immutability, hidden transparent RGB preservation, and fail-closed behavior for UInt16, Gray, non-zero-origin and malformed inputs. Failed loads preserve the caller's prior `ImageData`, and no temporary/backup transaction residue may remain.

Animated WebP is outside the initial framework slice and is covered by Windows acceptance with a real animated fixture; the shared structure inspector must reject its additional subimage/frame.

Expected summary: `passed=21 failed=0`.
