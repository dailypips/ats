#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include "context.h"
#include <assert.h>


TEST_IMPL(ctx_test)
{
    ctx_t ctx;

    ctx_init(&ctx);

    ctx_run(&ctx, RUN_NOWAIT);

    return 0;
}
