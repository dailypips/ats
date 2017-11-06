/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#include "context.h"
#include "mem.h"

static void loop_init(ctx_t *ctx)
{
    uv_mutex_init(&ctx->wait_mutex);
    uv_cond_init(&ctx->wait_cond);
    ctx->stop_flag = 0;
    mpscq_init(&ctx->async_queue);
}

static void destroy_async_queue(ctx_t *ctx)
{
    mpscq_node* node;
    while ((node = mpscq_pop(&ctx->async_queue))) {
        async_t* async = container_of(node, async_t, next);
        ats_free(async);
    }
}

static void loop_destroy(ctx_t *ctx)
{
    destroy_async_queue(ctx);
    uv_cond_destroy(&ctx->wait_cond);
    uv_mutex_destroy(&ctx->wait_mutex);
}

void ctx_init(ctx_t *ctx)
{
    loop_init(ctx);

    EVENT_BUS_INIT(ctx);

    DISPATCHER_INIT(ctx);
}

void ctx_destroy(ctx_t *ctx)
{
    DISPATCHER_DESTROY(ctx);
    EVENT_BUS_DESTROY(ctx);

    loop_destroy(ctx);
}

void ctx_wakeup(ctx_t *ctx)
{
    uv_cond_signal(&ctx->wait_cond);
}

void ctx_async_post(ctx_t *ctx, async_cb cb, void *user_data)
{
    async_t *async = ats_malloc(sizeof(*async));
    async->async_cb = cb;
    async->data = user_data;

    mpscq_push(&ctx->async_queue, (mpscq_node *)&async->next);
    ctx_wakeup(ctx);
}

static int64_t bus_next_timeout(ctx_t *ctx) {
  reminder_event_t *r = bus_timer_peek(ctx, CLOCK_LOCAL);

  if (r)
    return r->timeout - datetime_now();

  return 0;
}

static inline void timewait(ctx_t *ctx, int64_t timeout) {
  uv_mutex_lock(&ctx->wait_mutex);
  if (timeout > 0)
    uv_cond_timedwait(&ctx->wait_cond, &ctx->wait_mutex, timeout);
  else
    uv_cond_wait(&ctx->wait_cond, &ctx->wait_mutex);
  uv_mutex_unlock(&ctx->wait_mutex);
}

void ctx_run(ctx_t *ctx, ctx_run_mode mode) {
  while (ctx->stop_flag == 0) {
    /* step 1: call all async cb */
    mpscq_node *node;
    while ((node = mpscq_pop(&ctx->async_queue))) {
      async_t *async_node = container_of(node, async_t, next);
      async_node->async_cb(ctx, async_node->data);
    }
    /* step 2: pop bus */
    event_t *e = bus_dequeue(ctx);

    if (e)
      dispatch_event(ctx, e);

    if (mode == RUN_NOWAIT || (mode == RUN_ONCE && e))
      break;

    int64_t timeout = bus_next_timeout(ctx);
    if (timeout > 0)
      timewait(ctx, timeout);
  }
}

void ctx_stop(ctx_t *ctx) { ctx->stop_flag = 1; }
