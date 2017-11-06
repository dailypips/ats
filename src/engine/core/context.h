/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef LOOP_H
#define LOOP_H

#include <uv.h>
#include "mpscq.h"
#include "event_bus.h"
#include "event_dispatch.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ctx_s  ctx_t;
typedef struct async_s async_t;
typedef void (*async_cb)(ctx_t* ctx, void* data);

/* context module */
struct async_s {
    async_cb async_cb;
    void *data;
    /* private */
    atomic_t next;
};

typedef enum {
    RUN_DEFAULT = 0,
    RUN_ONCE,
    RUN_NOWAIT
} ctx_run_mode;

struct ctx_s {
    uv_mutex_t wait_mutex;
    uv_cond_t wait_cond;
    int stop_flag;
    mpscq_t async_queue;
    EVENT_BUS_FIELDS
    DISPATCHER_FIELDS
};

FUNC void ctx_init(ctx_t *ctx);
FUNC void ctx_destroy(ctx_t *ctx);

FUNC void ctx_run(ctx_t *ctx, ctx_run_mode mode);
FUNC void ctx_stop(ctx_t *ctx);

FUNC void ctx_wakeup(ctx_t *ctx);

FUNC void ctx_async_post(ctx_t *ctx, async_cb cb, void *user_data);

#ifdef __cplusplus
}
#endif

#endif // LOOP_H
