#ifndef MZ_CONFIG_H
#define MZ_CONFIG_H

// Repository-generated configuration for Windows CLANGx64; not upstream source.
#ifndef _WIN32
#error minizip-ng requires a separately validated configuration on this platform
#endif
#define HAVE_ZLIB 1
#define ZLIB_COMPAT 1
#define HAVE_DIRENT_H 0
#define HAVE_SYS_DIRENT_H 0
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_PDIR 0
#define HAVE_FSEEKO 0
#define HAVE_SYMLINK 0
#define HAVE_READLINK 0

#endif
