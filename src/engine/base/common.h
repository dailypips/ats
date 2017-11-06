/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
  /* Windows - set up dll import/export decorators. */
# if defined(BUILDING_SHARED)
    /* Building shared library. */
#   define EXTERN __declspec(dllexport)
# elif defined(USING_SHARED)
    /* Using shared library. */
#   define EXTERN __declspec(dllimport)
# else
    /* Building static library. */
#   define EXTERN /* nothing */
# define HIDDEN
# endif
#elif __GNUC__ >= 4
# define EXTERN __attribute__((visibility("default")))
# define HIDDEN __attribute__ ((visibility ("hidden")))
#else
# define EXTERN /* nothing */
#endif

#include <stddef.h>

#if defined(_MSC_VER) && _MSC_VER < 1600
# include "stdint-msvc2008.h"
#else
# include <stdint.h>
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include <assert.h>
#define ASSERT(x) assert((x))
#define PRINT_FOMAT_CHECK(x,y)

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))


#ifndef UNUSED
#define UNUSED(x)	((void)(x))	/* to avoid warnings */
#endif


#if defined(__GNUC__)

#define NORET	__attribute__((noreturn))
#define ALIGN(n)	__attribute__((aligned(n)))
#define INLINE	inline
#define AINLINE	inline __attribute__((always_inline))
#define NOINLINE	__attribute__((noinline))

#define LIKELY(x)	__builtin_expect(!!(x), 1)
#define UNLIKELY(x)	__builtin_expect(!!(x), 0)

#elif defined(_MSC_VER)

#define NORET	__declspec(noreturn)
#define ALIGN(n)	__declspec(align(n))
#define INLINE	__inline
#define AINLINE	__forceinline
#define NOINLINE	__declspec(noinline)

#else
#error "missing defines for your compiler"
#endif

#ifndef LIKELY
#define LIKELY(x)	(x)
#define UNLIKELY(x)	(x)
#endif

/* Optional defines. */
#ifndef NORET
#define NORET
#endif
#ifndef NOAPI
#define NOAPI	extern
#endif
#if defined(AMALG_C)
#define FUNC		static
#else
#define FUNC		NOAPI
#endif

/* Static assertions. */
#define ASSERT_NAME2(name, line)	name ## line
#define ASSERT_NAME(line)		ASSERT_NAME2(assert_, line)
#ifdef __COUNTER__
#define STATIC_ASSERT(cond) \
  extern void ASSERT_NAME(__COUNTER__)(int STATIC_ASSERTION_FAILED[(cond)?1:-1])
#else
#define STATIC_ASSERT(cond) \
  extern void ASSERT_NAME(__LINE__)(int STATIC_ASSERTION_FAILED[(cond)?1:-1])
#endif


#ifdef __cplusplus
}
#endif

#endif // __COMMON_H__
