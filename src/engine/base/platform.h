#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/* Get windows.h included everywhere (we need it) */
#if defined(_WIN64) || defined(WIN64) || defined(_WIN32) || defined(WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define GRPC_WIN32_LEAN_AND_MEAN_WAS_NOT_DEFINED
#define WIN32_LEAN_AND_MEAN
#endif /* WIN32_LEAN_AND_MEAN */

#ifndef NOMINMAX
#define GRPC_NOMINMX_WAS_NOT_DEFINED
#define NOMINMAX
#endif /* NOMINMAX */

#ifndef _WIN32_WINNT
#error \
    "Please compile grpc with _WIN32_WINNT of at least 0x600 (aka Windows Vista)"
#else /* !defined(_WIN32_WINNT) */
#if (_WIN32_WINNT < 0x0600)
#error \
    "Please compile grpc with _WIN32_WINNT of at least 0x600 (aka Windows Vista)"
#endif /* _WIN32_WINNT < 0x0600 */
#endif /* defined(_WIN32_WINNT) */

#include <windows.h>

#ifdef GRPC_WIN32_LEAN_AND_MEAN_WAS_NOT_DEFINED
#undef GRPC_WIN32_LEAN_AND_MEAN_WAS_NOT_DEFINED
#undef WIN32_LEAN_AND_MEAN
#endif /* GRPC_WIN32_LEAN_AND_MEAN_WAS_NOT_DEFINED */

#ifdef GRPC_NOMINMAX_WAS_NOT_DEFINED
#undef GRPC_NOMINMAX_WAS_NOT_DEFINED
#undef NOMINMAX
#endif /* GRPC_WIN32_LEAN_AND_MEAN_WAS_NOT_DEFINED */
#endif /* defined(_WIN64) || defined(WIN64) || defined(_WIN32) || \
          defined(WIN32) */

/* Override this file with one for your platform if you need to redefine
   things.  */

#if !defined(NO_AUTODETECT_PLATFORM)
#if defined(_WIN64) || defined(WIN64) || defined(_WIN32) || defined(WIN32)
#if defined(_WIN64) || defined(WIN64)
#define ARCH_64 1
#else
#define ARCH_32 1
#endif
#define PLATFORM_STRING "windows"
#define WINDOWS 1
#define WINDOWS_SUBPROCESS 1
#define WINDOWS_ENV
#ifdef __MSYS__
#define GETPID_IN_UNISTD_H 1
#define MSYS_TMPFILE
#define POSIX_LOG
#define POSIX_STRING
#define POSIX_TIME
#else
#define GETPID_IN_PROCESS_H 1
#define WINDOWS_TMPFILE
#define WINDOWS_LOG
#define WINDOWS_CRASH_HANDLER 1
#define WINDOWS_STRING
#define WINDOWS_TIME
#endif
#ifdef __GNUC__
#define GCC_ATOMIC 1
#define GCC_TLS 1
#else
#define WINDOWS_ATOMIC 1
#define MSVC_TLS 1
#endif
#elif defined(MANYLINUX1)
// TODO(atash): manylinux1 is just another __lin_ but with ancient
// libraries; it should be integrated with the `__lin_` definitions below.
#define PLATFORM_STRING "manylinux"
#define POSIX_CRASH_HANDLER 1
#define CPU_POSIX 1
#define GCC_ATOMIC 1
#define GCC_TLS 1
#define LINUX 1
#define LINLOG 1
#define SUPPORT_CHANNELS_FROM_FD 1
#define LINENV 1
#define POSIX_TMPFILE 1
#define POSIX_STRING 1
#define POSIX_SUBPROCESS 1
#define POSIX_SYNC 1
#define POSIX_TIME 1
#define GETPID_IN_UNISTD_H 1
#ifdef _LP64
#define ARCH_64 1
#else /* _LP64 */
#define ARCH_32 1
#endif /* _LP64 */
#elif defined(ANDROID) || defined(__ANDROID__)
#define PLATFORM_STRING "android"
#define ANDROID 1
#ifdef _LP64
#define ARCH_64 1
#else /* _LP64 */
#define ARCH_32 1
#endif /* _LP64 */
#define CPU_POSIX 1
#define GCC_SYNC 1
#define GCC_TLS 1
#define POSIX_ENV 1
#define POSIX_TMPFILE 1
#define ANDROID_LOG 1
#define POSIX_STRING 1
#define POSIX_SUBPROCESS 1
#define POSIX_SYNC 1
#define POSIX_TIME 1
#define GETPID_IN_UNISTD_H 1
#define SUPPORT_CHANNELS_FROM_FD 1
#elif defined(__lin_)
#define PLATFORM_STRING "linux"
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <features.h>
#define CPU_LINUX 1
#define GCC_ATOMIC 1
#define GCC_TLS 1
#define LINUX 1
#define LINLOG
#define SUPPORT_CHANNELS_FROM_FD 1
#define LINENV 1
#define POSIX_TMPFILE 1
#define POSIX_STRING 1
#define POSIX_SUBPROCESS 1
#define POSIX_SYNC 1
#define POSIX_TIME 1
#define GETPID_IN_UNISTD_H 1
#ifdef _LP64
#define ARCH_64 1
#else /* _LP64 */
#define ARCH_32 1
#endif /* _LP64 */
#ifdef __GLIBC__
#define POSIX_CRASH_HANDLER 1
#else /* musl libc */
#define MUSL_LIBC_COMPAT 1
#endif
#elif defined(__APPLE__)
#include <Availability.h>
#include <TargetConditionals.h>
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#if TARGET_OS_IPHONE
#define FORBID_UNREACHABLE_CODE 1
#define PLATFORM_STRING "ios"
#define CPU_IPHONE 1
#define PTHREAD_TLS 1
#else /* TARGET_OS_IPHONE */
#define PLATFORM_STRING "osx"
#ifdef __MAC_OS_X_VERSION_MIN_REQUIRED
#if __MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_7
#define CPU_IPHONE 1
#define PTHREAD_TLS 1
#else /* __MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_7 */
#define CPU_POSIX 1
#define GCC_TLS 1
#endif
#else /* __MAC_OS_X_VERSION_MIN_REQUIRED */
#define CPU_POSIX 1
#define GCC_TLS 1
#endif
#define POSIX_CRASH_HANDLER 1
#endif
#define APPLE 1
#define GCC_ATOMIC 1
#define POSIX_LOG 1
#define POSIX_ENV 1
#define POSIX_TMPFILE 1
#define POSIX_STRING 1
#define POSIX_SUBPROCESS 1
#define POSIX_SYNC 1
#define POSIX_TIME 1
#define GETPID_IN_UNISTD_H 1
#define SUPPORT_CHANNELS_FROM_FD 1
#ifdef _LP64
#define ARCH_64 1
#else /* _LP64 */
#define ARCH_32 1
#endif /* _LP64 */
#elif defined(__FreeBSD__)
#define PLATFORM_STRING "freebsd"
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#define FREEBSD 1
#define CPU_POSIX 1
#define GCC_ATOMIC 1
#define GCC_TLS 1
#define POSIX_LOG 1
#define POSIX_ENV 1
#define POSIX_TMPFILE 1
#define POSIX_STRING 1
#define POSIX_SUBPROCESS 1
#define POSIX_SYNC 1
#define POSIX_TIME 1
#define GETPID_IN_UNISTD_H 1
#define SUPPORT_CHANNELS_FROM_FD 1
#ifdef _LP64
#define ARCH_64 1
#else /* _LP64 */
#define ARCH_32 1
#endif /* _LP64 */
#elif defined(__native_client__)
#define PLATFORM_STRING "nacl"
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define NACL 1
#define CPU_POSIX 1
#define GCC_ATOMIC 1
#define GCC_TLS 1
#define POSIX_LOG 1
#define POSIX_ENV 1
#define POSIX_TMPFILE 1
#define POSIX_STRING 1
#define POSIX_SUBPROCESS 1
#define POSIX_SYNC 1
#define POSIX_TIME 1
#define GETPID_IN_UNISTD_H 1
#ifdef _LP64
#define ARCH_64 1
#else /* _LP64 */
#define ARCH_32 1
#endif /* _LP64 */
#else
#error "Could not auto-detect platform"
#endif
#endif /* NO_AUTODETECT_PLATFORM */

#if defined(__has_include)
#if __has_include(<atomic>)
#define GRPC_HAS_CXX11_ATOMIC
#endif /* __has_include(<atomic>) */
#endif /* defined(__has_include) */

#ifndef PLATFORM_STRING
#warning "PLATFORM_STRING not auto-detected"
#define PLATFORM_STRING "unknown"
#endif

#ifdef GCOV
#undef FORBID_UNREACHABLE_CODE
#define FORBID_UNREACHABLE_CODE 1
#endif

#ifdef _MSC_VER
#ifdef _PYTHON_MSVC
// The Python 3.5 Windows runtime is missing InetNtop
#define WIN_INET_NTOP
#endif  // _PYTHON_MSVC
#if _MSC_VER < 1700
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif /* _MSC_VER < 1700 */
#else
#include <stdint.h>
#endif /* _MSC_VER */

/* Cache line alignment */
#ifndef CACHELINE_SIZE_LOG
#if defined(__i386__) || defined(__x86_64__)
#define CACHELINE_SIZE_LOG 6
#endif
#ifndef CACHELINE_SIZE_LOG
/* A reasonable default guess. Note that overestimates tend to waste more
   space, while underestimates tend to waste more time. */
#define CACHELINE_SIZE_LOG 6
#endif /* CACHELINE_SIZE_LOG */
#endif /* CACHELINE_SIZE_LOG */

#define CACHELINE_SIZE (1 << CACHELINE_SIZE_LOG)

/* scrub GCC_ATOMIC if it's not available on this compiler */
#if defined(GCC_ATOMIC) && !defined(__ATOMIC_RELAXED)
#undef GCC_ATOMIC
#define GCC_SYNC 1
#endif

/* Validate platform combinations */
#if defined(GCC_ATOMIC) + defined(GCC_SYNC) + \
        defined(WINDOWS_ATOMIC) !=                \
    1
#error Must define exactly one of GCC_ATOMIC, GCC_SYNC, WINDOWS_ATOMIC
#endif

#if defined(ARCH_32) + defined(ARCH_64) != 1
#error Must define exactly one of ARCH_32, ARCH_64
#endif

#if defined(CPU_LINUX) + defined(CPU_POSIX) + defined(WINDOWS) + \
        defined(CPU_IPHONE) + defined(CPU_CUSTOM) !=                 \
    1
#error Must define exactly one of CPU_LINUX, CPU_POSIX, WINDOWS, CPU_IPHONE, CPU_CUSTOM
#endif

#if defined(MSVC_TLS) + defined(GCC_TLS) + defined(PTHREAD_TLS) + \
        defined(CUSTOM_TLS) !=                                            \
    1
#error Must define exactly one of MSVC_TLS, GCC_TLS, PTHREAD_TLS, CUSTOM_TLS
#endif

/* maximum alignment needed for any type on this platform, rounded up to a
   power of two */
#define MAX_ALIGNMENT 16

#ifndef GRPC_ARES
#ifdef WINDOWS
#define GRPC_ARES 0
#else
#define GRPC_ARES 1
#endif
#endif

#ifndef GRPC_MUST_USE_RESULT
#if defined(__GNUC__) && !defined(__MINGW32__)
#define GRPC_MUST_USE_RESULT __attribute__((warn_unused_result))
#define ALIGN_STRUCT(n) __attribute__((aligned(n)))
#else
#define GRPC_MUST_USE_RESULT
#define ALIGN_STRUCT(n)
#endif
#endif

#ifndef PRINT_FORMAT_CHECK
#ifdef __GNUC__
#define PRINT_FORMAT_CHECK(FORMAT_STR, ARGS) \
  __attribute__((format(printf, FORMAT_STR, ARGS)))
#else
#define PRINT_FORMAT_CHECK(FORMAT_STR, ARGS)
#endif
#endif /* PRINT_FORMAT_CHECK */

#if FORBID_UNREACHABLE_CODE
#define UNREACHABLE_CODE(STATEMENT)
#else
#define UNREACHABLE_CODE(STATEMENT)             \
  do {                                              \
    log(ERROR, "Should never reach here."); \
    abort();                                        \
    STATEMENT;                                      \
  } while (0)
#endif /* FORBID_UNREACHABLE_CODE */

#ifndef UXAPI
#define UXAPI
#endif

#ifndef GRPCAPI
#define GRPCAPI UXAPI
#endif

#ifndef CENSUSAPI
#define CENSUSAPI GRPCAPI
#endif

#ifndef ATTRIBUTE_NO_TSAN /* (1) */
#if defined(__has_feature)
#if __has_feature(thread_sanitizer)
#define ATTRIBUTE_NO_TSAN __attribute__((no_sanitize("thread")))
#endif                        /* __has_feature(thread_sanitizer) */
#endif                        /* defined(__has_feature) */
#ifndef ATTRIBUTE_NO_TSAN /* (2) */
#define ATTRIBUTE_NO_TSAN
#endif /* ATTRIBUTE_NO_TSAN (2) */
#endif /* ATTRIBUTE_NO_TSAN (1) */

#endif /* __PLATFORM_H__ */
