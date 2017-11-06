#include <uv.h>
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include "heap-inl.h"
#include <assert.h>

#define NUM_TICKS 102400

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

typedef struct {
        void* heap_node[3];
        uint64_t start_id;
}test_node_t;


typedef struct {
    struct heap  queue_heap;
    uint64_t counter;
}test_heap_t;

static int node_less_than(const struct heap_node* ha, const struct heap_node* hb)
{
    assert(ha != NULL);
    assert(hb != NULL);

    const test_node_t* lhs = container_of(ha, test_node_t, heap_node);
    const test_node_t* rhs = container_of(hb, test_node_t, heap_node);

    if (lhs->start_id < rhs->start_id)
        return 1;
    if (rhs->start_id < lhs->start_id)
        return 0;

    return 0;
}

BENCHMARK_IMPL(heap)
{
    test_heap_t heap;
    test_node_t *node;
    struct heap_node* m;

    uint64_t ns;

    ns = uv_hrtime();

    heap_init(&heap.queue_heap);

    for (int i = 0; i < NUM_TICKS; i++)
        node = malloc(sizeof(test_node_t));
        node->start_id = heap.counter++;
        heap_insert((struct heap*)&heap.queue_heap,
            (struct heap_node*)&node->heap_node,
            node_less_than);

   while((m =  heap_min(&heap.queue_heap)) != NULL) {
       heap_dequeue(&heap.queue_heap, node_less_than);

   }

   ns = uv_hrtime() - ns;

   fprintf(stderr, "heap: %d ticks in %.2fs (%.0f_10K/s)\n",
           NUM_TICKS,
           ns / 1e9,
           NUM_TICKS / (ns / 1e9) / 10000);
   fflush(stderr);

    return 0;
}
