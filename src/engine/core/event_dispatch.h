/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef EVENT_DISPATCH_H
#define EVENT_DISPATCH_H

#include "common.h"
#include "event.h"
#include "queue.h"
#include "spscq.h"
#include "logger.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*event_handle)(void *ctx, event_t *);
typedef event_t* (*filter_event)(event_t *);

#define DISPATCHER_FIELDS \
    event_handle event_handle_table[EVENT_LAST]; \
    filter_event   event_filter;  \
    long dispatch_reentrant_count;  \
    spscq_t buffed_event_queue;


#define DISPATCHER_INIT(ctx)         dispatcher_init((ctx))
#define DISPATCHER_DESTROY(ctx)      dispatcher_destroy((ctx))

FUNC void dispatcher_init(ctx_t *ctx);
FUNC void dispatcher_destroy(ctx_t* ctx);

FUNC event_handle dispatcher_set_event_dispatch(ctx_t *ctx, event_type type, event_handle dispatch);
FUNC filter_event dispatcher_set_event_filter(ctx_t *ctx, filter_event filter);

FUNC void dispatch_event(ctx_t *ctx, event_t *e);
FUNC void delay_dispatch_event(ctx_t *ctx, event_t *e);
FUNC void emit_delayed_events(ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif // EVENT_DISPATCH_H
