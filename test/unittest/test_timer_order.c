#include "event_bus.h"
#include "task.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "context.h"

#define QUEUE_SIZE 3
#define EVENT_SIZE 30

static datetime_t etime[EVENT_SIZE] = {
    8, 10, 34, 19, 11, 33, 25, 39, 4, 30,
    1, 6, 13, 3, 28, 24, 8, 10,
    1, 13, 24, 10, 30, 38, 1, 14, 20, 39, 17, 16

};

typedef struct {
    datetime_t time;
    int qindex;
} result_except_t;

static datetime_t except[EVENT_SIZE] = {
    1,1,1,3,4,6,8,8,10,10,10,11,13,13,14,16,17,19,20,24,24,25,28,30,30,33,34,38,39,39
};

static void on_reminder(ctx_t *ctx, reminder_event_t *r)
{
    (void)(ctx);
    (void)(r);
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

static int test_timer_event_order(bus_mode mode, clock_type ctype)
{
    fprintf(stderr, "mode = %d clock_type = %d\n", mode, ctype);
    ctx_t ctx;

    reminder_event_t* event[EVENT_SIZE];

    ctx_init(&ctx);
    ctx.mode = mode;

    /* prepare event */
    for (int i = 0; i < EVENT_SIZE; i++) {
        event[i] = (reminder_event_t*)event_zalloc(REMINDER_EVENT);
        timer_init(event[i], ctype, etime[i]);
        bus_add_timer(&ctx, event[i]);
    }

    /* test timer order */
    int j = 0;

    reminder_event_t *e;

    while ((e = bus_timer_dequeue(&ctx, ctype)) != NULL) {

        TASSERT_THEN(e->timestamp == except[j],
                    fprintf(stderr, "[%d] except %"PRIu64" but %"PRIu64"\n", j, except[j], e->timestamp);
                );
        event_unref((event_t*)e);
        j++;
    }

    TASSERT(j == EVENT_SIZE);
    /* TASSERT event lifetime */
    for (int i = 0; i < EVENT_SIZE; i++) {
        TASSERT(event[i]->refcount == 0);
    }

    /* clear bus*/

    ctx_destroy(&ctx);

    return 0;
}

TEST_IMPL(bus_signle_timer_event_order)
{
    // test diff mode && diff pipe

    for (int z = BUS_SIMULATION; z < BUS_MODE_LAST; z++) {
        for (int k = CLOCK_LOCAL; k < CLOCK_LAST; k++) {
            test_timer_event_order(z, k);
        }
    }
    return 0;
}
