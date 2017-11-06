/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include "mpscq.h"
#include "spscq.h"
#include "mem.h"
#include "event.h"

typedef enum {
    CATEGORY_MARKET = 0,
    CATEGORY_EXECUTION,
    CATEGORY_SERVICE,
    CATEGORY_COMMAND,
    CATEGORY_LAST
}queue_category;

typedef struct event_queue_s event_queue_t;

struct event_queue_s {
  atomic_t refcount;
  void *data;
  /* private */
  spscq_t spsc;
  /* only access by consumer */
  void *ctx;
  queue_category category;
  /* mpsc */
  mpscq_node mpsc_node;
  /* min heap node */
  void *heap_node[3];
  uint64_t start_id;
};

EXTERN event_queue_t* event_queue_new(size_t size, queue_category category);

FUNC event_queue_t *event_queue_init(event_queue_t *q, unsigned int size,
                                  queue_category category);
FUNC void event_queue_destroy(event_queue_t *q);

FUNC void event_queue_ref(event_queue_t *q);
FUNC void event_queue_unref(event_queue_t *q);

FUNC unsigned int event_queue_size(event_queue_t *q);
FUNC unsigned int event_queue_capacity(event_queue_t *q);

EXTERN int event_queue_push(event_queue_t *q, void *e);
FUNC void *event_queue_peek(const event_queue_t *q);
FUNC void *event_queue_pop(event_queue_t *q);

FUNC int event_queue_is_empty(event_queue_t *q);
FUNC int event_queue_is_full(event_queue_t *q);

#endif // __EVENT_QUEUE_H__
