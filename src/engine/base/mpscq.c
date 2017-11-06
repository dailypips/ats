/*
 *
 * Copyright 2016 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "mpscq.h"

void mpscq_init(mpscq_t* q)
{
    atomic_no_barrier_store(&q->head, (atomic_t)&q->sentinel);
    q->tail = &q->sentinel;
    atomic_no_barrier_store(&q->sentinel.next, (atomic_t)NULL);
}

void mpscq_destroy(mpscq_t* q)
{
    ASSERT(atomic_no_barrier_load(&q->head) == (atomic_t)&q->sentinel);
    ASSERT(q->tail == &q->sentinel);
}

void mpscq_push(mpscq_t* q, mpscq_node* n)
{
    atomic_no_barrier_store(&n->next, (atomic_t)NULL);
    mpscq_node* prev = (mpscq_node*)atomic_full_xchg(&q->head, (atomic_t)n);
    atomic_rel_store(&prev->next, (atomic_t)n);
}

mpscq_node* mpscq_pop(mpscq_t* q)
{
    bool empty;
    return mpscq_pop_and_check_end(q, &empty);
}

int mpscq_is_empty(mpscq_t* q)
{
    mpscq_node* tail = q->tail;
    mpscq_node* next = (mpscq_node*)atomic_acq_load(&tail->next);
    if (tail == &q->sentinel && next == NULL)
        return 1;
    return 0;
}

mpscq_node* mpscq_pop_and_check_end(mpscq_t* q, bool* empty)
{
    mpscq_node* tail = q->tail;
    mpscq_node* next = (mpscq_node*)atomic_acq_load(&tail->next);
    if (tail == &q->sentinel) {
        // indicates the list is actually (ephemerally) empty
        if (next == NULL) {
            *empty = true;
            return NULL;
        }
        q->tail = next;
        tail = next;
        next = (mpscq_node*)atomic_acq_load(&tail->next);
    }
    if (next != NULL) {
        *empty = false;
        q->tail = next;
        return tail;
    }
    mpscq_node* head = (mpscq_node*)atomic_acq_load(&q->head);
    if (tail != head) {
        *empty = false;
        // indicates a retry is in order: we're still adding
        return NULL;
    }
    mpscq_push(q, &q->sentinel);
    next = (mpscq_node*)atomic_acq_load(&tail->next);
    if (next != NULL) {
        q->tail = next;
        return tail;
    }
    // indicates a retry is in order: we're still adding
    *empty = false;
    return NULL;
}
