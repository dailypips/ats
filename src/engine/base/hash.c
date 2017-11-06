/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#include "hash.h"
#include "common.h"
#include <assert.h>

__KHASH_IMPL(int, kh_inline klib_unused, khint32_t, void *, 1, kh_int_hash_func,
             kh_int_hash_equal)
__KHASH_IMPL(str, kh_inline klib_unused, kh_cstr_t, void *, 1, kh_str_hash_func,
             kh_str_hash_equal)
__KHASH_IMPL(ilist, kh_inline klib_unused, khint32_t, ats_list_t, 1,
             kh_int_hash_func, kh_int_hash_equal)
__KHASH_IMPL(slist, kh_inline klib_unused, kh_cstr_t, ats_list_t, 1,
             kh_str_hash_func, kh_str_hash_equal)

void *int_hash_get(ats_ihash_t *hash, khint32_t key) {
  khint_t iter = kh_get_int(hash, key);
  if (iter == kh_end(hash))
    return NULL;
  return kh_value(hash, iter);
}

void *str_hash_get(ats_shash_t *hash, const char *key) {
  khint_t iter = kh_get_str(hash, key);
  if (iter == kh_end(hash))
    return NULL;
  return kh_value(hash, iter);
}

void int_hash_set(ats_ihash_t *hash, khint32_t key, void *val) {
  int ret;
  khint_t iter = kh_put_int(hash, key, &ret);
  ASSERT(ret > 0);
  kh_value(hash, iter) = val;
}

void str_hash_set(ats_shash_t *hash, const char *key, void *val) {
  int ret;
  khint_t iter = kh_put_str(hash, key, &ret);
  ASSERT(ret > 0);
  kh_value(hash, iter) = val;
}

ats_list_t *int_hash_get_list(ats_list_ihash_t *hash, khint32_t key) {
  khint_t iter = kh_get_ilist(hash, key);
  if (iter == kh_end(hash)) {
    return NULL;
  }
  return (ats_list_t *)(&(kh_value(hash, iter)));
}

ats_list_t *str_hash_get_list(ats_list_shash_t *hash, const char *key) {
  khint_t iter = kh_get_slist(hash, key);
  if (iter == kh_end(hash)) {
    return NULL;
  }
  return (ats_list_t *)(&(kh_value(hash, iter)));
}

ats_list_t *int_hash_get_or_create_list(ats_list_ihash_t *hash,
                                          khint32_t key) {
  ats_list_t *list;
  khint_t iter;
  int ret;

  iter = kh_get_ilist(hash, key);
  if (iter == kh_end(hash)) {
    iter = kh_put_ilist(hash, key, &ret);
    ASSERT(ret > 0);
    list = &(kh_value(hash, iter));
    QUEUE_INIT(&list->queue);
    return list;
  }
  list = &(kh_value(hash, iter));
  return list;
}

ats_list_t *str_hash_get_or_create_list(ats_list_shash_t *hash,
                                          const char *key) {
  ats_list_t *list;
  khint_t iter;
  int ret;

  iter = kh_get_slist(hash, key);
  if (iter == kh_end(hash)) {
    iter = kh_put_slist(hash, key, &ret);
    ASSERT(ret > 0);
    list = &(kh_value(hash, iter));
    QUEUE_INIT(&list->queue);
    return list;
  }
  list = &(kh_value(hash, iter));
  return list;
}
