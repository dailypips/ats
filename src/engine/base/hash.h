/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef __HASH_H__
#define __HASH_H__

#include "khash.h"
#include "queue.h"
#include "mem.h"

/* undefine the defaults */
#undef kcalloc
#undef kmalloc
#undef krealloc
#undef kfree

/* re-define, specifying alternate functions */
#define kcalloc(N, Z) ats_calloc(N, Z)
#define kmalloc(Z) ats_malloc(Z)
#define krealloc(P, Z) ats_realloc(P, Z)
#define kfree(P) ats_free(P)

typedef struct ats_list_s {
  void *queue[2];
} ats_list_t;

KHASH_DECLARE(int, khint32_t, void *)
KHASH_DECLARE(str, kh_cstr_t, void *)
KHASH_DECLARE(ilist, khint32_t, ats_list_t)
KHASH_DECLARE(slist, kh_cstr_t, ats_list_t)

typedef khash_t(int) ats_ihash_t;
typedef khash_t(str) ats_shash_t;
typedef khash_t(ilist) ats_list_ihash_t;
typedef khash_t(slist) ats_list_shash_t;

void *int_hash_get(ats_ihash_t *hash, khint32_t key);
void *str_hash_get(ats_shash_t *hash, const char *key);
void int_hash_set(ats_ihash_t *hash, khint32_t key, void *val);
void str_hash_set(ats_shash_t *hash, const char *key, void *val);
ats_list_t* int_hash_get_list(ats_list_ihash_t *hash, khint32_t key);
ats_list_t* str_hash_get_list(ats_list_shash_t *hash,  const char *key);
ats_list_t* int_hash_get_or_create_list(ats_list_ihash_t *hash, khint32_t key);
ats_list_t* str_hash_get_or_create_list(ats_list_shash_t *hash,  const char *key);

#endif // __HASH_H__
