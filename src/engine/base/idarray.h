/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef __ID_ARRAY_H__
#define __ID_ARRAY_H__

#include <assert.h>
#include <limits.h>
#include <string.h>
#include "mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/* idarray
 * fit for inc count from zero
 */
typedef struct idarray_s {
  size_t m;
  uintptr_t *a;
} idarray_t;

#define IDARRAY_MIN_SIZE 16
#define IDARRAY_MAX_SIZE INT_MAX /* don't set to UINT_MAX */

#define roundup32(x)                                                           \
  (--(x), (x) |= (x) >> 1, (x) |= (x) >> 2, (x) |= (x) >> 4, (x) |= (x) >> 8,  \
   (x) |= (x) >> 16, ++(x))

static inline void ida_init(idarray_t *v) {
  v->m = 0;
  v->a = NULL;
}

static inline void ida_destroy(idarray_t *v) { ats_free(v->a); }

static inline uintptr_t ida_get(idarray_t *v, size_t i) {
  assert(i < IDARRAY_MAX_SIZE);
  if (UNLIKELY(i >= v->m))
    return 0;
  return v->a[i];
}

static inline void ida_set(idarray_t *v, size_t i, uintptr_t val) {
  assert(i < IDARRAY_MAX_SIZE);
  if (UNLIKELY(v->m <= i)) {
    v->m = i + 1;
    v->m = v->m < IDARRAY_MIN_SIZE ? IDARRAY_MIN_SIZE : v->m;
    roundup32(v->m);
    v->a = (uintptr_t *)ats_realloc(v->a, sizeof(uintptr_t) * v->m);
    memset(v->a + i, '\0', (v->m - i) * sizeof(uintptr_t));
  }
  v->a[i] = val;
}

static inline void ida_clear(idarray_t *v) {
  ida_destroy(v);
  ida_init(v);
}

#define IDA_GET(v,i) (void*)ida_get((v), (i))
#define IDA_SET(v, i, val) ida_set((v), (i), (uintptr_t)(val))

#undef roundup32

#ifdef __cplusplus
}
#endif

#endif // __ID_ARRAY_H__
