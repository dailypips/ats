/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#include "datetime.h"
#include "mem.h"
#include "event_queue.h"
#include "heap-inl.h"
#include <stdio.h>
#include "event_bus.h"
#include "event_def.h"
#include "context.h"

#define LOG_OUT_OF_ORDER(x, y) printf("outoforder\n")

static int queue_less_than(const struct heap_node* ha, const struct heap_node* hb)
{
    ASSERT(ha != NULL);
    ASSERT(hb != NULL);

    const event_queue_t* qa = container_of(ha, event_queue_t, heap_node);
    const event_queue_t* qb = container_of(hb, event_queue_t, heap_node);

    event_t* a = event_queue_peek(qa);
    event_t* b = event_queue_peek(qb);

    ASSERT(a != NULL);
    ASSERT(b != NULL);

    if (a->timestamp < b->timestamp)
        return 1;
    if (b->timestamp < a->timestamp)
        return 0;

    if (qa->start_id < qb->start_id)
        return 1;
    if (qb->start_id < qa->start_id)
        return 0;

    return 0;
}

static int timer_less_than(const struct heap_node* ha,
    const struct heap_node* hb)
{
    ASSERT(ha != NULL);
    ASSERT(hb != NULL);

    const reminder_event_t* a;
    const reminder_event_t* b;

    a = container_of(ha, reminder_event_t, heap_node);
    b = container_of(hb, reminder_event_t, heap_node);

    if (a->timeout < b->timeout)
        return 1;
    if (b->timeout < a->timeout)
        return 0;

    if (a->start_id < b->start_id)
        return 1;
    if (b->start_id < a->start_id)
        return 0;

    return 0;
}

static AINLINE int event_is_tick(event_t* e)
{
    ASSERT(e != NULL);

    event_flag flag = e->event_flag;
    if (flag & EVENT_FLAG_TICK_MASK)
        return 1;
    else
        return 0;
}

static AINLINE int heap_is_empty(min_heap* heap)
{
    ASSERT(heap != NULL);

    return heap_min((const struct heap*)heap) == NULL;
}

static event_t* event_pop_form_pipe(ctx_t *ctx, queue_category category)
{
    ASSERT(ctx != NULL);

    struct heap_node* node = heap_min((const struct heap*)&ctx->queue_heap[category]);

    if (node == NULL)
        return NULL;

    event_queue_t* q = container_of(node, event_queue_t, heap_node);

    heap_remove((struct heap*)&ctx->queue_heap[category],
        (struct heap_node*)node,
        queue_less_than);

    event_t* e = event_queue_pop(q);

    if (!event_queue_is_empty(q)) {
        ctx_t *ctx = q->ctx;
        q->start_id = ctx->counter++;
        heap_insert((struct heap*)&ctx->queue_heap[category],
            (struct heap_node*)node,
            queue_less_than);
    }
    return e;
}

static INLINE reminder_event_t* timer_heap_peek(min_heap* heap)
{
    struct heap_node* node = heap_min((const struct heap*)heap);

    if (node == NULL)
        return NULL;

    reminder_event_t* timer = container_of(node, reminder_event_t, heap_node);

    return timer;
}

static reminder_event_t* timer_heap_pop(min_heap* heap)
{
    struct heap_node* node = heap_min((const struct heap*)heap);

    if (node == NULL)
        return NULL;

    reminder_event_t* timer = container_of(node, reminder_event_t, heap_node);

    heap_remove((struct heap*)heap,
        (struct heap_node*)node,
        timer_less_than);

    if (timer->repeat != 0) {
        ctx_t *ctx = timer->ctx;
        timer->start_id = ctx->counter++;
        timer->timeout = timer->timeout + timer->repeat;
        heap_insert((struct heap*)heap,
            (struct heap_node*)node,
            timer_less_than);
    }
    return timer;
}

static INLINE int pipe_is_empty(ctx_t *ctx, queue_category category)
{
    return heap_is_empty(&ctx->queue_heap[category]);
}

static void queue_heap_clear(min_heap* heap)
{
    struct heap_node* node;
    while ((node = heap_min((const struct heap*)heap)) != NULL) {
        event_queue_t* q = container_of(node, event_queue_t, heap_node);
        heap_remove((struct heap*)heap,
            (struct heap_node*)node,
            queue_less_than);
        event_queue_unref(q);
    }
}

static void timer_heap_clear(min_heap* heap)
{
    struct heap_node* node;
    while ((node = heap_min((const struct heap*)heap)) != NULL) {
        reminder_event_t* timer = container_of(node, reminder_event_t, heap_node);
        heap_remove((struct heap*)heap,
            (struct heap_node*)node,
            timer_less_than);
        event_unref((event_t*)timer);
    }
}

static event_t* bus_simualtion_dequeue(ctx_t *ctx)
{
again:
    // market data
    if (!pipe_is_empty(ctx, CATEGORY_MARKET) && ctx->saved_event == NULL) {
        event_t* e = event_pop_form_pipe(ctx, CATEGORY_MARKET);

        //if (e == NULL) goto next; // TODO
        ASSERT(e != NULL);

        if (e->event_type == SIMULATOR_STOP_EVENT) {
            ctx->is_simulator_stop = 1;
            goto again;
        }

        datetime_t bus_time = bus_get_time(ctx);
        if (e->timestamp < bus_time) {
            if (e->event_type != QUEUE_CLOSE_EVENT && e->event_type != QUEUE_OPEN_EVENT) {
                if (e->event_type != SIMULATOR_PROGRESS_EVENT) {
                    LOG_OUT_OF_ORDER(ctx_time, e);
                    goto again;
                }
            }
            e->timestamp = bus_get_time(ctx);
        }
        ctx->saved_event = e;
    }

next:
    // local clock timer
    if (!heap_is_empty(&ctx->timer_heap[CLOCK_LOCAL])) {
        if (ctx->saved_event != NULL) {
            reminder_event_t* r = (reminder_event_t*)timer_heap_peek(&ctx->timer_heap[CLOCK_LOCAL]);
            if (r->timeout <= ctx->saved_event->timestamp) {
                return (event_t*)timer_heap_pop(&ctx->timer_heap[CLOCK_LOCAL]);
            }
        }
    }

    // exchage clock timer
    if (!heap_is_empty(&ctx->timer_heap[CLOCK_EXCHANGE])
        && ctx->saved_event != NULL && event_is_tick(ctx->saved_event)) {
        reminder_event_t* r = (reminder_event_t*)timer_heap_peek(&ctx->timer_heap[CLOCK_EXCHANGE]);
        if (r->timeout <= ctx->saved_event->timestamp) {
            return (event_t*)timer_heap_pop(&ctx->timer_heap[CLOCK_EXCHANGE]);
        }
    }

    // execution
    if (!pipe_is_empty(ctx, CATEGORY_EXECUTION)) {
        return event_pop_form_pipe(ctx, CATEGORY_EXECUTION);
    }

    // service
    if (!pipe_is_empty(ctx, CATEGORY_SERVICE)) {
        return event_pop_form_pipe(ctx, CATEGORY_SERVICE);
    }

    // command
    if (!pipe_is_empty(ctx, CATEGORY_COMMAND)) {
        return event_pop_form_pipe(ctx, CATEGORY_COMMAND);
    }

    if (ctx->saved_event) {
        event_t* e = ctx->saved_event;
        ctx->saved_event = NULL;
        for (int i = 0; i < ctx->attached_count; i++) {
            if (e->event_type != QUEUE_CLOSE_EVENT && e->event_type != QUEUE_OPEN_EVENT) {
                event_queue_t* q = ctx->attached[i];
                event_ref(e);
                event_queue_push(q, e);
            }
        }
        return e;
    }

    return NULL;
}

static event_t* bus_realtime_dequeue(ctx_t *ctx)
{
    // market data
    if (!pipe_is_empty(ctx, CATEGORY_MARKET) && ctx->saved_event == NULL) {
        ctx->saved_event = event_pop_form_pipe(ctx, CATEGORY_MARKET);
    }

    // local clock timer
    if (!heap_is_empty(&ctx->timer_heap[CLOCK_LOCAL])) {
        reminder_event_t* r = (reminder_event_t*)timer_heap_peek(&ctx->timer_heap[CLOCK_LOCAL]);
        if (r->timeout <= bus_get_time(ctx)) {
            return (event_t*)timer_heap_pop(&ctx->timer_heap[CLOCK_LOCAL]);
        }
    }

    // exchage clock timer
    if (!heap_is_empty(&ctx->timer_heap[CLOCK_EXCHANGE]) && ctx->saved_event != NULL
        && event_is_tick(ctx->saved_event)) {
        reminder_event_t* r = (reminder_event_t*)timer_heap_peek(&ctx->timer_heap[CLOCK_EXCHANGE]);
        if (r->timeout <= ctx->saved_event->timestamp) {
            return (event_t*)timer_heap_pop(&ctx->timer_heap[CLOCK_EXCHANGE]);
        }
    }

    // execution
    if (!pipe_is_empty(ctx, CATEGORY_EXECUTION)) {
        return event_pop_form_pipe(ctx, CATEGORY_EXECUTION);
    }

    // command
    if (!pipe_is_empty(ctx, CATEGORY_COMMAND)) {
        return event_pop_form_pipe(ctx, CATEGORY_COMMAND);
    }

    // service
    if (!pipe_is_empty(ctx, CATEGORY_SERVICE)) {
        return event_pop_form_pipe(ctx, CATEGORY_SERVICE);
    }

    if (ctx->saved_event) {
        event_t* e = ctx->saved_event;
        ctx->saved_event = NULL;
        return e;
    }

    return NULL;
}

static inline void check_pending_queue(ctx_t *ctx)
{
    mpscq_node* node;
    while ((node = mpscq_pop(&ctx->pending_queue))) {
        event_queue_t* q = container_of(node, event_queue_t, mpsc_node);
        bus_add_queue(ctx, q);
    }
}

event_t* bus_dequeue(ctx_t *ctx)
{
    check_pending_queue(ctx);
    if (ctx->mode == BUS_SIMULATION)
        return bus_simualtion_dequeue(ctx);
    else
        return bus_realtime_dequeue(ctx);
}

reminder_event_t* bus_timer_peek(ctx_t *ctx, clock_type type)
{
    assert(type < CLOCK_LAST);
    return timer_heap_peek(&ctx->timer_heap[type]);
}

reminder_event_t* bus_timer_dequeue(ctx_t *ctx, clock_type type)
{
    assert(type < CLOCK_LAST);
    return timer_heap_pop(&ctx->timer_heap[type]);
}

void bus_attach(ctx_t* src, ctx_t* dst)
{
    int num = src->attached_count++;
    src->attached[num] = ats_zalloc(sizeof(event_queue_t));
    if (src->attached[num] != NULL) {
        event_queue_init(src->attached[num], 10000, CATEGORY_MARKET);
        bus_add_queue(dst, src->attached[num]);
    }
}

void bus_detach(ctx_t* src, ctx_t* dst)
{
    for (int i = 0; i < src->attached_count; i++) {
        event_queue_t* q = src->attached[i];
        ASSERT(q != NULL);
        if (q->ctx == dst) {
            bus_remove_queue(dst, q); // todo: add test(remove q from heap when q not insert heap)
            for (int j = i; j < src->attached_count - 1; j++)
                src->attached[j] = src->attached[j + 1];
            src->attached_count--;
        }
    }
}

void bus_add_queue(ctx_t *ctx, event_queue_t* q)
{
    ASSERT(ctx != NULL);
    ASSERT(q != NULL);

    ASSERT(q->ctx == ctx || q->ctx == NULL);
    ASSERT(q->category >= 0 && q->category < CATEGORY_LAST);

    q->ctx = ctx;
    q->start_id = ctx->counter++;

    if (event_queue_is_empty(q))
        return;

    heap_insert((struct heap*)&ctx->queue_heap[q->category],
        (struct heap_node*)&q->heap_node,
        queue_less_than);
}

void bus_remove_queue(ctx_t *ctx, event_queue_t* q)
{
    ASSERT(ctx != NULL);
    ASSERT(q != NULL);

    heap_remove((struct heap*)&ctx->queue_heap[q->category],
        (struct heap_node*)&q->heap_node,
        queue_less_than);
}

void bus_add_timer(ctx_t *ctx, reminder_event_t* timer)
{
    ASSERT(ctx != NULL);
    ASSERT(timer != NULL);

    timer->ctx = ctx;
    timer->start_id = ctx->counter++;
    heap_insert((struct heap*)&ctx->timer_heap[timer->clock_type],
        (struct heap_node*)&timer->heap_node,
        timer_less_than);
}

void bus_remove_timer(ctx_t *ctx, reminder_event_t* timer)
{
    ASSERT(ctx != NULL);
    ASSERT(timer != NULL);

    heap_remove((struct heap*)&ctx->timer_heap[timer->clock_type],
        (struct heap_node*)&timer->heap_node,
        timer_less_than);
}

datetime_t bus_get_time(ctx_t *ctx)
{
    ASSERT(ctx != NULL);

    if (ctx->mode == BUS_SIMULATION)
        return ctx->time[CLOCK_LOCAL];
    else
        return datetime_now();
}

int bus_set_time(ctx_t *ctx, datetime_t time)
{
    ASSERT(ctx != NULL);

    if (ctx->mode == BUS_REALTIME)
        return -1;

    if (time < ctx->time[CLOCK_LOCAL]) {
        return -1;
    }
    ctx->time[CLOCK_LOCAL] = time;
    return 0;
}

datetime_t bus_get_exchange_time(ctx_t *ctx)
{
    ASSERT(ctx != NULL);

    return ctx->time[CLOCK_EXCHANGE];
}

int bus_set_exchange_time(ctx_t *ctx, datetime_t time)
{
    ASSERT(ctx != NULL);

    if (time < ctx->time[CLOCK_EXCHANGE]) {
        return -1;
    }
    ctx->time[CLOCK_EXCHANGE] = time;
    return 0;
}

static void events_register(void)
{
    register_event(REMINDER_EVENT, 0, "reminder", sizeof(reminder_event_t), NULL, NULL);
    register_event(ASK_EVENT, EVENT_FLAG_TICK_MASK, "ask", sizeof(ask_event_t), NULL, NULL);
    register_event(BID_EVENT, EVENT_FLAG_TICK_MASK, "bid", sizeof(bid_event_t), NULL, NULL);
    register_event(TRADE_EVENT, EVENT_FLAG_TICK_MASK, "trade", sizeof(trade_event_t), NULL, NULL);
    register_event(QUEUE_OPEN_EVENT, 0, "queue_open", sizeof(queue_open_event_t), NULL, NULL);
    register_event(QUEUE_CLOSE_EVENT, 0, "queue_close", sizeof(queue_close_event_t), NULL, NULL);
    register_event(SIMULATOR_START_EVENT, 0, "simulator_start", sizeof(simulator_start_event_t), NULL, NULL);
    register_event(SIMULATOR_STOP_EVENT, 0, "simulator_stop", sizeof(simulator_stop_event_t), NULL, NULL);
    register_event(SIMULATOR_PROGRESS_EVENT, 0, "simulator_progress", sizeof(simulator_progress_event_t), NULL, NULL);
}

void bus_init(ctx_t *ctx)
{
    ctx->mode = BUS_SIMULATION;

    mpscq_init(&ctx->pending_queue);
    for (int i = 0; i < CATEGORY_LAST; i++)
        heap_init((struct heap*)&ctx->queue_heap[i]);

    heap_init((struct heap*)&ctx->timer_heap[CLOCK_LOCAL]);
    heap_init((struct heap*)&ctx->timer_heap[CLOCK_EXCHANGE]);

    ctx->time[CLOCK_LOCAL] = MIN_DATE_TIME;
    ctx->time[CLOCK_EXCHANGE] = MIN_DATE_TIME;

    ctx->counter = 1;
    ctx->saved_event = NULL;
    ctx->is_simulator_stop = 0;

    memset(&ctx->attached, '\0', sizeof(ctx->attached));
    ctx->attached_count = 0;

    ctx->event_count = 0;

    events_register();
}

void bus_destroy(ctx_t *ctx)
{
    bus_clear(ctx);
}


void bus_clear(ctx_t *ctx)
{
    if (ctx->saved_event != NULL) {
        event_unref(ctx->saved_event);
        ctx->saved_event = NULL;
    }

    for (int i = 0; i < CATEGORY_LAST; i++)
        queue_heap_clear(&ctx->queue_heap[i]);

    for (int i = 0; i < CLOCK_LAST; i++)
        timer_heap_clear(&ctx->timer_heap[i]);

    for (int i = 0; i < ctx->attached_count; i++) {
        event_queue_t* q = ctx->attached[i];
        bus_detach(ctx, q->ctx);
        event_queue_destroy(q);
    }
    ctx->attached_count = 0;
}

void bus_reset_counts(ctx_t *ctx)
{

}


void bus_async_add_queue(ctx_t *ctx, event_queue_t *q)
{
    mpscq_push(&ctx->pending_queue, &q->mpsc_node);
    ctx_wakeup(ctx);
}
