// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Contributors to the OpenEXR Project.

#ifndef INCLUDED_OPENEXR_INTERNAL_CONFIG_H
#define INCLUDED_OPENEXR_INTERNAL_CONFIG_H 1

#pragma once

/* Current narrow package attempt uses external libdeflate package. */
/* #undef OPENEXR_USE_INTERNAL_DEFLATE */

/* Test-suite-only or platform probes are intentionally off in this package attempt. */
/* #undef OPENEXR_IMF_HAVE_LINUX_PROCFS */
/* #undef OPENEXR_IMF_HAVE_DARWIN */
#define OPENEXR_IMF_HAVE_COMPLETE_IOMANIP 1
#define OPENEXR_IMF_HAVE_SYSCONF_NPROCESSORS_ONLN 1
#define OPENEXR_IMF_HAVE_GCC_INLINE_ASM_AVX 1
#define OPENEXR_MISSING_ARM_VLD1 0

#endif
