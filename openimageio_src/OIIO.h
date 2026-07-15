#pragma once

#ifndef UPP_IMAGING_LOCAL_OPENIMAGEIO_SOURCE_INCLUDE
#define UPP_IMAGING_LOCAL_OPENIMAGEIO_SOURCE_INCLUDE 1
#endif

#ifndef UPP_IMAGING_LOCAL_OPENIMAGEIO_UTIL_SOURCE_INCLUDE
#define UPP_IMAGING_LOCAL_OPENIMAGEIO_UTIL_SOURCE_INCLUDE 1
#endif

#ifndef OIIO_STATIC_DEFINE
#define OIIO_STATIC_DEFINE 1
#endif

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

#include <OpenImageIO/oiioversion.h>
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
#include <OpenImageIO/imagecache.h>
#include <OpenImageIO/color.h>
#include <OpenImageIO/deepdata.h>
#include <OpenImageIO/texture.h>
