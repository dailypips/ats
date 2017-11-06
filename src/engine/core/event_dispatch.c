/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#include "event_dispatch.h"
#include "context.h"

void dispatcher_init(ctx_t *ctx)
{
    spscq_init(&ctx->buffed_event_queue, 102400);
}

void dispatcher_destroy(ctx_t* ctx)
{
    spscq_destroy(&ctx->buffed_event_queue);
}

event_handle dispatcher_set_event_dispatch(ctx_t *ctx, event_type type, event_handle dispatch)
{
    ASSERT(type < EVENT_LAST);
    event_handle handle = ctx->event_handle_table[type];
    ctx->event_handle_table[type] = dispatch;
    return handle;
}

filter_event dispatcher_set_event_filter(ctx_t *ctx, filter_event filter)
{
    filter_event handle = ctx->event_filter;
    ctx->event_filter = filter;
    return handle;
}

void dispatch_event(ctx_t *ctx, event_t *e)
{
    ctx->dispatch_reentrant_count++;

    // step 1: filter event

    if(ctx->event_filter)
        e = ctx->event_filter(e);

    if (e == NULL)
        return;

    // step 2: logger event

    // step 3: process event
    event_handle dispatcher = ctx->event_handle_table[e->event_type];
    if (dispatcher)
      dispatcher(ctx, e);

    // step4: dispatch to other

    // step5: unref
    event_unref(e);
    ctx->dispatch_reentrant_count--;
}

void delay_dispatch_event(ctx_t *ctx, event_t *e)
{
    spscq_push(&ctx->buffed_event_queue, e);
}

void emit_delayed_events(ctx_t *ctx) {
  event_t *e;
  while ((e = spscq_pop(&ctx->buffed_event_queue))) {
    dispatch_event(ctx, e);
  }
}
