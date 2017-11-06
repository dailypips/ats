/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/
#include <uv.h>
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include "mpscq.h"
#include <assert.h>
#include "event_queue.h"

#define NUM_TICKS 10240000

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))


BENCHMARK_IMPL(queue)
{
    event_queue_t q;

    event_queue_init(&q, 10000000, CATEGORY_MARKET);

    uint64_t ns;

    ns = uv_hrtime();

    for (int i = 0; i < NUM_TICKS; i++)
        event_queue_push(&q, &q);

   ns = uv_hrtime() - ns;

   fprintf(stderr, "spsc: %d ticks in %.2fs (%.0f_10K/s)\n",
           NUM_TICKS,
           ns / 1e9,
           NUM_TICKS / (ns / 1e9) / 10000);
   fflush(stderr);


    mpscq_t mq;
    mpscq_node node;
    mpscq_init(&mq);


    ns = uv_hrtime();
     for (int i =0; i < NUM_TICKS; i++)
    mpscq_push(&mq, &node);
    ns = uv_hrtime() - ns;

    fprintf(stderr, "mpsc: %d ticks in %.2fs (%.0f_10K/s)\n",
        NUM_TICKS,
        ns / 1e9,
        NUM_TICKS / (ns / 1e9) / 10000);
    fflush(stderr);

    return 0;
}
