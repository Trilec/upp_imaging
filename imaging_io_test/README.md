# ImagingIO public test

Exercises the public backend-neutral EXR and PNG API without including
OpenImageIO headers.

Windows expected total: 89/0. Includes exclusive same-directory reservations,
stale temporary/backup preservation, existing-target replacement, failed native
promotion and a destination held without delete sharing, plus a payload spanning
multiple verification batches. --save-worker PATH performs twelve saves for the
external concurrent-process stress check. Non-Windows builds omit the locked-file
check and expect 88/0.
