/*
 * Repository-owned generated-equivalent FFmpeg 9.0.1 configuration.
 * Target: Windows x86_64, U++ CLANGx64, static scalar decode-first build.
 */
#ifndef UPP_FFMPEG_CONFIG_H
#define UPP_FFMPEG_CONFIG_H

#define FFMPEG_CONFIGURATION "--disable-everything --disable-autodetect --disable-programs --disable-doc --disable-network --disable-asm --disable-inline-asm --disable-pthreads --disable-w32threads --disable-avdevice --disable-avfilter --disable-swresample --enable-avcodec --enable-avformat --enable-swscale --enable-decoder=h264 --enable-demuxer=mov --enable-protocol=file"
#define FFMPEG_LICENSE "LGPL version 2.1 or later"
#define CONFIG_THIS_YEAR 2026
#define FFMPEG_DATADIR "."
#define AVCONV_DATADIR "."
#define CC_IDENT "clang (U++ CLANGx64)"
#define OS_NAME "win64"
#define EXTERN_PREFIX ""
#define EXTERN_ASM
#define BUILDSUF ""
#define SLIBSUF ".dll"
#define SWS_MAX_FILTER_SIZE 256

#define ARCH_AARCH64 0
#define ARCH_ARM 0
#define ARCH_IA64 0
#define ARCH_LOONGARCH 0
#define ARCH_LOONGARCH32 0
#define ARCH_LOONGARCH64 0
#define ARCH_MIPS 0
#define ARCH_MIPS64 0
#define ARCH_PARISC 0
#define ARCH_PARISC64 0
#define ARCH_PPC 0
#define ARCH_PPC64 0
#define ARCH_RISCV 0
#define ARCH_RISCV32 0
#define ARCH_RISCV64 0
#define ARCH_S390 0
#define ARCH_SH4 0
#define ARCH_SPARC 0
#define ARCH_SPARC64 0
#define ARCH_TILEGX 0
#define ARCH_TILEPRO 0
#define ARCH_WASM 0
#define ARCH_X86 1
#define ARCH_X86_32 0
#define ARCH_X86_64 1

#define CONFIG_STATIC 1
#define CONFIG_SHARED 0
#define CONFIG_GPL 0
#define CONFIG_NONFREE 0
#define CONFIG_VERSION3 0
#define CONFIG_SMALL 0
#define CONFIG_RUNTIME_CPUDETECT 0
#define CONFIG_NETWORK 0
#define CONFIG_MEMORY_POISONING 0
#define CONFIG_PIXELUTILS 0
#define CONFIG_HARDCODED_TABLES 1
#define CONFIG_GRAY 0
#define CONFIG_SWSCALE_ALPHA 1

#define CONFIG_AVUTIL 1
#define CONFIG_AVCODEC 1
#define CONFIG_AVFORMAT 1
#define CONFIG_SWSCALE 1
#define CONFIG_SWRESAMPLE 0
#define CONFIG_AVFILTER 0
#define CONFIG_AVDEVICE 0

#define CONFIG_H264_DECODER 1
#define CONFIG_MOV_DEMUXER 1
#define CONFIG_FILE_PROTOCOL 1
#define CONFIG_CABAC 1
#define CONFIG_GOLOMB 1
#define CONFIG_H264CHROMA 1
#define CONFIG_H264DSP 1
#define CONFIG_H264PARSE 1
#define CONFIG_H264PRED 1
#define CONFIG_H264QPEL 1
#define CONFIG_H264_SEI 1
#define CONFIG_VIDEODSP 1
#define CONFIG_ISO_MEDIA 1
#define CONFIG_RIFFDEC 1
#define CONFIG_ERROR_RESILIENCE 0

#define CONFIG_GCRYPT 0
#define CONFIG_OPENSSL 0
#define CONFIG_ZLIB 0
#define CONFIG_BZLIB 0
#define CONFIG_LZMA 0
#define CONFIG_ICONV 0
#define CONFIG_VULKAN 0
#define CONFIG_CUDA 0
#define CONFIG_D3D11VA 0
#define CONFIG_D3D12VA 0
#define CONFIG_DXVA2 0
#define CONFIG_AMF 0
#define CONFIG_QSV 0
#define CONFIG_VAAPI 0
#define CONFIG_VDPAU 0
#define CONFIG_VIDEOTOOLBOX 0
#define CONFIG_MEDIACODEC 0
#define CONFIG_OPENCL 0
#define CONFIG_OHCODEC 0

#define HAVE_BIGENDIAN 0
#define HAVE_FAST_64BIT 1
#define HAVE_FAST_CLZ 1
#define HAVE_FAST_UNALIGNED 1
#define HAVE_SIMD_ALIGN_16 0
#define HAVE_SIMD_ALIGN_32 0
#define HAVE_SIMD_ALIGN_64 0

#define HAVE_X86ASM 0
#define HAVE_INLINE_ASM 0
#define HAVE_MMX_INLINE 0
#define HAVE_MMX_EXTERNAL 0
#define HAVE_MM_EMPTY 0
#define HAVE_SSE 0
#define HAVE_AVX 0
#define HAVE_AVX2 0
#define HAVE_AVX512 0
#define HAVE_AVX512ICL 0

#define HAVE_ALIGNED_MALLOC 1
#define HAVE_POSIX_MEMALIGN 0
#define HAVE_MEMALIGN 0
#define HAVE_MALLOC_H 1
#define HAVE_IO_H 1
#define HAVE_UNISTD_H 0
#define HAVE_WINDOWS_H 1
#define HAVE_MAPVIEWOFFILE 1
#define HAVE_MMAP 0
#define HAVE_FCNTL 0
#define HAVE_MKSTEMP 0
#define HAVE_TEMPNAM 0

#define HAVE_GETSYSTEMTIMEASFILETIME 1
#define HAVE_GETTIMEOFDAY 0
#define HAVE_CLOCK_GETTIME 0
#define HAVE_NANOSLEEP 0
#define HAVE_USLEEP 0
#define HAVE_SLEEP 1

#define HAVE_PTHREADS 0
#define HAVE_W32THREADS 0
#define HAVE_OS2THREADS 0
#define HAVE_THREADS 0
#define HAVE_SCHED_GETAFFINITY 0
#define HAVE_GETPROCESSAFFINITYMASK 0
#define HAVE_SYSCTL 0
#define HAVE_GETAUXVAL 0
#define HAVE_ELF_AUX_INFO 0

#define HAVE_BCRYPT 0
#define HAVE_ARC4RANDOM_BUF 0
#define HAVE_DOS_PATHS 1
#define HAVE_STRUCT_ADDRINFO 1
#define HAVE_STRUCT_SOCKADDR_STORAGE 1
#define HAVE_STDATOMIC_H 1

#endif /* UPP_FFMPEG_CONFIG_H */
