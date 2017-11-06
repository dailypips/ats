#include "event_bus.h"
#include "context.h"
#include "task.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define QUEUE_SIZE 3
#define EVENT_SIZE 30
#define TIMER_SIZE 5

static datetime_t etime[EVENT_SIZE] = {
    8, 10, 34, 19, 11, 33, 25, 39, 4, 30,
    1, 6, 13, 3, 28, 24, 8, 10,
    1, 13, 24, 10, 30, 38, 1, 14, 20, 39, 17, 16

};

static datetime_t timer_time[TIMER_SIZE] = {
    15, 30, 25, 9, 11
};

typedef struct {
    datetime_t time;
    int qindex;
} result_except_t;

static result_except_t except[EVENT_SIZE] = {
    {1, 2}, {13, 2}, {24, 2}, {10, 2}, {30, 2}, {38, 2}, {1, 2}, {14, 2}, {20, 2}, {39, 2}, {17, 2}, {16, 2},
    {1, 1}, {6, 1}, {8, 0}, {10, 0}, {13, 1}, {3, 1}, {28, 1}, {24, 1}, {8, 1}, {10, 1},
    {34, 0}, {19, 0}, {11, 0}, {33, 0}, {25, 0}, {39, 0}, {4, 0}, {30, 0}
};

static datetime_t except_timer[TIMER_SIZE] = {
    9, 11, 15, 25, 30
};

static datetime_t all_time[EVENT_SIZE + TIMER_SIZE] = {
    1, 13, 24, 10, 30, 38, 1, 14, 20, 39, 17, 16, // first pop all execution message
    1, 6, 8, 9, // last is timer
    10, 11,     // last is timer
    13, 3, 15, 25, // last and before is timer
    28, 24, 8, 10, 30, // last is timer
    34, 19, 11, 33, 25, 39, 4, 30
};
static void on_reminder(ctx_t *ctx, reminder_event_t *r)
{
    (void)(ctx);
    (void)(r);
}

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

static void timer_init(reminder_event_t* e, clock_type ctype, datetime_t timeout)
{
    e->clock_type = ctype;
    e->callback = on_reminder;
    e->user_data = e;
    e->timestamp = timeout;
    e->timeout = timeout;
    e->repeat = 0;
}

static int test_event_order(bus_mode mode)
{
    ctx_t ctx;

    event_queue_t queue[QUEUE_SIZE];

    event_t* event[EVENT_SIZE];
    reminder_event_t* timer[TIMER_SIZE];

    for (int i = 0; i < QUEUE_SIZE; i++) {
        event_queue_init(&queue[i], 10000, CATEGORY_MARKET);
    }

    ctx_init(&ctx);
    ctx.mode = mode;

    /* prepare event */
    for (int i = 0; i < EVENT_SIZE; i++) {
        event[i] = event_zalloc(ASK_EVENT);
        tick_init(event[i], i, i + 10000, 0, 1.0, i);
        event[i]->timestamp = etime[i];
    }

    for (int i = 0; i < TIMER_SIZE; i++) {
        timer[i] = (reminder_event_t*)event_zalloc(REMINDER_EVENT);
        timer_init(timer[i], CLOCK_LOCAL, timer_time[i]);
        bus_add_timer(&ctx, timer[i]);
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
        queue[2].category = CATEGORY_EXECUTION;
        ((tick_event_t*)event[i])->provider_id = 2;
    }

    bus_add_queue(&ctx, &queue[0]);
    bus_add_queue(&ctx, &queue[1]);
    bus_add_queue(&ctx, &queue[2]);

    /* test event order */
    event_t* e;

    int k = 0;
    int z= 0;
    while ((e = bus_dequeue(&ctx)) != NULL) {
        TASSERT(e->timestamp == all_time[z]);
        if (e->event_type == ASK_EVENT)
        {
#ifndef NDEBUG
            TASSERT_THEN(except[k].qindex == ((tick_event_t*)e)->provider_id,
                    fprintf(stderr, "event index:%d time:%" PRIu64 " except [%d] but [%d]\n", k, e->timestamp, except[k].qindex, ((tick_event_t*)e)->provider_id);
            );
#endif
            k++;
        }
        event_unref(e);
        z++;
    }
    TASSERT(z == TIMER_SIZE + EVENT_SIZE);
    TASSERT(k == EVENT_SIZE);
    /* TASSERT event lifetime */
    for (int i = 0; i < EVENT_SIZE; i++) {
        TASSERT(event[i]->refcount == 0);
    }

    for (int i = 0; i < TIMER_SIZE; i++) {
        TASSERT(timer[i]->refcount == 0);
    }

    /* clear bus*/

    ctx_destroy(&ctx);
    return 0;
}

TEST_IMPL(bus_full_event_order)
{
    test_event_order(BUS_SIMULATION);

    return 0;
}
