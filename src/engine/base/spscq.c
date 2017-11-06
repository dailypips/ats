/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#include "spscq.h"
#include "atomic.h"
#include "mem.h"

int spscq_init(spscq_t* q, uint32_t size)
{
    ASSERT(size >= 2);
    q->records = ats_malloc(size * sizeof(void*));
    if (!q->records)
        return -1;

    q->capacity = size;
    atomic_no_barrier_store(&q->read_pos, 0);
    atomic_no_barrier_store(&q->write_pos, 0);

    return 0;
}

void spscq_destroy(spscq_t* q)
{
    // No real synchronization needed at destructor time: only one
    // thread can be doing this.
    ASSERT(spscq_is_empty(q) == 1);
    ats_free(q->records);
}

int spscq_push(spscq_t* q, void* e)
{
    unsigned int currentWrite = atomic_no_barrier_load(&q->write_pos);
    unsigned int nextRecord = currentWrite + 1;
    if (nextRecord == q->capacity)
        nextRecord = 0;

    if (nextRecord != atomic_acq_load(&q->read_pos)) {
        q->records[currentWrite] = e;
        atomic_rel_store(&q->write_pos, nextRecord);
        return 0;
    }

    // queue if full
    return -1;
}

void* spscq_pop(spscq_t* q)
{
    unsigned int currentRead = atomic_no_barrier_load(&q->read_pos);
    if (currentRead == atomic_acq_load(&q->write_pos)) {
        // queue is empty
        return NULL;
    }

    unsigned int nextRecord = currentRead + 1;
    if (nextRecord == q->capacity)
        nextRecord = 0;
    atomic_rel_store(&q->read_pos, nextRecord);

    return q->records[currentRead];
}

void* spscq_peek(const spscq_t* q)
{
    unsigned int currentRead = atomic_no_barrier_load(&q->read_pos);
    if (currentRead == atomic_acq_load(&q->write_pos)) {
        // queue is empty
        return NULL;
    }

    return q->records[currentRead];
}

int spscq_is_empty(spscq_t* q)
{
    return atomic_acq_load(&q->read_pos) == atomic_acq_load(&q->write_pos);
}

int spscq_is_full(spscq_t* q)
{
    unsigned int nextRecord = atomic_acq_load(&q->write_pos) + 1;
    if (nextRecord == q->capacity)
        nextRecord = 0;

    if (nextRecord != atomic_acq_load(&q->read_pos)) {
        return 0;
    }

    // queue is full
    return 1;
}

unsigned int spscq_size(spscq_t* q)
{
    int ret = atomic_acq_load(&q->write_pos) - atomic_acq_load(&q->read_pos);
    if (ret < 0)
        ret += q->capacity;
    return ret;
}

unsigned int spscq_capacity(spscq_t* q) { return q->capacity; }
