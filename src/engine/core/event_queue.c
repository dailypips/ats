/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#include "event_queue.h"
#include "atomic.h"
#include "event_bus.h"

event_queue_t *event_queue_init(event_queue_t *q, unsigned int size,
                            queue_category category) {
  int ret = spscq_init(&q->spsc, size);
  if (ret < 0) {
    return NULL;
  }
  q->ctx = NULL;
  q->category = category;
  return q;
}

void event_queue_destroy(event_queue_t *q) {
  event_t *e;
  while ((e = event_queue_pop(q)))
    event_unref(e);

  spscq_destroy(&q->spsc);
}

static void queue_free(event_queue_t *q) {
  event_queue_destroy(q);
  free(q);
}

void event_queue_ref(event_queue_t *q) {
  assert(q != NULL);
  atomic_full_fetch_add(&q->refcount, 1);
}

void event_queue_unref(event_queue_t *q) {
  assert(q != NULL);
  if (atomic_full_fetch_add(&q->refcount, -1) == 1)
    queue_free(q);
}
/*
 * 如果向空队列压入事件，必须通知相应的bus
 * 通知bus是异步的，通过把队列压入bus的pending_queue来实现
 * 如果要简化生命周期管理的话，queue_t必须是MPSC的节点子类型
 * 同时pipe通过最小堆管理queue，所以queue_t必须是heap的节点子类型
 * 如果队列内容为空，heap并不保存这个queue的指针，
 * queue的关闭是通过push QUEUE_CLOSED事件来完成
 */
/* only for producer */
int event_queue_push(event_queue_t *q, void *e) {
  int is_empty = 0;
  if (spscq_is_empty(&q->spsc))
    is_empty = 1;

  int ret = spscq_push(&q->spsc, e);

  if (is_empty && q->ctx) {
    ASSERT(ret == 0);
    bus_async_add_queue(q->ctx, q);
  }

  return ret;
}

AINLINE unsigned int event_queue_size(event_queue_t *q) {
  return spscq_size(&q->spsc);
}

AINLINE unsigned int event_queue_capacity(event_queue_t *q) {
  return spscq_capacity(&q->spsc);
}

AINLINE void *event_queue_pop(event_queue_t *q) { return spscq_pop(&q->spsc); }

AINLINE void *event_queue_peek(const event_queue_t *q) {
  return spscq_peek(&q->spsc);
}

AINLINE int event_queue_is_empty(event_queue_t *q) {
  return spscq_is_empty(&q->spsc);
}

AINLINE int event_queue_is_full(event_queue_t *q) {
  return spscq_is_full(&q->spsc);
}
