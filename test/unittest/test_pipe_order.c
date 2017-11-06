#include "task.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "context.h"

#define QUEUE_SIZE 3
#define EVENT_SIZE 30

/* random [rmin, rmax] */
static int random_range(int rmin, int rmax)
{
    return (rand() % (rmax - rmin + 1)) + rmin;
}

static datetime_t etime[EVENT_SIZE] = {
    8, 10, 34, 19, 11, 33, 25, 39, 4, 30,
    1, 6, 13, 3, 28, 24, 8, 10,
    1, 13, 24, 10, 30, 38, 1, 14, 20, 39, 17, 16

};

typedef struct {
    datetime_t time;
    int qindex;
} result_except_t;

static result_except_t except[EVENT_SIZE] = {
    { 1, 1 }, { 1, 2 }, { 6, 1 }, { 8, 0 }, { 10, 0 }, { 13, 2 }, { 13, 1 }, { 3, 1 },
    { 24, 2 }, { 10, 2 }, { 28, 1 }, { 24, 1 }, { 8, 1 }, { 10, 1 },
    { 30, 2 }, { 34, 0 }, { 19, 0 }, { 11, 0 }, { 33, 0 }, { 25, 0 },
    { 38, 2 }, { 1, 2 }, { 14, 2 }, { 20, 2 }, { 39, 0 }, { 4, 0 }, { 30, 0 },
    { 39, 2 }, { 17, 2 }, { 16, 2 }
};

static event_t* tick_init(event_t* e, int provider, int instrument, datetime_t exchange_timestamp, double price, long size)
{
    tick_event_t* tick = (tick_event_t*)e;
    tick->exchange_timestamp = exchange_timestamp;
    tick->instrument_id = instrument;
    tick->provider_id = provider;
    tick->price = price;
    tick->size = size;
    return e;
}

static int test_pipe_event_order(bus_mode mode, queue_category category)
{
    ctx_t ctx;

    event_queue_t queue[QUEUE_SIZE];

    event_t* event[EVENT_SIZE];

    for (int i = 0; i < QUEUE_SIZE; i++) {
        event_queue_init(&queue[i], 10000, category);
    }

    ctx_init(&ctx);
    ctx.mode = mode;

    /* prepare event */
    for (int i = 0; i < EVENT_SIZE; i++) {
        event[i] = event_zalloc(ASK_EVENT);
        tick_init(event[i], i, i + 10000, 0, 1.0, i);
        event[i]->timestamp = etime[i];
    }

    for (int i = 0; i < 10; i++) {
        event_queue_push(&queue[0], event[i]);
        ((tick_event_t*)event[i])->provider_id = 0;
    }

    for (int i = 10; i < 18; i++) {
        event_queue_push(&queue[1], event[i]);
        ((tick_event_t*)event[i])->provider_id = 1;
    }

    for (int i = 18; i < 30; i++) {
        event_queue_push(&queue[2], event[i]);
        ((tick_event_t*)event[i])->provider_id = 2;
    }

    bus_add_queue(&ctx, &queue[0]);
    bus_add_queue(&ctx, &queue[1]);
    bus_add_queue(&ctx, &queue[2]);

    /* test event order */
    event_t* e;
    int j = 0;

    while ((e = bus_dequeue(&ctx)) != NULL) {
        TASSERT(e->timestamp == except[j].time);
#ifndef NDEBUG
        TASSERT_THEN(except[j].qindex == ((tick_event_t*)e)->provider_id,
                fprintf(stderr, "event index:%d time:%" PRIu64 " except [%d] but [%d]\n", j, e->timestamp, except[j].qindex, ((tick_event_t*)e)->provider_id);
        );
#endif
        event_unref(e);
        j++;
    }

    TASSERT(j == EVENT_SIZE);
    /* TASSERT event lifetime */
    for (int i = 0; i < EVENT_SIZE; i++) {
        TASSERT(event[i]->refcount == 0);
    }

    TASSERT(j == EVENT_SIZE);

    /* clear bus*/

    ctx_destroy(&ctx);
    return 0;
}

TEST_IMPL(bus_signle_pipe_event_order)
{
    // test diff mode && diff pipe

    for (int z = BUS_SIMULATION; z < BUS_MODE_LAST; z++) {
        for (int k = CATEGORY_MARKET; k < CATEGORY_LAST; k++) {
            test_pipe_event_order(z, k);
        }
    }
    return 0;
}
