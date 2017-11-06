/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* memory module */
typedef void* (*ats_malloc_func)(size_t size);
typedef void* (*ats_realloc_func)(void* ptr, size_t size);
typedef void* (*ats_calloc_func)(size_t count, size_t size);
typedef void (*ats_free_func)(void* ptr);

EXTERN int ats_replace_allocator(ats_malloc_func malloc_func,
                                   ats_realloc_func realloc_func,
                                   ats_calloc_func calloc_func,
                                   ats_free_func free_func);

EXTERN void* ats_malloc(size_t size);
EXTERN void* ats_calloc(size_t count, size_t size);
EXTERN void* ats_zalloc(size_t size);
EXTERN void  ats_free(void *ptr);
EXTERN void* ats_realloc(void *ptr, size_t size);
EXTERN void* ats_malloc_aligned(size_t size, size_t alignment);
EXTERN char *ats_strdup(const char *src);
EXTERN char* ats_strndup(const char* s, size_t n);

#ifdef __cplusplus
}
#endif

#endif // __MEMORY_H__
