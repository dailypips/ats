/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#include <stdlib.h> /* malloc */
#include <string.h> /* memset */

#if defined(_WIN32)
#include <malloc.h> /* malloc */
#endif

#include <assert.h>
#include <errno.h>
#include "mem.h"

typedef struct {
    ats_malloc_func local_malloc;
    ats_realloc_func local_realloc;
    ats_calloc_func local_calloc;
    ats_free_func local_free;
} ats__allocator_t;

static ats__allocator_t ats__allocator = {
    malloc,
    realloc,
    calloc,
    free,
};

char* ats_strdup(const char* s)
{
    assert(s != NULL);
    size_t len = strlen(s) + 1;
    char* m = ats_malloc(len);
    if (m == NULL)
        return NULL;
    return memcpy(m, s, len);
}

char* ats_strndup(const char* s, size_t n)
{
    assert(s != NULL);
    char* m;
    size_t len = strlen(s);
    if (n < len)
        len = n;
    m = ats_malloc(len + 1);
    if (m == NULL)
        return NULL;
    m[len] = '\0';
    return memcpy(m, s, len);
}

void* ats_malloc(size_t size)
{
    assert(size > 0);
    void *p =  ats__allocator.local_malloc(size);
    if (!p)
        abort();
    return p;
}

void ats_free(void* ptr)
{
    assert(ptr != NULL);
    int saved_errno;

    saved_errno = errno;
    ats__allocator.local_free(ptr);
    errno = saved_errno;
}

void* ats_calloc(size_t count, size_t size)
{
    assert(count * size > 0);
    return ats__allocator.local_calloc(count, size);
}

void* ats_realloc(void* ptr, size_t size)
{
    assert(ptr != NULL);
    assert(size > 0);
    return ats__allocator.local_realloc(ptr, size);
}

void* ats_zalloc(size_t size)
{
    assert(size > 0);
    void* p = ats_malloc(size);
    if (p)
        memset(p, '\0', size);
    return p;
}

int ats_replace_allocator(ats_malloc_func malloc_func,
    ats_realloc_func realloc_func,
    ats_calloc_func calloc_func,
    ats_free_func free_func)
{
    if (malloc_func == NULL || realloc_func == NULL || calloc_func == NULL || free_func == NULL) {
        return -1;
    }

    ats__allocator.local_malloc = malloc_func;
    ats__allocator.local_realloc = realloc_func;
    ats__allocator.local_calloc = calloc_func;
    ats__allocator.local_free = free_func;

    return 0;
}
