/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#include "khash.h"
#include <uv.h>
#include "currency.h"

static uv_once_t currency_once_only = UV_ONCE_INIT;
KHASH_MAP_INIT_STR(map, currency_t)
khash_t(map) * currency_map;

static void put_key(const char *key, currency_t val, int *ret) {
  khint_t iter = kh_put(map, currency_map, key, ret);
  kh_val(currency_map, iter) = val;
}

static void init_hash_table() {
  int ret;
  currency_map = kh_init(map);
#define INIT_MAP(name, code) put_key(#name, code, &ret);
  CURRENCY_DEF(INIT_MAP)
#undef INIT_MAP
}

currency_t currency_from_name(const char *name) {
  uv_once(&currency_once_only, init_hash_table);
  khint_t iter = kh_get(map, currency_map, name);
  if (iter == kh_end(currency_map))
    return CURRENCY_NONE;
  return kh_val(currency_map, iter);
}

#define CURRENCY_NAME_GEN(name, _)                                          \
  case CURRENCY_##name:                                                     \
    return #name;
const char *currency_to_name(currency_t currency) {
  switch (currency) {
    CURRENCY_DEF(CURRENCY_NAME_GEN)
  default:
    break;
  }

  return "NONE";
}
#undef CURRENCY_NAME_GEN
