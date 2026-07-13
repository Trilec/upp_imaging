#pragma once

#ifndef OIIO_STATIC_DEFINE
#define OIIO_STATIC_DEFINE 1
#endif

#if defined(__has_include) && __has_include(<OpenImageIO/argparse.h>)
#include <OpenImageIO/argparse.h>
#include <OpenImageIO/benchmark.h>
#include <OpenImageIO/errorhandler.h>
#include <OpenImageIO/filter.h>
#include <OpenImageIO/filesystem.h>
#include <OpenImageIO/fmath.h>
#include <OpenImageIO/hash.h>
#include <OpenImageIO/optparser.h>
#include <OpenImageIO/parallel.h>
#include <OpenImageIO/paramlist.h>
#include <OpenImageIO/plugin.h>
#include <OpenImageIO/refcnt.h>
#include <OpenImageIO/span.h>
#include <OpenImageIO/string_view.h>
#include <OpenImageIO/strutil.h>
#include <OpenImageIO/strongparam.h>
#include <OpenImageIO/thread.h>
#include <OpenImageIO/timer.h>
#include <OpenImageIO/typedesc.h>
#include <OpenImageIO/ustring.h>
#include <OpenImageIO/vecparam.h>
#else
#include "upstream/OpenImageIO/argparse.h"
#include "upstream/OpenImageIO/benchmark.h"
#include "upstream/OpenImageIO/errorhandler.h"
#include "upstream/OpenImageIO/filter.h"
#include "upstream/OpenImageIO/filesystem.h"
#include "upstream/OpenImageIO/fmath.h"
#include "upstream/OpenImageIO/hash.h"
#include "upstream/OpenImageIO/optparser.h"
#include "upstream/OpenImageIO/parallel.h"
#include "upstream/OpenImageIO/paramlist.h"
#include "upstream/OpenImageIO/plugin.h"
#include "upstream/OpenImageIO/refcnt.h"
#include "upstream/OpenImageIO/span.h"
#include "upstream/OpenImageIO/string_view.h"
#include "upstream/OpenImageIO/strutil.h"
#include "upstream/OpenImageIO/strongparam.h"
#include "upstream/OpenImageIO/thread.h"
#include "upstream/OpenImageIO/timer.h"
#include "upstream/OpenImageIO/typedesc.h"
#include "upstream/OpenImageIO/ustring.h"
#include "upstream/OpenImageIO/vecparam.h"
#endif

#define UPP_IMAGING_LOCAL_OPENIMAGEIO_UTIL_SOURCE_INCLUDE 1
