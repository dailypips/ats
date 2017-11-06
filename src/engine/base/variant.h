/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef VARIANT_H
#define VARIANT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup msgpack_object Dynamically typed object
 * @ingroup msgpack
 * @{
 */

typedef enum {
    VARIANT_TYPE_NIL                  = 0x00,
    VARIANT_TYPE_BOOLEAN              = 0x01,
    VARIANT_TYPE_POSITIVE_INTEGER     = 0x02,
    VARIANT_TYPE_NEGATIVE_INTEGER     = 0x03,
    VARIANT_TYPE_FLOAT32              = 0x0a,
    VARIANT_TYPE_FLOAT64              = 0x04,
    VARIANT_TYPE_FLOAT                = 0x04,
    VARIANT_TYPE_STR                  = 0x05,
    VARIANT_TYPE_ARRAY                = 0x06,
    VARIANT_TYPE_MAP                  = 0x07,
    VARIANT_TYPE_BIN                  = 0x08,
    VARIANT_TYPE_EXT                  = 0x09
} variant_type;


struct variant_s;
struct variant_kv;

typedef struct {
    uint32_t size;
    struct variant_s* ptr;
} variant_array;

typedef struct {
    uint32_t size;
    struct variant_kv* ptr;
} variant_map;

typedef struct {
    uint32_t size;
    const char* ptr;
} variant_str;

typedef struct {
    uint32_t size;
    const char* ptr;
} variant_bin;

typedef struct {
    int8_t type;
    uint32_t size;
    const char* ptr;
} variant_ext;

typedef union {
    bool boolean;
    uint64_t u64;
    int64_t  i64;
    double   f64;
    variant_array array;
    variant_map map;
    variant_str str;
    variant_bin bin;
    variant_ext ext;
} variant_union;

typedef struct variant_s {
    variant_type type;
    variant_union via;
} variant_t;

typedef struct variant_kv {
    variant_t key;
    variant_t val;
} variant_kv;

void variant_print(FILE* out, variant_t o);

int variant_print_buffer(char *buffer, size_t buffer_size, variant_t o);

bool variant_equal(const variant_t x, const variant_t y);

/** @} */


#ifdef __cplusplus
}
#endif

#endif // VARIANT_H
