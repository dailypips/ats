/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef __EVENT_BUS_H__
#define __EVENT_BUS_H__

#include "mem.h"
#include "datetime.h"
#include "event.h"
#include "mpscq.h"
#include "spscq.h"
#include "event_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void *min;
  unsigned int nelts;
} min_heap;

typedef enum {
    BUS_SIMULATION,
    BUS_REALTIME,
    BUS_MODE_LAST
} bus_mode;

typedef enum {
    CLOCK_LOCAL  = 0,
    CLOCK_EXCHANGE,
    CLOCK_LAST
}clock_type;

#define EVENT_BUS_FIELDS                                                       \
  bus_mode mode;                                                            \
  mpscq_t pending_queue;                                                       \
  min_heap queue_heap[CATEGORY_LAST];                                       \
  min_heap timer_heap[CLOCK_LAST];                                          \
  uint64_t counter;                                                            \
  datetime_t time[CLOCK_LAST];                                               \
  event_t *saved_event;                                                     \
  event_queue_t *attached[128];                                                   \
  int attached_count;                                                          \
  int is_simulator_stop;    \
      long event_count;


typedef struct bus_s bus_t;

typedef struct reminder_event_s reminder_event_t;

typedef void (*reminder_cb)(ctx_t *ctx, reminder_event_t *timer);

struct reminder_event_s {
    EVENT_COMMON_FIELDS
    clock_type clock_type;
    reminder_cb callback;
    void *user_data;
    datetime_t timeout;
    uint64_t repeat;
    /*private*/
    void *ctx;
    void* heap_node[3];
    uint64_t start_id;
};

#define TICK_EVENT_FIELDS \
    EVENT_COMMON_FIELDS \
    datetime_t exchange_timestamp; \
    int instrument_id; \
    int provider_id; \
    double price; \
    long size;

typedef struct tick_event_s {
    TICK_EVENT_FIELDS
}tick_event_t;

typedef struct tick_event_s ask_event_t;
typedef struct tick_event_s bid_event_t;

typedef struct trade_event_s {
    TICK_EVENT_FIELDS
    int direction;
}trade_event_t;

typedef struct queue_open_event_s {
    EVENT_COMMON_FIELDS
    event_queue_t *q;
}queue_open_event_t;

typedef struct queue_close_event_s {
    EVENT_COMMON_FIELDS
    event_queue_t *q;
}queue_close_event_t;

typedef struct simulator_start_event_s {
    EVENT_COMMON_FIELDS
}simulator_start_event_t;

typedef struct simulator_stop_event_s {
    EVENT_COMMON_FIELDS
}simulator_stop_event_t;

typedef struct simulator_progress_event_s {
    EVENT_COMMON_FIELDS
}simulator_progress_event_t;

struct bus_s {
EVENT_BUS_FIELDS
};

#define EVENT_BUS_INIT(ctx)         bus_init((ctx))
#define EVENT_BUS_DESTROY(ctx)      bus_destroy((ctx))

FUNC void bus_init(ctx_t *ctx);
FUNC void bus_destroy(ctx_t *ctx);
FUNC void bus_clear(ctx_t *ctx);

FUNC void bus_attach(ctx_t *src, ctx_t *dst);
FUNC void bus_detach(ctx_t *src, ctx_t *dst);

FUNC void bus_add_queue(ctx_t *ctx, event_queue_t *q);
FUNC void bus_remove_queue(ctx_t *ctx, event_queue_t *q);

FUNC void bus_async_add_queue(ctx_t *ctx, event_queue_t *q);

FUNC void bus_add_timer(ctx_t *ctx, reminder_event_t *timer);
FUNC void bus_remove_timer(ctx_t *ctx, reminder_event_t *timer);

FUNC event_t *bus_dequeue(ctx_t *ctx);
FUNC reminder_event_t *bus_timer_peek(ctx_t *ctx, clock_type type);
FUNC reminder_event_t *bus_timer_dequeue(ctx_t *ctx, clock_type type);

FUNC datetime_t bus_get_time(ctx_t *ctx);
FUNC int bus_set_time(ctx_t *ctx, datetime_t time);

FUNC datetime_t bus_get_exchange_time(ctx_t *ctx);
FUNC int bus_set_exchange_time(ctx_t *ctx, datetime_t time);

FUNC void bus_reset_counts(ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif // __EVENT_BUS_H__
